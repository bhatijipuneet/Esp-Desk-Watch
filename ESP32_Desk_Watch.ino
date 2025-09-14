#include <WiFi.h>
#include <LiquidCrystal.h>
#include "time.h"
#include <WebServer.h>

const char* ssid     = "Amit Bhati_4G";
const char* password = "Ashok@01#";

LiquidCrystal lcd(21, 22, 19, 18, 23, 25); // RS, E, D4-D7 pins

const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 19800; // GMT+5:30
const int daylightOffset_sec = 0;

const char* daysOfWeek[7] = {"Sun","Mon","Tue","Wed","Thu","Fri","Sat"};

WebServer server(80);

String customMessage = "";
bool displayOn = true;

int scrollIndex = 0;
unsigned long lastScrollTime = 0;
const int scrollInterval = 600;
unsigned long messageStartTime = 0;
const unsigned long messageDuration = 10000;

// Web page
const char* htmlPage = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>ESP32 Desk Watch Control</title>
<style>
  body { font-family: Arial; text-align: center; padding-top: 30px; }
  input[type=text] { width: 80%; font-size: 20px; padding: 10px; }
  button { font-size: 20px; padding: 10px 20px; margin: 10px; }
  footer { font-size: 12px; margin-top: 20px; color: gray; }
</style>
<script>
function sendMessage() {
  var msg = document.getElementById("msg").value;
  var xhr = new XMLHttpRequest();
  xhr.open("GET", "/send?msg=" + encodeURIComponent(msg), true);
  xhr.send();
  document.getElementById("msg").value = "";
}
function toggleDisplay() {
  var xhr = new XMLHttpRequest();
  xhr.open("GET", "/toggle", true);
  xhr.send();
}
</script>
</head>
<body>
<h2>ESP32 Desk Watch Control</h2>
<input type="text" id="msg" placeholder="Type message" maxlength="64">
<button onclick="sendMessage()">Send Message</button>
<br>
<button onclick="toggleDisplay()">Toggle Display ON/OFF</button>
<footer>© 2025 Puneet Bhati</footer>
</body>
</html>
)rawliteral";

void showStartupMessage() {
  String message = "Made By Puneet Bhati  ";
  int len = message.length();
  for(int i = 0; i < len; i++){
    lcd.clear();
    String part = message.substring(i);
    if(part.length() < 16){
      part += message.substring(0,16 - part.length());
    }
    lcd.setCursor(0,0);
    lcd.print(part.substring(0,16));
    lcd.setCursor(0,1);
    lcd.print("ESP32 Desk Watch");
    delay(400);
  }
}

void handleRoot() { server.send(200, "text/html", htmlPage); }
void handleSendMessage() {
  if(server.hasArg("msg")){
    customMessage = server.arg("msg");
    scrollIndex = 0;
    messageStartTime = millis();
  }
  server.send(200,"text/plain","OK");
}
void handleToggle() {
  displayOn = !displayOn;
  server.send(200,"text/plain", displayOn ? "ON":"OFF");
}

void setup() {
  lcd.begin(16,2);
  showStartupMessage();

  lcd.clear();
  lcd.print("Connecting WiFi");
  WiFi.begin(ssid,password);
  while(WiFi.status()!=WL_CONNECTED){
    delay(500);
    lcd.print(".");
  }
  lcd.clear();
  lcd.print("WiFi Connected");
  lcd.setCursor(0,1);
  lcd.print(WiFi.localIP());
  delay(5000);
  lcd.clear();

  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

  server.on("/",handleRoot);
  server.on("/send",handleSendMessage);
  server.on("/toggle",handleToggle);
  server.begin();
}

void loop() {
  server.handleClient();
  lcd.clear();

  if(!displayOn){
    lcd.setCursor(0,0);
    lcd.print("Display OFF");
    return;
  }

  if(customMessage != "" && millis()-messageStartTime < messageDuration){
    unsigned long now = millis();
    if(now - lastScrollTime >= scrollInterval){
      lastScrollTime = now;
      scrollIndex++;
      if(scrollIndex > customMessage.length()) scrollIndex = 0;
    }

    lcd.setCursor(0,0);
    if(customMessage.length() <= 16){
      lcd.print(customMessage);
    } else {
      String part1 = customMessage.substring(scrollIndex);
      if(part1.length()<16){
        part1 += " " + customMessage.substring(0,16-part1.length());
      }
      lcd.print(part1.substring(0,16));
    }
    lcd.setCursor(0,1);
    lcd.print("ESP32 Desk Watch");

  } else {
    customMessage = "";

    struct tm timeinfo;
    if(!getLocalTime(&timeinfo)){
      lcd.setCursor(0,0);
      lcd.print("Time not avail");
      delay(1000);
      return;
    }

    int hour12 = timeinfo.tm_hour % 12;
    if(hour12 == 0) hour12 = 12;
    const char* ampm = (timeinfo.tm_hour >= 12) ? "PM" : "AM";

    lcd.setCursor(0,0);
    lcd.printf("%02d:%02d:%02d %s", hour12, timeinfo.tm_min, timeinfo.tm_sec, ampm);

    lcd.setCursor(0,1);
    lcd.printf("%s %02d/%02d/%04d", daysOfWeek[timeinfo.tm_wday],
               timeinfo.tm_mday, timeinfo.tm_mon+1, timeinfo.tm_year+1900);
  }

  delay(200);
}
