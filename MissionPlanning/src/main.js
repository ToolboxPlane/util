const fs = require('fs');
const {dialog} = require('electron').remote;


let map = {
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
                let lonlat = map.getLonLatFromViewPortPx(e.xy);

                lonlat.transform(
                    new OpenLayers.Projection("EPSG:900913"),
                    new OpenLayers.Projection("EPSG:4326")
                );


                let position = new OpenLayers.Geometry.Point(lonlat.lon, lonlat.lat);
                position.transform("EPSG:4326", map.getProjectionObject());
                let feature = new OpenLayers.Feature.Vector(position,
                    {
                        salutation: "",
                        Lon: lonlat.lon,
                        Lat: lonlat.lat
                    });

                vectorLayer.addFeatures(feature);

                if (planning.waypoints.length > 0) {
                    let lastPoint = new OpenLayers.Geometry.Point(
                        planning.waypoints[planning.waypoints.length - 1].lon,
                        planning.waypoints[planning.waypoints.length - 1].lat
                    );
                    lastPoint.transform("EPSG:4326", map.getProjectionObject());
                    let linePoints = Array(
                        lastPoint,
                        position
                    );
                    let line = new OpenLayers.Feature.Vector(new OpenLayers.Geometry.LineString(linePoints));
                    vectorLayer.addFeatures(line);
                }
                planning.addWaypoint(lonlat.lat, lonlat.lon);
            }
        });


        map = new OpenLayers.Map("map");
        let mapnik = new OpenLayers.Layer.OSM();
        let fromProjection = new OpenLayers.Projection("EPSG:4326");   // Transform from WGS 1984
        let toProjection = new OpenLayers.Projection("EPSG:900913"); // to Spherical Mercator Projection
        let position = new OpenLayers.LonLat(9.9300282, 48.4242041).transform(fromProjection, toProjection);
        let zoom = 15;

        map.addLayer(mapnik);
        map.setCenter(position, zoom);

        let markers = new OpenLayers.Layer.Markers("Markers");
        map.addLayer(markers);

        let vectorLayer = new OpenLayers.Layer.Vector("Vector Layer", {
            projection: "EPSG:4326",
            style: {
                fillColor: "#0000FF",
                pointRadius: "8px"
            }
        });
        map.addLayer(vectorLayer);


        let click = new OpenLayers.Control.Click();
        map.addControl(click);
        click.activate();
    }
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

    addWaypoint: function (lat, lon) {
        $.getJSON("https://maps.googleapis.com/maps/api/elevation/json?" +
            "locations="+lat+","+lon+"&key=AIzaSyDnZRJg6wth7Rchyq8K0y7WkfmqJNAEyfQ", function (res) {
            let height  = res.results[0].elevation + Number($("#inputHeight").val());
            let maxDelta = Number($("#inputMaxDelta").val());
            let landingAllowed = false;

            planning.waypoints.push(new planning.Waypoint(lat, lon, height, maxDelta, landingAllowed));

            $("#listBody").append(
                "<tr>" +
                "<td>"+lat.toFixed(3)+"</td>" +
                "<td>"+lon.toFixed(3)+"</td>" +
                "<td>"+height.toFixed(1)+"m</td>" +
                "<td>"+maxDelta.toFixed(1)+"m</td>" +
                "<td>"+(landingAllowed?"Yes":"No")+"</td>" +
                "</tr>"
            );
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
});