
/*
A4
Amardeep Singh
1079318
*/


jQuery(document).ready(function() {
    jQuery('#db-stuff').hide();

    var loggedIn = false;

    jQuery('#upload-btn').click(function(e) {
        var files = jQuery('#first-upload').prop('files')[0];
        if (!files) {
            alert("No file chosen");
        } else {
            let filename = files.name;
            jQuery.ajax({
                url: '/validate-upload', 
                type: 'get',
                data: {
                    filename: filename
                },
                success: function (data) {
                    alert(data);
                    console.log("File uploaded");
                },
                fail: function (error) {
                    alert("file upload failed");
                    console.log(error); 
                }
            });
        }

        if (loggedIn) { 
            let username = jQuery("#db-uname").val();
            let password = jQuery("#db-pswd").val();
            let dbname = jQuery("#db-dbname").val();

            jQuery.ajax({
                type: 'get',
                url: '/dbstorefile',
                data: {
                    username: username,
                    password: password,
                    dbname: dbname,
                },
                success: function (data1) {
                    console.log(data1);
                },
                fail: function (error) {
                    console.log(error);
                }
            });
        }
    });

    
    jQuery.ajax({
        url: '/make-file-log',
        type: 'get',
        dataType: 'json',
        success: function (data) {
            if (data.length == 0) {
                jQuery('#no-file-log').show();
            } else {
                jQuery('#no-file-log').hide();
                for(var i = 0; i < data.length; i++) {
                    let gpxData = JSON.parse(data[i]);
                    jQuery('#file-log-table').append('<tr><th scope="row"><a href="uploads/'+ gpxData["file"] + '">' + gpxData["file"] + '</a></th><th>' + gpxData["version"] + '</th><th>' + gpxData["creator"] + '</th><th>' + gpxData["numWaypoints"] + '</th><th>' + gpxData["numRoutes"] + '</th><th>' + gpxData["numTracks"] + '</th>');
                    if (gpxData["numRoutes"] == 0 && gpxData["numTracks"] == 0) {
                        jQuery('#fileDropOne').append('<option disabled value="' + gpxData["file"] + '">' + gpxData["file"] + '</option>');
                    } else {
                        jQuery('#fileDropOne').append('<option value="' + gpxData["file"] + '">' + gpxData["file"] + '</option>');
                    }
                    jQuery('#fileDropTwo').append('<option value="' + gpxData["file"] + '">' + gpxData["file"] + '</option>');
                }
                console.log("Files loaded");
            }
            
        },
        fail: function (error) {
            alert('file load failed');
            console.log(error); 
        }
    });



    jQuery('#fileDropOne').change(function() {
        let gpxfile = 'uploads/' + jQuery('#fileDropOne option:selected').text();
        jQuery('.rterows').remove();

        jQuery.ajax({
            url: '/make-route-veiw',
            type: 'get',     
            dataType: 'json',
            data: {
                filename: gpxfile
            },
            success: function (data) {
                for(var i = 0; i < data.length; i++) {
                    jQuery('#gpx-view-table').append('<tr class="rterows"><th>Route ' + (i+1) + '</th><th>' + data[i]["name"] + '</th><th>' + data[i]["numPoints"] + '</th><th>' + data[i]["len"] + '</th><th>' + data[i]["loop"] + '</th><th><input type="button" id="rteOther_'+ i + '" value="View" class="btn btn-secondary rteOtherBtn"></th><th><input type="button" id="rteRename_'+ i + '" value="Rename" class="btn btn-success rteRenameBtn"></th></tr>');
                }
                var otherViewBtn = function(i) {
                    jQuery.ajax({
                        url: '/make-other-alert',
                        type: 'get',           
                        dataType: 'json',   
                        data: {
                            filename: gpxfile,
                            routename: data[i]["name"]
                        },
                        success: function (otherData) {
                            var output = '';
                            if (otherData.length == 0) {
                                output += 'No other data';
                            } else {
                                for (var j = 0; j < otherData.length; j++) {
                                    output += 'name : ' + otherData[j]["name"] + ', value : ' + otherData[j]["value"] + '\n';
                                }
                            }
                            alert(output); 
                            console.log("Found other data");
                        },
                        fail: function (error) {
                            alert('Other data load failed');
                            console.log(error); 
                        }
                    });
                }
                jQuery('.rteOtherBtn').click(function() {
                    otherViewBtn(this.id.replace('rteOther_', ''));
                });

                var otherRenameBtn = function(i) {
                    let renameValue = jQuery('#rename-text').val();
                    if (renameValue == "") {
                        renameValue = " ";
                    }

                    jQuery.ajax({
                        url: '/rename-route',
                        type: 'get',
                        data: {
                            filename: gpxfile,
                            routename: renameValue,
                            idx: parseInt(i),
                        },
                        success: function (renameData) {
                            alert("Renamed Route " + (parseInt(i)+1) + " to " + renameValue + renameData);
                            if (loggedIn) { 
                                let username = jQuery("#db-uname").val();
                                let password = jQuery("#db-pswd").val();
                                let dbname = jQuery("#db-dbname").val();
            
                                jQuery.ajax({
                                    type: 'get',
                                    url: '/dbstorefile',
                                    data: {
                                        username: username,
                                        password: password,
                                        dbname: dbname,
                                    },
                                    success: function (data1) {
                                        console.log(data1);
                                    },
                                    fail: function (error) {
                                        console.log(error);
                                    }
                                });
                                alert("Database Updated");
                            }

                            location.reload();
                        },
                        fail: function (error) {
                            alert('Rename failed');
                            console.log(error); 
                        }
                    });
                }
                jQuery('.rteRenameBtn').click(function() {
                    otherRenameBtn(this.id.replace('rteRename_', ''));
                    
                });
            },
            fail: function (error) {
                alert('Components failed for routes');
                console.log(error); 
            }
        });

        jQuery.ajax({
            url: '/make-track-veiw',
            type: 'get',     
            dataType: 'json',
            data: {
                filename: gpxfile
            },

            success: function (data) {
                for(var i = 0; i < data.length; i++) {
                    jQuery('#gpx-view-table').append('<tr class="rterows"><th>Track ' + (i+1) + '</th><th>' + data[i]["name"] + '</th><th> </th><th>' + data[i]["len"] + '</th><th>' + data[i]["loop"] + '</th><th><input type="button" id="trkOther_'+ i + '" value="View" class="btn btn-secondary trkOtherBtn"></th><th><input type="button" id="trkRename_'+ i + '" value="Rename" class="btn btn-success trkRenameBtn"></th></tr>');
                }
                var otherViewBtntrk = function(i) {
                    jQuery.ajax({
                        url: '/make-other-alert-track',
                        type: 'get',           
                        dataType: 'json',   
                        data: {
                            filename: gpxfile,
                            routename: data[i]["name"]
                        },
                        success: function (otherData) {
                            var output = '';
                            if (otherData.length == 0) {
                                output += 'No other data';
                            } else {
                                for (var j = 0; j < otherData.length; j++) {
                                    output += 'name : ' + otherData[j]["name"] + ', value : ' + otherData[j]["value"] + '\n';
                                }
                            }
                            alert(output); 
                            console.log("Found other data");
                        },
                        fail: function (error) {
                            alert('Other data load failed');
                            console.log(error); 
                        }
                    });
                }
                jQuery('.trkOtherBtn').click(function() {
                    otherViewBtntrk(this.id.replace('trkOther_', ''));
                });

                var otherRenameBtntrk = function(i) {
                    let renameValue = jQuery('#rename-text').val();
                    if (renameValue == "") {
                        renameValue = " ";
                    }

                    jQuery.ajax({
                        url: '/rename-track',
                        type: 'get',
                        data: {
                            filename: gpxfile,
                            routename: renameValue,
                            idx: parseInt(i),
                        },
                        success: function (renameData) {
                            alert("Renamed Track " + (parseInt(i)+1) + " to " + renameValue + renameData);
                            location.reload();
                        },
                        fail: function (error) {
                            alert('Rename failed');
                            console.log(error); 
                        }
                    });
                }
                jQuery('.trkRenameBtn').click(function() {
                    otherRenameBtntrk(this.id.replace('trkRename_', ''));
                });
            },
            fail: function (error) {
                alert('Components failed for tracks');
                console.log(error); 
            }
        });
    });


    jQuery('#gpx-form').submit(function(e){
        e.preventDefault();

        jQuery.ajax({
            url: '/make-gpx-file',
            type: 'get',
            data: {
                filename: 'uploads/' + jQuery("#createFileName").val() + '.gpx',
                creator: jQuery("#createCreator").val(),
                version: parseFloat(jQuery("#createVersion").val())
            },
            success: function (data) {
                alert(data);
                if (loggedIn) { 
                    let username = jQuery("#db-uname").val();
                    let password = jQuery("#db-pswd").val();
                    let dbname = jQuery("#db-dbname").val();

                    jQuery.ajax({
                        type: 'get',
                        url: '/dbstorefile',
                        data: {
                            username: username,
                            password: password,
                            dbname: dbname,
                        },
                        success: function (data1) {
                            console.log(data1);
                        },
                        fail: function (error) {
                            console.log(error);
                        }
                    });
                    alert("Database Updated");
                }

                location.reload();
            },
            fail: function (error) {
                alert('Creating GPX failed');
                console.log(error);
            }
        });
        return false;
    });


    jQuery('#addwptbox').change(function(){
        if (jQuery('#addwptbox').is(":checked")){
            jQuery("#addWptName").prop( "disabled", false );
            jQuery("#addLon").prop( "disabled", false );
            jQuery("#addLat").prop( "disabled", false );
            jQuery("#addLon").prop( "required", true );
            jQuery("#addLat").prop( "required", true );
        } else {
            jQuery("#addWptName").val('');
            jQuery("#addWptName").prop( "disabled", true );
            jQuery("#addLon").prop( "disabled", true );
            jQuery("#addLat").prop( "disabled", true );
            jQuery("#addLon").prop( "required", false );
            jQuery("#addLat").prop( "required", false );
        }
    });

    jQuery('#route-form').submit(function(e) {
        e.preventDefault();

        var isWaypoint = 0;
        if (jQuery('#addwptbox').is(':checked')) {
            isWaypoint = 1;
        }

        jQuery.ajax({
            url: '/make-route-file',
            type: 'get',
            data: {
                filename: 'uploads/' + jQuery('#fileDropTwo option:selected').text(),
                routename: jQuery('#addRouteName').val(),
                isWaypoint: parseInt(isWaypoint),
                waypointname: jQuery('#addWptName').val(),
                lon: parseFloat(jQuery('#addLon').val()),
                lat: parseFloat(jQuery('#addLat').val())
            },
            success: function(data) {				
                alert("Added route to " + jQuery('#fileDropTwo option:selected').text() + data);
                if (loggedIn) { 
                    let username = jQuery("#db-uname").val();
                    let password = jQuery("#db-pswd").val();
                    let dbname = jQuery("#db-dbname").val();

                    jQuery.ajax({
                        type: 'get',
                        url: '/dbstorefile',
                        data: {
                            username: username,
                            password: password,
                            dbname: dbname,
                        },
                        success: function (data1) {
                            console.log(data1);
                        },
                        fail: function (error) {
                            console.log(error);
                        }
                    });
                    alert("Database Updated");
                }
                location.reload();
            },
            fail: function (error) {
                alert('Adding route failed');
                console.log(error); 
            }
        });

        return false;
    });


    jQuery('#path-form').submit(function (e){
        e.preventDefault();

        jQuery('.pathrows').remove();

        let	startLon = parseFloat(jQuery('#startLon').val());
        let	startLat = parseFloat(jQuery('#startLat').val());
        let	endLon = parseFloat(jQuery('#endLon').val());
        let	endLat = parseFloat(jQuery('#endLat').val());
        let	delta = parseFloat(jQuery('#deltaa').val());

        this.reset();

        jQuery.ajax({
            url: '/find-route-between',
            type: 'get',
            dataType: 'json',
            data: {
                startLon: startLon,
                startLat: startLat,
                endLon: endLon,
                endLat: endLat,
                delta: delta,
            },
            success: function (data) {
                var total = 0;
                for (var datai in data) {
                    let componentData = JSON.parse(data[datai]);
                    for (var dataj in componentData) {
                        total++;                       
                        jQuery("#find-path-table").append('<tr class="pathrows"><th>Route ' + (parseInt(dataj)+1) + '</th><th>' + componentData[dataj]["name"] + '</th><th>' + componentData[dataj]["numPoints"] + '</th><th>' + componentData[dataj]["len"] + '</th><th>' + componentData[dataj]["loop"] + '</th>');
                    }
                }
                alert('Found ' + total + ' Route(s) Between');
                console.log("finished finding paths");
            },
            fail: function (error) {
                alert('Finding paths between failed');
                console.log(error); 
            }
        });


        jQuery.ajax({
            url: '/find-track-between',
            type: 'get',
            dataType: 'json',
            data: {
                startLon: startLon,
                startLat: startLat,
                endLon: endLon,
                endLat: endLat,
                delta: delta,
            },
            success: function (data) {
                var total = 0;
                for (var datai in data) {
                    let componentData = JSON.parse(data[datai]);
                    for (var dataj in componentData) {
                        total++;                       
                        jQuery("#find-path-table").append('<tr class="pathrows"><th>Track ' + (parseInt(dataj)+1) + '</th><th>' + componentData[dataj]["name"] + '</th><th> </th><th>' + componentData[dataj]["len"] + '</th><th>' + componentData[dataj]["loop"] + '</th>');
                    }
                }
                alert('Found ' + total + ' Track(s) Between');
                console.log("finished finding paths");
            },
            fail: function (error) {
                alert('Finding paths between failed');
                console.log(error); 
            }
        });

        return false;
    });

    jQuery('#db-form').submit(function(e) {
        e.preventDefault();

        let username = jQuery("#db-uname").val();
        let password = jQuery("#db-pswd").val();
        let dbname = jQuery("#db-dbname").val();
        
        jQuery.ajax({
            url: '/dblogin',
            type: 'get',
            data: {
                username: username,
                password: password,
                dbname: dbname,
            },
            success: function (data) {
                if (data == "Login Failed") {
                    alert('Password or Username Invalid' + ' ' + data);
                    jQuery('#db-stuff').hide();
                    location.reload();
                } else {
                    alert(username + ' ' + data);
                    loggedIn = true;
                    jQuery('#db-stuff').show();
                }
            },
            fail: function (error) {
                alert('Logging into database failed');
                console.log(error); 
            }
        });

        jQuery('#db-storefiles').click(function(e){
            e.preventDefault();
            jQuery.ajax({
                url: '/dbstorefile',
                type: 'get',
                data: {
                    username: username,
                    password: password,
                    dbname: dbname,
                },
                success: function (data) {
                    alert(data);
                },
                fail: function (error) {
                    alert('Storing files in database failed');
                    console.log(error); 
                }
            });
            jQuery('#db-table1-select').prop('selectedIndex', 0);
            jQuery("#db-check2").prop("checked", false);
            jQuery("#db-check3").prop("checked", false);
            jQuery("#db-check4").prop("checked", false);
            jQuery("#db-check5").prop("checked", false);
        });


        jQuery('#db-status').click(function(e){
            e.preventDefault();
            
            jQuery.ajax({
                url: '/dbstatus',
                type: 'get',
                dataType: 'json',
                data: {
                    username: username,
                    password: password,
                    dbname: dbname
                },
                success: function (data) {
                    alert("Database has " + data[0] + " files, " + data[1] + " routes, and " + data[2] + " points");
                },
                fail: function (error) {
                    alert('Status for database failed');
                    console.log(error); 
                }
            });
        });


        jQuery('#db-removefiles').click(function(e){
            e.preventDefault();
            jQuery.ajax({
                url: '/dbremove',
                type: 'get',
                data: {
                    username: username,
                    password: password,
                    dbname: dbname
                },
                success: function (data) {
                    alert(data);
                },
                fail: function (error) {
                    alert('Remove function for database failed');
                    console.log(error); 
                }
            });
            jQuery('#db-table1-select').prop('selectedIndex', 0);
            jQuery("#db-check2").prop("checked", false);
            jQuery("#db-check3").prop("checked", false);
            jQuery("#db-check4").prop("checked", false);
            jQuery("#db-check5").prop("checked", false);
        });


        jQuery('#db-update').click(function(e){
            e.preventDefault();
            jQuery.ajax({
                url: '/dbstorefile',
                type: 'get',
                data: {
                    username: username,
                    password: password,
                    dbname: dbname,
                },
                success: function (data) {
                    alert("Database Updated and " + data);
                },
                fail: function (error) {
                    alert('Update function for database failed');
                    console.log(error); 
                }
            });
            jQuery('#db-table1-select').prop('selectedIndex', 0);
            jQuery("#db-check2").prop("checked", false);
            jQuery("#db-check3").prop("checked", false);
            jQuery("#db-check4").prop("checked", false);
            jQuery("#db-check5").prop("checked", false);
        });

        jQuery('#db-table1-select').prop('selectedIndex', 0);
        jQuery('.db1').remove();
        jQuery('#db-table1-select').change(function() {
            jQuery('.db1').remove();
            if (jQuery('#db-table1-select option:selected').val() != "sort") {
                jQuery.ajax({
                    url: '/queryOne',
                    type: 'get',
                    dataType: 'json',
                    data: {
                        username: username,
                        password: password,
                        dbname: dbname,
                        sortoption: jQuery('#db-table1-select option:selected').val()
                    },
                    success: function (data) {
                        for(var i = 0; i < data.length; i++) {
                            jQuery('#db-table1').append('<tr class="db1"><th>' + data[i]["route_id"] + '</th><th>' + data[i]["route_name"] + '</th><th>' + data[i]["route_len"] + '</th><th>' + data[i]["gpx_id"] + '</th></tr>');
                        }
                    },
                    fail: function (error) {
                        alert('Display 1 failed');
                        console.log(error); 
                    }
                });
            }
        });

        jQuery("#db-check2").prop("checked", false);
        jQuery('#db-check2').change( function () {
            if (jQuery('#db-check2').is(":checked")) {
                jQuery('#db-div2').show();
                jQuery.ajax({
                    url: '/dbfiledrop',
                    type: 'get',
                    dataType: 'json',
                    data: {
                        username: username,
                        password: password,
                        dbname: dbname
                    },
                    success: function(data) {
                        jQuery('#file-table2-select').empty();
                        for(var i = 0; i < data.length; i++) {
                            jQuery('#file-table2-select').append('<option value="' + data[i]["gpx_id"] + '">' + data[i]["file_name"] + '</option>');
                        }
                    },
                    fail: function (error) {
                        alert('Display failed');
                        console.log(error); 
                    }
                });
            } else {
                jQuery('#db-div2').hide();
            }
        });

        jQuery('#file-table2-select').change(function() {
            jQuery('#db-table2-select').prop('selectedIndex', 0);
            jQuery('.db2').remove();
        });
        jQuery('#db-table2-select').prop('selectedIndex', 0);
        jQuery('.db2').remove();
        jQuery('#db-table2-select').change(function() {
            jQuery('.db2').remove();
            if (jQuery('#db-table2-select option:selected').val() != "sort") {
                jQuery.ajax({
                    url: '/queryTwo',
                    type: 'get',
                    dataType: 'json',
                    data: {
                        username: username,
                        password: password,
                        dbname: dbname,
                        sortoption: jQuery('#db-table2-select option:selected').val(),
                        gpxid: parseInt(jQuery('#file-table2-select option:selected').val())
                    },
                    success: function (data) {
                        for(var i = 0; i < data.length; i++) {
                            jQuery('#db-table2').append('<tr class="db2"><th>' + data[i]["route_id"] + '</th><th>' + data[i]["route_name"] + '</th><th>' + data[i]["route_len"] + '</th><th>' + data[i]["gpx_id"] + '</th></tr>');
                        }
                    },
                    fail: function (error) {
                        alert('Display failed');
                        console.log(error); 
                    }
                });
            }
        });

        jQuery("#db-check3").prop("checked", false);
        jQuery('#db-check3').change( function () {
            if (jQuery('#db-check3').is(":checked")) {
                jQuery('#db-div3').show();
                jQuery.ajax({
                    url: '/dbroutedrop',
                    type: 'get',
                    dataType: 'json',
                    data: {
                        username: username,
                        password: password,
                        dbname: dbname
                    },
                    success: function(data) {
                        jQuery('#file-table3-select').empty().append('<option disabled selected>Select Route</option>');;
                        for(var i = 0; i < data.length; i++) {
                            jQuery('#file-table3-select').append('<option value="' + data[i]["route_id"] + '">' + data[i]["route_name"] + '</option>');
                        }
                    },
                    fail: function (error) {
                        alert('Display failed');
                        console.log(error); 
                    }
                });
            } else {
                jQuery('#db-div3').hide();
            }
        });

        jQuery('#file-table3-select').change(function() {
            jQuery('.db3').remove();
            jQuery.ajax({
                url: '/queryThree',
                type: 'get',
                dataType: 'json',
                data: {
                    username: username,
                    password: password,
                    dbname: dbname,
                    routeid: parseInt(jQuery('#file-table3-select option:selected').val())
                },
                success: function(data) {
                    for(var i = 0; i < data.length; i++) {
                        jQuery('#db-table3').append('<tr class="db3"><th>' + data[i]["point_index"] + '</th><th>' + data[i]["point_name"] + '</th><th>' + data[i]["latitude"] + '</th><th>' + data[i]["longitude"] + '</th></tr>');
                    }
                },
                fail: function (error) {
                    alert('Display failed');
                    console.log(error); 
                }
            });
        });

        jQuery("#db-check4").prop("checked", false);
        jQuery('#db-check4').change( function () {
            if (jQuery('#db-check4').is(":checked")) {
                jQuery('#db-div4').show();
                jQuery.ajax({
                    url: '/dbroutedrop',
                    type: 'get',
                    dataType: 'json',
                    data: {
                        username: username,
                        password: password,
                        dbname: dbname
                    },
                    success: function(data) {
                        jQuery('#file-table4-select').empty().append('<option disabled selected>Select File</option>');
                        for(var i = 0; i < data.length; i++) {
                            jQuery('#file-table4-select').append('<option value="' + data[i]["route_id"] + '">' + data[i]["route_name"] + '</option>');
                        }
                    },
                    fail: function (error) {
                        alert('Display failed');
                        console.log(error); 
                    }
                });
            } else {
                jQuery('#db-div4').hide();
            }
        });

        jQuery('#file-table4-select').change(function() {
            jQuery('.db4').remove();
            jQuery.ajax({
                url: '/queryThree',
                type: 'get',
                dataType: 'json',
                data: {
                    username: username,
                    password: password,
                    dbname: dbname,
                    routeid: parseInt(jQuery('#file-table4-select option:selected').val())
                },
                success: function(data) {
                    for(var i = 0; i < data.length; i++) {
                        jQuery('#db-table4').append('<tr class="db4"><th>' + data[i]["point_index"] + '</th><th>' + data[i]["point_name"] + '</th><th>' + data[i]["latitude"] + '</th><th>' + data[i]["longitude"] + '</th></tr>');
                    }
                },
                fail: function (error) {
                    alert('Display failed');
                    console.log(error); 
                }
            });
        });

        jQuery("#db-check5").prop("checked", false);
        jQuery('#db-check5').change( function () {
            if (jQuery('#db-check5').is(":checked")) {
                jQuery('#db-div5').show();
                jQuery.ajax({
                    url: '/dbfiledrop',
                    type: 'get',
                    dataType: 'json',
                    data: {
                        username: username,
                        password: password,
                        dbname: dbname
                    },
                    success: function(data) {
                        jQuery('#file-table5-select').empty();
                        for(var i = 0; i < data.length; i++) {
                            jQuery('#file-table5-select').append('<option value="' + data[i]["gpx_id"] + '">' + data[i]["file_name"] + '</option>');
                        }
                    },
                    fail: function (error) {
                        alert('Display failed');
                        console.log(error); 
                    }
                });
            } else {
                jQuery('#db-div5').hide();
            }
        });

        jQuery('.db5').remove();
        jQuery('#db5-form').submit(function(e) {
            e.preventDefault();
            jQuery('.db5').remove();
            
            jQuery.ajax({
                url: '/queryFive',
                type: 'get',
                dataType: 'json',
                data: {
                    username: username,
                    password: password,
                    dbname: dbname,
                    sortoption: jQuery('#db-table5-select option:selected').val(),
                    filename: jQuery('#file-table5-select option:selected').text(),
                    type: jQuery('#op-table5-select option:selected').val(),
                    nValue: parseFloat(jQuery("#n-value").val())
                },
                success: function(data) {
                    for (var i = 0; i < parseFloat(jQuery("#n-value").val()); i++) {
                        jQuery('#db-table5').append('<tr class="db5"><th>' + data[i]["route_name"] + '</th><th>' + data[i]["route_len"] + '</th><th>' + data[i]["file_name"] + '</th><th>' + data[i]["route_id"] + '</th></tr>');
                    }
                },
                fail: function (error) {
                    alert('Display failed');
                    console.log(error); 
                }
            });
        });


    });
    
    jQuery("#db-check2").prop("checked", false);
    if (jQuery('#db-check2').is(":checked")){
        jQuery('#db-div2').show();

    } else {
        jQuery('#db-div2').hide();
    }
    jQuery("#db-check3").prop("checked", false);
    if (jQuery('#db-check3').is(":checked")){
        jQuery('#db-div3').show();

    } else {
        jQuery('#db-div3').hide();
    }
    jQuery("#db-check4").prop("checked", false);
    if (jQuery('#db-check4').is(":checked")){
        jQuery('#db-div4').show();

    } else {
        jQuery('#db-div4').hide();
    }
    jQuery("#db-check5").prop("checked", false);
    if (jQuery('#db-check5').is(":checked")){
        jQuery('#db-div5').show();

    } else {
        jQuery('#db-div5').hide();
    }

});
