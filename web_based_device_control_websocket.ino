#include <WiFi.h>
#include <WebServer.h>
#include <WebSocketsServer.h>

//---------------------------------------------------

//---------------------------------------------------
#define LED1 2
#define LED2 4

//---------------------------------------------------
const char* ssid = "Pranay";
const char* password = "Pranay21";
//---------------------------------------------------
WebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);
//---------------------------------------------------
boolean LED1onoff=false; boolean LED2onoff=false;
String JSONtxt;
//---------------------------------------------------


const char webpageCont[] PROGMEM = 
R"=====(
<!DOCTYPE HTML>
<html>
<title>ESP32 WebSocket Server</title>
<!------------------------------------------CSS----------------------------------------->
<style>
  html {
    font-family: Arial, Helvetica, sans-serif;
    text-align: center;
  }
  h1 {
    font-size: 1.8rem;
    color: white;
  }
  h2{
    font-size: 1.5rem;
    font-weight: bold;
    color: #143642;
  }
  .topnav {
    overflow: hidden;
    background: #5D3FD3;
  }
  body {
    margin: 0;
    background-color:rgba(128,128,128,0.322)
  }
  .content {
    padding: 30px;
    max-width: 600px;
    margin: 0 auto;
  }
  .card {
    background-color: #F8F7F9;;
    box-shadow: 2px 2px 12px 1px rgba(140,140,140,.5);
    padding-top:10px;
    padding-bottom:20px;
  }
  .button {
    padding: 15px 50px;
    font-size: 24px;
    text-align: center;
    outline: none;
    color: #fff;
    background-color: #0f8b8d;
    border: none;
    border-radius: 5px;
    -webkit-touch-callout: none;
    -webkit-user-select: none;
    -khtml-user-select: none;
    -moz-user-select: none;
    -ms-user-select: none;
    user-select: none;
    -webkit-tap-highlight-color: rgba(0,0,0,0);
   }
   .button:hover {background-color: #34495e ;box-shadow: 0 12px 16px 0 rgba(0,0,0,0.24),0 17px 50px 0 rgba(0,0,0,0.19);}
   .button:active {
     background-color: #0f8b8d;
     box-shadow: 2 2px #CDCDCD;
     transform: translateY(2px);
   }
   .state {
     font-size: 1.5rem;
     color:#8c8c8c;
     font-weight: bold;
   }
  </style>
<title>ESP Web Server</title>
<meta name="viewport" content="width=device-width, initial-scale=1">
<link rel="icon" href="data:,">
</head>
<body>
  <div class="topnav">
    <h1>Web Based Device Control</h1>
  </div>
  <div class="content">
    <div class="card">
      <h2>Output - GPIO 2</h2>
      <p class="state">state: <span id="ls1"></span></p>
      <p><button id="button1" class="button" onclick="led1()"></button></p>
    </div>
    </div>
    <div class="content">
    <div class="card">
      <h2>Output - GPIO 4</h2>
      <p class="state">state: <span id="ls2"></span></p>
      <p><button id="button2" class="button" onclick="led2()"></button></p>
    </div>
    </div>
<!-------------------------------------JavaScript--------------------------------------->
<script>
  InitWebSocket()
  function InitWebSocket()
  {
    websock = new WebSocket('ws://'+window.location.hostname+':81/');
    websock.onmessage=function(evt)
    {
       JSONobj = JSON.parse(evt.data);
       
       document.getElementById('button1').innerHTML = JSONobj.LED1onoff;
       document.getElementById('button2').innerHTML = JSONobj.LED2onoff;
       document.getElementById('ls1').innerHTML = JSONobj.LED1STAT;
       if(JSONobj.LED1STAT == 'ON')
       {
        document.getElementById("ls1").innerHTML="ON";
        
       }
       else
       {
        document.getElementById("ls1").innerHTML="OFF";
        
       }  
       document.getElementById('ls2').innerHTML = JSONobj.LED2STAT;
       if(JSONobj.LED2STAT == 'ON')
       {
        document.getElementById("ls2").innerHTML="ON";
       
       }
       else
       {
        document.getElementById("ls2").innerHTML="OFF";
       
       }
      
    }
  }
  //----------------------------------------------------------------------------------
  function led1()
  {
    led1btn = 'LED1onoff=ON';
    if(document.getElementById('button1').innerHTML == 'ON')
    {
      led1btn = 'LED1onoff=OFF';
    }
    websock.send(led1btn);
  }
  //-----------------------------------------------------------------------------------
  function led2()
  {
    led2btn = 'LED2onoff=ON';
    if(document.getElementById('button2').innerHTML == 'ON')
    {
      led2btn = 'LED2onoff=OFF';
    }
    websock.send(led2btn);
  }
</script>
</html>
)=====";
void handleRoot()
{
  server.send(200,"text/html", webpageCont);
}
//=====================================================
//function process event: new data received from client
//=====================================================
void webSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t welength)
{
  String payloadString = (const char *)payload;
  //--------------------------------------------------
  if(type == WStype_TEXT)
  {
    byte separator=payloadString.indexOf('=');
    String var = payloadString.substring(0,separator);
    String val = payloadString.substring(separator+1);
    //------------------------------------------------
    if(var == "LED1onoff")
    {
      LED1onoff = false;
      if(val == "ON") LED1onoff = true;
    }
    //------------------------------------------------
    if(var == "LED2onoff")
    {
      LED2onoff = false;
      if(val == "ON") LED2onoff = true;
    }
  }
}
void setup()
{
  Serial.begin(115200);
  pinMode(LED1, OUTPUT); pinMode(LED2, OUTPUT);
  
  //-----------------------------------------------

  //-----------------------------------------------
  WiFi.begin(ssid, password);
  while(WiFi.status() != WL_CONNECTED)
  {
    Serial.print("."); delay(500);  
  }
  WiFi.mode(WIFI_STA);
  Serial.print(" Local IP: ");
  Serial.println(WiFi.localIP());
  //-----------------------------------------------
  server.on("/", handleRoot);
  server.begin();
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
}
void loop() 
{
  String led1status;
  String led2status;
  webSocket.loop(); server.handleClient();
  delay(1000);
  //----------------------------------------------------

  //----------------------------------------------------
  if(LED1onoff == false){
    digitalWrite(LED1, LOW);
    led1status = "OFF";
  }
  else{
    digitalWrite(LED1, HIGH);
    led1status = "ON";
  }
  
  //----------------------------------------------------
  if(LED2onoff == false) {
    digitalWrite(LED2, LOW);
    led2status = "OFF";
  }
  else{
    digitalWrite(LED2, HIGH);
    led2status = "ON";
  }
 
  //----------------------------------------------------
  //----------------------------------------------------
  JSONtxt = "{\"LED1onoff\":\""+led1status+"\",";
  JSONtxt += "\"LED2onoff\":\""+led2status+"\",";
  JSONtxt += "\"LED1STAT\":\""+led1status+"\",";
  JSONtxt += "\"LED2STAT\":\""+led2status+"\"}";
  //----------------------------------------------------
  webSocket.broadcastTXT(JSONtxt);
}
