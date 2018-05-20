const fs = require('fs');
const {dialog} = require('electron').remote;


let map = {
    map: null,
    vectorLayer: null,

    drawPath: function() {
        this.vectorLayer.destroyFeatures();
        for(let c=0; c<planning.waypoints.length; c++) {
            let position = new OpenLayers.Geometry.Point(planning.waypoints[c].lon, planning.waypoints[c].lat);
            position.transform("EPSG:4326", this.map.getProjectionObject());
            let point = new OpenLayers.Feature.Vector(position,
                {
                    salutation: "",
                    Lon: planning.waypoints[c].lon,
                    Lat: planning.waypoints[c].lat
                });

            this.vectorLayer.addFeatures(point);

            if (c > 0) {
                let lastPoint = new OpenLayers.Geometry.Point(
                    planning.waypoints[c - 1].lon,
                    planning.waypoints[c - 1].lat
                );
                lastPoint.transform("EPSG:4326", this.map.getProjectionObject());
                let linePoints = Array(
                    lastPoint,
                    position
                );
                let line = new OpenLayers.Feature.Vector(new OpenLayers.Geometry.LineString(linePoints));
                this.vectorLayer.addFeatures(line);
            }
        }
    },

    init: function () {
        OpenLayers.Control.Click = OpenLayers.Class(OpenLayers.Control, {
            defaultHandlerOptions: {
                'single': true,
                'double': false,
                'pixelTolerance': 0,
                'stopSingle': false,
                'stopDouble': false
            },

            initialize: function (options) {
                this.handlerOptions = OpenLayers.Util.extend(
                    {}, this.defaultHandlerOptions
                );
                OpenLayers.Control.prototype.initialize.apply(
                    this, arguments
                );
                this.handler = new OpenLayers.Handler.Click(
                    this, {
                        'click': this.trigger
                    }, this.handlerOptions
                );
            },

            trigger: function (e) {
                //A click happened!
                let lonlat = map.map.getLonLatFromViewPortPx(e.xy);

                lonlat.transform(
                    new OpenLayers.Projection("EPSG:900913"),
                    new OpenLayers.Projection("EPSG:4326")
                );

                planning.addWaypoint(lonlat.lat, lonlat.lon,function () {
                    map.drawPath();
                });
            }
        });


        this.map = new OpenLayers.Map("map");
        let fromProjection = new OpenLayers.Projection("EPSG:4326");   // Transform from WGS 1984
        let toProjection = new OpenLayers.Projection("EPSG:900913"); // to Spherical Mercator Projection
        let position = new OpenLayers.LonLat(9.9300282, 48.4242041).transform(fromProjection, toProjection);

        this.map.addLayer(new OpenLayers.Layer.OSM());
        this.map.setCenter(position, 15);

        this.vectorLayer = new OpenLayers.Layer.Vector("Vector Layer", {
            projection: "EPSG:4326",
            style: {
                fillColor: "#0000FF",
                pointRadius: "8px"
            }
        });
        this.map.addLayer(this.vectorLayer);


        let click = new OpenLayers.Control.Click();
        this.map.addControl(click);
        click.activate();
    },

};


let planning = {
    Waypoint: function (lat, lon, height, maxDelta, landingAllowed) {
        this.lat = lat;
        this.lon = lon;
        this.height = height;
        this.maxDelta = maxDelta;
        this.landingAllowed = landingAllowed;
    },

    waypoints: Array(),

    addWaypoint: function (lat, lon, callback) {
        $.getJSON("https://maps.googleapis.com/maps/api/elevation/json?" +
            "locations="+lat+","+lon+"&key=AIzaSyDnZRJg6wth7Rchyq8K0y7WkfmqJNAEyfQ", function (res) {
            let height  = res.results[0].elevation + Number($("#inputHeight").val());
            let maxDelta = Number($("#inputMaxDelta").val());
            let landingAllowed = false;

            planning.waypoints.push(new planning.Waypoint(lat, lon, height, maxDelta, landingAllowed));

            $("#listBody").append(
                "<tr>" +
                "<td contenteditable=\'true\'>"+lat.toFixed(3)+"</td>" +
                "<td contenteditable=\'true\'>"+lon.toFixed(3)+"</td>" +
                "<td contenteditable=\'true\'>"+height.toFixed(1)+"</td>" +
                "<td contenteditable=\'true\'>"+maxDelta.toFixed(1)+"</td>" +
                "<td contenteditable=\'true\'>"+(landingAllowed?"Yes":"No")+"</td>" +
                "<td><a class=\"icon icon-cancel-circled buttonDelete\" href='#'></a></td>"+
                "</tr>"
            );
            callback();
        });


    },

    save: function (filename) {
        let string = JSON.stringify(this.waypoints, null, 4);
        fs.writeFile(filename, string, function (error) {
            if(error) {
                console.warn("Errror writing file!");
            }
        })
    }
};


$(document).ready(function () {
    map.init();

    $("#buttonSave").click(function () {
        let filename = dialog.showSaveDialog({
            "title": "Save Mission",
            "defaultPath": "mission.json"
        });
        planning.save(filename);
    });


    $("#listBody").on("input", function (event) {
        let col = $(event.target).index();
        let row = $(event.target).closest('tr').index();
        let newVal = $(event.target).text();
        switch (col) {
            case 0:
                planning.waypoints[row].lat = Number(newVal);
                break;
            case 1:
                planning.waypoints[row].lon = Number(newVal);
                break;
            case 2:
                planning.waypoints[row].height = Number(newVal);
                break;
            case 3:
                planning.waypoints[row].maxDelta = Number(newVal);
                break;
            case 4:
                planning.waypoints[row].landingAllowed = newVal === "Yes";
                break;
            default:
                console.warn("Event handling fuckup");
        }
        map.drawPath();
    });

    $(document).on("click", ".buttonDelete", function (event) {
        let row = $(event.target).closest('tr');
        planning.waypoints.splice(row.index(), 1);
        map.drawPath();
        row.remove();
    })
});