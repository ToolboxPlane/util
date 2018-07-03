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
                    salutation: "Test",
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
                'let': false,
                'pixelTolerance': 0,
                'stopSingle': false,
                'stoplet': false
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

                planning.addWaypoint(lonlat.lat, lonlat.lon);
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
    Waypoint: function (lat, lon, altitude, maxDelta, landingAllowed) {
        this.lat = lat;
        this.lon = lon;
        this.altitude = altitude;
        this.maxDelta = maxDelta;
        this.landingAllowed = landingAllowed;

        this.distanceTo = function (wp) {
            const EARTH_RADIUS = 6371E3;
            let phi1 = this.lon / 180 * Math.PI;
            let sigma1 = Math.PI/2 - (this.lat / 180 * Math.PI);
            let r1 = EARTH_RADIUS + this.altitude;
            let x1 = r1 * Math.sin(sigma1) * Math.cos(phi1);
            let y1 = r1 * Math.sin(sigma1) * Math.sin(phi1);
            let z1 = r1 * Math.cos(sigma1);

            let phi2 = wp.lon / 180 * Math.PI;
            let sigma2 = Math.PI/2 - (wp.lat / 180 * Math.PI);
            let r2 = EARTH_RADIUS + wp.altitude;
            let x2 = r2 * Math.sin(sigma2) * Math.cos(phi2);
            let y2 = r2 * Math.sin(sigma2) * Math.sin(phi2);
            let z2 = r2 * Math.cos(sigma2);

            let dx = x1 - x2;
            let dy = y1 - y2;
            let dz = z1 - z2;

            return Math.sqrt(dx*dx + dy*dy + dz*dz);
        }
    },

    waypoints: Array(),

    addWaypoint: function (lat, lon) {
        $.getJSON("https://maps.googleapis.com/maps/api/elevation/json?" +
            "locations="+lat+","+lon+"&key="+Config.MAPS_API_KEY, function (res) {
            let altitude  = res.results[0].elevation + Number($("#inputHeight").val());
            let maxDelta = Number($("#inputMaxDelta").val());
            let landingAllowed = false;

            planning.waypoints.push(new planning.Waypoint(lat, lon, altitude, maxDelta, landingAllowed));

            planning.updateAllViews();
        });
    },

    removeWaypoint: function(index) {
        planning.waypoints.splice(index, 1);
        this.updateAllViews();
    },

    updateAllViews: function() {
        let text = "";
        for(let c=0; c<this.waypoints.length; c++) {
            text += "<tr>" +
                "<td contenteditable=\'true\'>"+this.waypoints[c].lat.toFixed(3)+"</td>" +
                "<td contenteditable=\'true\'>"+this.waypoints[c].lon.toFixed(3)+"</td>" +
                "<td contenteditable=\'true\'>"+this.waypoints[c].altitude.toFixed(1)+"</td>" +
                "<td contenteditable=\'true\'>"+this.waypoints[c].maxDelta.toFixed(1)+"</td>" +
                "<td contenteditable=\'true\'>"+(this.waypoints[c].landingAllowed?"Yes":"No")+"</td>" +
                "<td><a class=\"icon icon-cancel-circled buttonDelete\" href='#'></a></td>"+
                "</tr>"
        }
        $("#listBody").html(text);

        map.drawPath();
        $("#spanDistance").text(planning.getLength().toFixed(0) + "m");
    },

    save: function (filename) {
        let text = "0\nTimestamp; Lat; Lon; Altitude; MaxDelta; LandingAllowed";
        for(let c=0; c<this.waypoints.length; c++) {
            text += "\n0; " + this.waypoints[c].lat + "; " + this.waypoints[c].lon + "; " + this.waypoints[c].altitude
             + "; " + this.waypoints[c].maxDelta + "; " + (this.waypoints[c].landingAllowed ? "1" : "0");
        }

        fs.writeFile(filename, text, function (error) {
            if (error) {
                console.warn("Errror writing file!");
            }
        })
    },

    load: function (filename) {
        fs.readFile(filename, 'utf8', (err, data) => {
            if(err != null) {
                console.warn(err);
            } else {
                let lines = data.split("\n");

                // A little workaround to get all methods of the waypoint-objects
                planning.waypoints = [];
                for(let c=2; c<lines.length; c++) {
                    let items = lines[c].split(";");
                    planning.waypoints.push(
                        new planning.Waypoint(
                            Number(items[1]), Number(items[2]), Number(items[3]), Number(items[4]), Boolean(items[5])));
                }
                this.updateAllViews();
            }
        })
    },


    getLength: function () {
        let length = 0;
        for(let c=1; c<this.waypoints.length; c++) {
            length += this.waypoints[c-1].distanceTo(this.waypoints[c]);
        }
        return length;
    }
};


$(document).ready(function () {
    map.init();

    $("#buttonSave").click(function () {
        let filename = dialog.showSaveDialog({
            "title": "Save Mission",
            "defaultPath": "mission.csv"
        });
        if(filename != null) {
            planning.save(filename);
        }
    });


    $("#buttonOpen").click(function () {
        let filename = dialog.showOpenDialog({
            "title": "Load Mission"
        });
        if(filename != null) {
            planning.load(filename[0]);
        }
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
                planning.waypoints[row].altitude = Number(newVal);
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
        planning.removeWaypoint(row.index())
    })
});