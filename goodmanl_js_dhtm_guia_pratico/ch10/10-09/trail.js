// Navigation Trail Library (trail.js)
// by Danny Goodman (http://dannyg.com)
// From "JavaScript & DHTML Cookbook", Second Edition (O'Reilly) by Danny Goodman
// Copyright 2007 Danny Goodman.  All Rights Reserved.


var trailMenu = new Object();
trailMenu["catalog"] = "Product Line";
trailMenu["economy"] = "Budget";
trailMenu["deluxe"] = "Luxury";
trailMenu["export"] = "Export Only";
trailMenu["support"] = "Product Support";
trailMenu["faq"] = "Frequently Asked Questions";
trailMenu["downloads"] = "Free Downloads";
trailMenu["manuals"] = "Manuals";

function makeTrailMenu() {
    var parseStart, volDelim, parseEnd;
    var output = "<span style='font-family:Arial, Helvetica, sans-serif; font-size:12px; color:#000000; padding:4px'>";
    var linkStyle = "color:#339966";
    var path = location.pathname;
    var separator = "&nbsp;&raquo;&nbsp;";
    var re = /\\/g;
    path = path.replace(re, "/");
    var trail = location.protocol + "//" + location.hostname;
    var leaves = path.split("/");
    if (location.protocol.indexOf("file") != -1) {
        parseStart = 1;
        volDelim = "/";
    } else {
        parseStart = 0;
        volDelim = "";
    }
    if (leaves[leaves.length-1] == "" || leaves[leaves.length-1] == "index.html" || leaves[leaves.length-1] == "default.html") {
        parseEnd = leaves.length -1;
    } else {
        parseEnd = leaves.length;
    }
    for (var i = parseStart; i < parseEnd; i++) {
        if (i == parseStart) {
            trail += "/" + leaves[i] + volDelim;
            output += "<a href='" + trail + "' style='" + linkStyle + "'>";
            output += "Home";
        } else if (i == parseEnd - 1) {
            output += document.title;
            separator = "";
        } else {
            trail += leaves[i] + "/";
            output += "<a href='" + trail + "' style='" + linkStyle + "'>";
            output += trailMenu[leaves[i]];
        }
        output += "</a>" + separator;
    }
    output += "</span>";
    return output;
}
