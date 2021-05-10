'use strict'


// C library API
const ffi = require('ffi-napi');

// Express App (Routes)
const express = require("express");
const app     = express();
const path    = require("path");
const fileUpload = require('express-fileupload');

app.use(fileUpload());
app.use(express.static(path.join(__dirname+'/uploads')));

// Minimization
const fs = require('fs');
const JavaScriptObfuscator = require('javascript-obfuscator');

// Important, pass in port as in `npm run dev 1234`, do not change
const portNum = process.argv[2];

// Send HTML at root, do not change
app.get('/',function(req,res){
res.sendFile(path.join(__dirname+'/public/index.html'));
});

// Send Style, do not change
app.get('/style.css',function(req,res){
//Feel free to change the contents of style.css to prettify your Web app
res.sendFile(path.join(__dirname+'/public/style.css'));
});

// Send obfuscated JS, do not change
app.get('/index.js',function(req,res){
fs.readFile(path.join(__dirname+'/public/index.js'), 'utf8', function(err, contents) {
    const minimizedContents = JavaScriptObfuscator.obfuscate(contents, {compact: true, controlFlowFlattening: true});
    res.contentType('application/javascript');
    res.send(minimizedContents._obfuscatedCode);
});
});


//Respond to POST requests that upload files to uploads/ directory
app.post('/upload', function(req, res) {

    if(!req.files) {
        return res.redirect('/');
    }
    var exists = 0;
    let uploadFile = req.files.uploadFile;
    let files = fs.readdirSync('./uploads');
    for (var i = 0; i < files.length; i++) {
        if (files[i] == uploadFile.name) {
            exists = 1;
        }
    }
    if (!exists) {
        uploadFile.mv('uploads/' + uploadFile.name, function(err) {
            if(err) {
                return res.status(500).send(err);
            }
            res.redirect('/');
        });
    } else {
        return res.redirect('/');
    }
    
});

app.get('/validate-upload', function(req, res){
    let filenamee = req.query.filename;
    var exists = 0;
    let files = fs.readdirSync('./uploads');
    for (var i = 0; i < files.length; i++) {
        if (files[i] == filenamee) {
            exists = 1;
        }
    }
    if (!exists) {
        res.send('Uploaded ' + filenamee + '  \n -  it will show in the file log panel if it is valid');
    } else {
        res.send('File already exists cannot overwrite');
    }
});

//Respond to GET requests for files in the uploads/ directory
app.get('/uploads/:name', function(req , res){
fs.stat('uploads/' + req.params.name, function(err, stat) {
    if(err == null) {
    res.sendFile(path.join(__dirname+'/uploads/' + req.params.name));
    } else {
    console.log('Error in file downloading route: '+err);
    res.send('');
    }
});
});



let lib = ffi.Library(
    './libgpxparse', {
        'validateGPXtoJSON': ['string', ['string']],
        'makeOtherJsonRoute': ['string', ['string', 'string']],
        'makeOtherJsonTrack': ['string', ['string', 'string']],
        'waypointToComponentsJson': ['string', ['string']],
        'routeToComponentsJson': ['string', ['string']],
        'trackToComponentsJson': ['string', ['string']],
        'renameRoute': ['string', ['string', 'string', 'int']],
        'renameTrack': ['string', ['string', 'string', 'int']],
        'makeGPXjson': ['string', ['string', 'string', 'float']],
        'makeRouteJson': ['string', ['string', 'string', 'int', 'string', 'float', 'float']],
        'RoutesBetweentoJson': ['string', ['string', 'float', 'float', 'float', 'float', 'float']],
        'TracksBetweentoJson': ['string', ['string', 'float', 'float', 'float', 'float', 'float']],
        
});


app.get('/make-file-log', function(req, res){
    let files = fs.readdirSync('./uploads');
    var jsonFiles = [];
    var index = 0;
    for (var i = 0; i < files.length; i++) {
        let gpxJson = lib.validateGPXtoJSON("uploads/"+files[parseInt(i)]);
        if(gpxJson !== "Invalid") {
            let parsegpx = JSON.parse(gpxJson);
            parsegpx['file'] = files[parseInt(i)];
            jsonFiles[index] = JSON.stringify(parsegpx);
            index++;
        }
    }
    res.send(jsonFiles);
});

app.get('/make-other-alert', function(req , res) {
    res.send(lib.makeOtherJsonRoute(req.query.filename, req.query.routename));
});

app.get('/make-other-alert-track', function(req , res) {
    res.send(lib.makeOtherJsonTrack(req.query.filename, req.query.routename));
});

app.get('/make-waypoint-veiw', function(req , res){
    res.send(lib.waypointToComponentsJson(req.query.filename));
});

app.get('/make-route-veiw', function(req , res){
    res.send(lib.routeToComponentsJson(req.query.filename));
});

app.get('/make-track-veiw', function(req , res){
    res.send(lib.trackToComponentsJson(req.query.filename));
});

app.get('/rename-route', function(req , res) {
    res.send(lib.renameRoute(req.query.filename, req.query.routename, req.query.idx));
});

app.get('/rename-track', function(req , res){
    res.send(lib.renameTrack(req.query.filename, req.query.routename, req.query.idx));
});

app.get('/make-gpx-file', function(req , res) {

    var exists = 0;
    let files = fs.readdirSync('./uploads');
    for (var i = 0; i < files.length; i++) {
        let gpxJson = lib.validateGPXtoJSON("uploads/"+files[parseInt(i)]);
        if ('uploads/'+files[parseInt(i)] == req.query.filename && gpxJson != 'Invalid') {
            exists = 1;
        }
    }
    if (!exists) {
        let funcResp = lib.makeGPXjson(req.query.filename, req.query.creator, 1.1);
        res.send("Uploaded new GPX file in" + funcResp);
    } else {
        res.send('File already exists cannot overwrite');
    }
});

app.get('/make-route-file', function(req , res) {
    res.send(lib.makeRouteJson(req.query.filename, req.query.routename, req.query.isWaypoint, req.query.waypointname, req.query.lon, req.query.lat));
});

app.get('/find-route-between', function(req , res) {
    var valid = true;
    let uploadsFolder = fs.readdirSync('./uploads');
    var pathsFound = [];
    for (var file in uploadsFolder) {
        if (valid) {
            let parsePath = JSON.parse(lib.RoutesBetweentoJson('uploads/' + uploadsFolder[parseInt(file)], req.query.startLon, req.query.startLat, req.query.endLon, req.query.endLat, req.query.delta));
            parsePath['file'] = uploadsFolder[parseInt(file)];
            pathsFound[parseInt(file)] = JSON.stringify(parsePath);
        }
    }
    res.send(pathsFound);
});


app.get('/find-track-between', function(req , res) {
    var valid = true;
    let uploadsFolder = fs.readdirSync('./uploads');
    var pathsFound = [];
    for (var file in uploadsFolder) {
        if (valid) {
            let parsePath = JSON.parse(lib.TracksBetweentoJson('uploads/' + uploadsFolder[parseInt(file)], req.query.startLon, req.query.startLat, req.query.endLon, req.query.endLat, req.query.delta));
            parsePath['file'] = uploadsFolder[parseInt(file)];
            pathsFound[parseInt(file)] = JSON.stringify(parsePath);
        }
    }
    res.send(pathsFound);
});

const mysql = require('mysql2/promise');
let connection;

app.get('/dblogin', async function(req, res) {
    var status;
    try {  	
        connection = await mysql.createConnection({
            host : 'dursley.socs.uoguelph.ca',
            user : req.query.username,
            password : req.query.password,
            database : req.query.dbname,
        });
        await connection.execute(
            `CREATE TABLE IF NOT EXISTS FILE (
            gpx_id INT AUTO_INCREMENT PRIMARY KEY, 
            file_name VARCHAR(60) NOT NULL, 
            ver DECIMAL(2,1) NOT NULL, 
            creator VARCHAR(256) NOT NULL)`);
        await connection.execute(
            `CREATE TABLE IF NOT EXISTS ROUTE (
            route_id INT AUTO_INCREMENT PRIMARY KEY, 
            route_name VARCHAR(256), 
            route_len FLOAT(15,7) NOT NULL, 
            gpx_id INT NOT NULL,
            FOREIGN KEY(gpx_id) REFERENCES FILE(gpx_id) ON DELETE CASCADE)`);
        await connection.execute(
            `CREATE TABLE IF NOT EXISTS POINT (
            point_id INT AUTO_INCREMENT PRIMARY KEY, 
            point_index INT NOT NULL, 
            latitude DECIMAL(11,7) NOT NULL, 
            longitude DECIMAL(11,7) NOT NULL, 
            point_name VARCHAR(256), 
            route_id INT NOT NULL,
            FOREIGN KEY(route_id) REFERENCES ROUTE(route_id) ON DELETE CASCADE)`);
        status = "Logged In";
    } catch(e) {
        status = "Login Failed";
    }
    if (connection && connection.end ) {
        connection.end();  
    }
    
    res.send(status);
});

app.get('/dbstorefile', async function(req, res){
    var status;

    try {  	
        connection = await mysql.createConnection({
            host : 'dursley.socs.uoguelph.ca',
            user : req.query.username,
            password : req.query.password,
            database : req.query.dbname,
        });

        await connection.execute('delete from FILE;');
        await connection.execute('delete from ROUTE;');
        await connection.execute('delete from POINT;');

        let uploadsFolder = fs.readdirSync('./uploads');
        for(var i = 0; i < uploadsFolder.length; i++) {
            let gpxJSON = lib.validateGPXtoJSON("uploads/"+uploadsFolder[i]);
            
            if(gpxJSON != "Invalid") {		
                let parsedGPX = JSON.parse(gpxJSON);
                parsedGPX["filename"] = uploadsFolder[i];
                await connection.execute('INSERT INTO FILE (file_name, ver, creator) VALUES (\'' + parsedGPX["filename"] + '\',' + parsedGPX["version"] + ',\'' + parsedGPX["creator"] + '\');');
                
                let parsedRoute = JSON.parse(lib.routeToComponentsJson("uploads/" + uploadsFolder[i]));
                for(var j = 0; j < parsedRoute.length; j++) {
                    let [rows, fields] = await connection.execute('SELECT gpx_id FROM FILE WHERE file_name = \'' + uploadsFolder[i] + '\';');
                    await connection.execute('INSERT INTO ROUTE (route_name, route_len, gpx_id) VALUES (\'' + parsedRoute[j]["name"] + '\',' + parsedRoute[j]["len"] + ', \'' + rows[0]["gpx_id"] + '\');');
                }
                var addCheck = true;
                let parsedPoint = JSON.parse(lib.waypointToComponentsJson("uploads/" + uploadsFolder[i]));                
                for(var j = 0; j < parsedPoint.length; j++) {
                    let [rows, fields] = await connection.execute('SELECT route_id FROM ROUTE WHERE route_name = \'' + parsedPoint[j]["routename"] + '\';');
                    if (addCheck) {
                        for(var k = 0; k < parsedPoint[j]["waypoint"].length; k++) {
                            let pointInfo = parsedPoint[j]["waypoint"][k];
                            await connection.execute('INSERT INTO POINT (point_index, latitude, longitude, point_name, route_id) VALUES (' + pointInfo["waypointindex"] + ',' + pointInfo["lat"] + ', ' + pointInfo["lon"] + ',\'' + pointInfo["name"] + '\',\'' + rows[0]["route_id"] + '\');');
                        }
                    }
                }
            }
        }
        status = "All Files Stored";
    } catch(e) {  
        status = "Files Not Stored";
    }
    if (connection && connection.end ) {
        connection.end();  
    }
    res.send(status);
});

app.get('/dbstatus', async function(req, res){
    var values = [];
    try {  	
        connection = await mysql.createConnection({
            host : 'dursley.socs.uoguelph.ca',
            user : req.query.username,
            password : req.query.password,
            database : req.query.dbname,
        });

        let [rows, fields] = await connection.execute('SELECT COUNT(*) FROM FILE');
        values[0] = rows[0]["COUNT(*)"];
        [rows, fields] = await connection.execute('SELECT COUNT(*) FROM ROUTE');
        values[1] = rows[0]["COUNT(*)"];
        [rows, fields] = await connection.execute('SELECT COUNT(*) FROM POINT');
        values[2] = rows[0]["COUNT(*)"];

    } catch(e) {  
        values = [0,0,0];
    }
    if (connection && connection.end ) {
        connection.end();  
    }
    res.send(values);
});

app.get('/dbremove', async function(req, res){
    var status;
    try {  	
        connection = await mysql.createConnection({
            host : 'dursley.socs.uoguelph.ca',
            user : req.query.username,
            password : req.query.password,
            database : req.query.dbname,
        });
        await connection.execute('delete from FILE;');
        await connection.execute('delete from ROUTE;');
        await connection.execute('delete from POINT;');
        status = "All entries removed from the database";
    } catch(e) {  
        status = "Entries not removed from the database";
    }
    if (connection && connection.end ) {
        connection.end();  
    }
    res.send(status);
});

app.get('/queryOne', async function(req, res){
    var values = [];
    try {  	
        connection = await mysql.createConnection({
            host : 'dursley.socs.uoguelph.ca',
            user : req.query.username,
            password : req.query.password,
            database : req.query.dbname,
        });

        let [rows, fields] = await connection.execute('SELECT * FROM ROUTE ORDER BY '+ req.query.sortoption +';');
        for (var i = 0; i < rows.length; i++) {
            values[i] = {"route_id": rows[i]["route_id"], "route_name": rows[i]["route_name"], "route_len": rows[i]["route_len"], "gpx_id": rows[i]["gpx_id"]};
        }

    } catch(e) {  
        console.log(e);
    }
    if (connection && connection.end ) {
        connection.end();  
    }
    res.send(values);
});

app.get('/dbfiledrop', async function(req, res){
    var values = [];

    try {  	
        connection = await mysql.createConnection({
            host : 'dursley.socs.uoguelph.ca',
            user : req.query.username,
            password : req.query.password,
            database : req.query.dbname,
        });

        let [rows, fields] = await connection.execute('SELECT * FROM FILE;');
        for (var i = 0; i < rows.length; i++) {
            values[i] = {"gpx_id": rows[i]["gpx_id"], "file_name": rows[i]["file_name"]};
        }

    } catch(e) {  
        console.log(e);
    }
    if (connection && connection.end ) {
        connection.end();  
    }
    res.send(values);
});


app.get('/dbroutedrop', async function(req, res){
    var values = [];
    try {  	
        connection = await mysql.createConnection({
            host : 'dursley.socs.uoguelph.ca',
            user : req.query.username,
            password : req.query.password,
            database : req.query.dbname,
        });

        let [rows, fields] = await connection.execute('SELECT * FROM ROUTE;');
        for (var i = 0; i < rows.length; i++) {
            values[i] = {"route_id": rows[i]["route_id"], "route_name": rows[i]["route_name"]};
        }

    } catch(e) {  
        console.log(e);
    }
    if (connection && connection.end ) {
        connection.end();  
    }
    res.send(values);
});


app.get('/queryTwo', async function(req, res){
    var values = [];

    try {  	
        connection = await mysql.createConnection({
            host : 'dursley.socs.uoguelph.ca',
            user : req.query.username,
            password : req.query.password,
            database : req.query.dbname,
        });

        let [rows, fields] = await connection.execute('SELECT * FROM ROUTE WHERE gpx_id = ' + req.query.gpxid + ' ORDER BY '+ req.query.sortoption +';');
        for (var i = 0; i < rows.length; i++) {
            values[i] = {"route_id": rows[i]["route_id"], "route_name": rows[i]["route_name"], "route_len": rows[i]["route_len"], "gpx_id": rows[i]["gpx_id"]};
        }

    } catch(e) {  
        console.log(e);
    }
    if (connection && connection.end ) {
        connection.end();  
    }
    res.send(values);
});



app.get('/queryThree', async function(req, res){
    var values = [];

    try {  	
        connection = await mysql.createConnection({
            host : 'dursley.socs.uoguelph.ca',
            user : req.query.username,
            password : req.query.password,
            database : req.query.dbname,
        });

        let [rows, fields] = await connection.execute('SELECT * FROM POINT WHERE route_id = ' + req.query.routeid + ' ORDER BY point_index;');
        for (var i = 0; i < rows.length; i++) {
            values[i] = {"point_index": rows[i]["point_index"], "point_name": rows[i]["point_name"], "latitude": rows[i]["latitude"], "longitude": rows[i]["longitude"]};
        }

    } catch(e) {  
        console.log(e);
    }
    if (connection && connection.end ) {
        connection.end();  
    }
    res.send(values);
});



app.get('/queryFive', async function(req, res){
    var values = [];

    try {  	
        connection = await mysql.createConnection({
            host : 'dursley.socs.uoguelph.ca',
            user : req.query.username,
            password : req.query.password,
            database : req.query.dbname,
        });
        
        let [rows, fields] = await connection.execute('SELECT t.* FROM (SELECT * FROM ROUTE WHERE gpx_id = (SELECT gpx_id FROM FILE WHERE file_name = \'' + req.query.filename +'\') ORDER BY route_len ' + req.query.type + ' LIMIT ' + parseInt(req.query.nValue) + ') t ORDER BY t.'+ req.query.sortoption +';');
        for (var i = 0; i < rows.length; i++) {
            values[i] = {"route_name": rows[i]["route_name"], "route_len": rows[i]["route_len"], "file_name": req.query.filename, "route_id": rows[i]["route_id"]};
        }

    } catch(e) {  
        console.log(e);
    }
    if (connection && connection.end ) {
        connection.end();  
    }
    res.send(values);
});


app.listen(portNum);
console.log('Running app at localhost: ' + portNum);
