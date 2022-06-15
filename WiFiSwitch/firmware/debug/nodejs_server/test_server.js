const express = require("express");
const fileUpload = require('express-fileupload');
const bodyParser = require('body-parser');

const app = express();

function getBoardConfig()
{
  var aBoardInfo = {
    "board_name": "Datcha WiFi switch",
    "board_hw_version": "v1.0.0",
    "board_sw_version": "v2.1.3",
    "board_desc": "Datcha WiFi switch",
    "manufacturer": "Maygli <a href=\"mailto:mmaygli@gmail.com\">mmaygli@gmail.com</a>",
    "copyright": "&copy; Maygli 2021",
    "devices": "wifi_switch"
  };
  return JSON.stringify(aBoardInfo)
}

function getSwitchSettings()
{
  var aWiFiSettings = {
    "on_brightness": 47,
    "off_brightness": 36,
    "sound": "off",
    "style": 1,
  };
  return JSON.stringify(aWiFiSettings)
}

function getWiFiSettings(){
  var aWiFiSettings = {
    "is_st" : "on", 
    "ap_ssid":"Test",
    "ap_pwd":"11111111111111111",
    "ap_fixed_ip":"off",
    "ap_ip":"192.168.1.11",
    "ap_netmask":"255.255.255.0",
    "ap_gateway":"192.168.1.1",
    "st_ssid":"MaygliHome",
    "st_pwd":"1",
    "st_fixed_ip":"on",
    "st_ip": "192.168.1.70",
    "st_netmask":"255.255.0.0",
    "st_gateway":"192.168.2.2",
    "st_is_ap_after":"on",
    "st_attempts": "5"
  };
  return JSON.stringify(aWiFiSettings)
}

function getMeteoState(){
  var aMeteo = {
    "is_temp" : true, 
    "temp" : 23.4,
    "temp_unit_index":0,
    "is_pressure": true,
    "pressure": 762,
    "pressure_unit_index" : 0,
    "is_humidity" : false,
    "humidity": 27,
    "humidity_unit_index" : 0
  };
  return JSON.stringify(aMeteo)
}

app.use(fileUpload())
app.post('/internal_upload', function(req, res) {
  console.log(req.files);
  if (!req.files || Object.keys(req.files).length === 0) {
    return res.status(400).send('No files were uploaded.');
  }
  res.send('File uploaded!');


  // The name of the input field (i.e. "sampleFile") is used to retrieve the uploaded file
/*  let sampleFile = req.files.sampleFile;

  // Use the mv() method to place the file somewhere on your server
  sampleFile.mv('/somewhere/on/your/server/filename.jpg', function(err) {
    if (err)
      return res.status(500).send(err);

    res.send('File uploaded!');
  });*/
});


app.use(express.static('../../data'));

app.get("/switch_settings", function(request, response){
    response.send(getSwitchSettings());
});

app.use(bodyParser.json());

app.post("/switch_settings", function(request, response){
     console.log("Set switch parameters");
     console.log(request.body);
});

app.get("/wifi_settings", function(request, response){
    response.send(getWiFiSettings());
});

app.post("/wifi_settings", function(request, response){
     console.log("Set wifi parameters");
     console.log(request.body);
     response.send("Ok");
});

app.get("/board_info", function(request, response){
    response.send(getBoardConfig());
});

app.get("/meteo_state", function(request, response){
    response.send(getMeteoState());
});

app.listen(3000);