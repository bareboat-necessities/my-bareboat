#include <M5ez.h>
#include <ezTime.h>
#include <WiFi.h>

#include "TinyGPS++.h"
#include "images.h"

#define MAIN_DECLARED

const char* host = "gl-x750";
const int port = 10110;

const char* EXIT = "Exit";
String back_button = EXIT;

TinyGPSPlus gps;

void setup() {
  #include <themes/dark.h>
  #include <themes/default.h>
  ezt::setDebug(INFO);
  ez.begin();
}

void loop() {
  ezMenu mainmenu("M5 Boat Display");
  mainmenu.txtSmall();
  mainmenu.addItem("Location", mainmenu_location);
  mainmenu.addItem("NMEA Debug", mainmenu_nmea_debug);
  mainmenu.addItem("System Info", mainmenu_sys);
  mainmenu.addItem("Built-in WiFi & Settings", ez.settings.menu);
  mainmenu.upOnFirst("last|up");
  mainmenu.downOnLast("first|down");
  mainmenu.run();
}

void mainmenu_nmea_debug() {
  ez.screen.clear();
  ez.header.show("NMEA Debug");
  ez.buttons.show("#" + back_button + "#");
  ez.canvas.font(&FreeSans9pt7b);
  const int MAX_LINES = 5;
  boolean done = nmea_loop(true, MAX_LINES, on_nmea_sentence_debug);  
  if (!done) {
    while (!nmea_loop_interrupted()) {}
  }
}

void mainmenu_location() {
  ez.screen.clear();
  ez.header.show("Location");
  ez.buttons.show("#" + back_button + "#");
  ez.canvas.font(&FreeSans9pt7b);
  boolean done = nmea_loop(false, -1, on_nmea_sentence);  
  if (!done) {
    while (!nmea_loop_interrupted()) {}
  }
}

boolean nmea_loop_interrupted() {
  String btn = ez.buttons.poll();
  return btn == EXIT;
}

boolean nmea_loop(boolean debug, int maxLines, void (&onMessage)(String&)) {
  WiFiClient client;
  if (!client.connect(host, port)) {
    ez.canvas.println("connection failed");
    return false;
  }
  if (debug) ez.canvas.println("connection succeeded");

  int lines = 0;
  while (true) {
    unsigned long timeout = millis();
    while (client.available() <= 0) {
      if (millis() - timeout > 5000) {
        ez.canvas.println(">>> client timeout!");
        client.stop();
        return false;
      }
      boolean done = nmea_loop_interrupted();
      if (done) {
        client.stop();
        return true;
      }
      delay(10);
    }
    
    while (client.available() > 0) {
      String line = client.readStringUntil('\n');
      lines++;
      onMessage(line);
      boolean done = nmea_loop_interrupted();
      if ((maxLines > 0 && lines >= maxLines) || done) {
        client.stop();
        return done;
      }
    }
  }  
}

void on_nmea_sentence(String& line) {
  for (int i = 0; i < line.length(); i++) {
    gps.encode(line.charAt(i));
  }
  displayInfo();
}

void on_nmea_sentence_debug(String& line) {
  ez.canvas.println(line);
}

void printSpeed(char* label, boolean valid, float value, boolean prevValid, float prevValue, char* ending) {
  printLabel(label);
  String prevSpeed = formatSpeed(prevValue, prevValid, ending);
  String speed = formatSpeed(value, valid, ending);
  if (prevSpeed != speed) {
    erase(prevSpeed);
  }
  print(speed);
  ez.canvas.println();
}

String formatSpeed(float value, boolean valid, char* ending) {
  return valid ? String(value, 1) + ' ' + ending : String('-');  
}

void printAngle(char* label, boolean valid, float value, boolean prevValid, float prevValue) {
  printLabel(label);
  String prevAngle = formatAngle(prevValue, prevValid);
  String angle = formatAngle(value, valid);
  if (prevAngle != angle) {
    erase(prevAngle);
  }
  print(angle);
  if (valid) {
    printDegree();
  }
  ez.canvas.println();
}

String formatAngle(float value, boolean valid) {
  String str = String(value > 0 ? value : -value, 0);
  str.trim();
  return valid ? str : String('-');  
}

void printCoord(char* label, boolean valid, float value, boolean prevValid, float prevValue, char a, char b) {
  printLabel(label);
  String prefix = coordPrefix(value, valid, a, b);
  String prevPrefix = coordPrefix(prevValue, prevValid, a, b);
  String suffix = coordSuffix(value, valid);
  String prevSuffix = coordSuffix(prevValue, prevValid);
  if (prevPrefix != prefix) {
    erase(prevPrefix);
  }
  print(prefix);
  if (valid) {
    printDegree();
  }
  if (prevSuffix != suffix) {
    erase(prevSuffix);
  }
  print(suffix);
  ez.canvas.println();
}

void printDegreeWithColor(uint16_t color) {
  M5.Lcd.drawEllipse(ez.canvas.x() + 5, ez.canvas.y() + 4, 3, 3, color);
  M5.Lcd.drawEllipse(ez.canvas.x() + 5, ez.canvas.y() + 4, 4, 4, color);
}

void printDegree() {
  printDegreeWithColor(ez.theme->foreground);
  ez.canvas.x(ez.canvas.x() + 20);
}

void eraseDegree() {
  printDegreeWithColor(ez.theme->background);
}

String coordPrefix(float value, boolean valid, char a, char b) {
  if (!valid) {
    return String('-');
  }
  String result = String(a);
  if (value < 0) {
    result = String(b);
    value = (-value);
  }
  result += ' ';
  int deg = value;
  String degrees = "";
  if (deg < 100) degrees += '0';
  if (deg < 10) degrees += '0';
  degrees = degrees + deg;
  return result + degrees;  
}

String coordSuffix(float value, boolean valid) {
  if (!valid) {
    return String();
  }
  float deg = value;
  if (deg < 0) {
    deg = (-deg);
  }
  float minutesRemainder = (deg - ((int)deg)) * 60.0;
  String minutes = "";
  if (minutesRemainder < 10) minutes = minutes + "0";
  return minutes + String(minutesRemainder, 5) + '\'';
}

void erase(String& value) {  
  uint16_t fg = ez.canvas.color();
  uint16_t bg = ez.theme->background;
  int x = ez.canvas.x();
  int y = ez.canvas.y();  
  ez.canvas.color(bg);
  ez.canvas.print(value);
  eraseDegree();
  ez.canvas.color(fg);
  ez.canvas.x(x);
  ez.canvas.y(y);
}

void print(String& value) {
  ez.canvas.print(value);
}

void printLabel(char* label) {
  ez.canvas.print(label);  
  ez.canvas.print(": ");
}

void printTime(char* label, boolean valid, String& value, boolean prevValid, String& prevValue) {
  printLabel(label);
  if (prevValue != value) {
    erase(prevValue);
  }
  print(value);
  ez.canvas.println();
}

boolean prevValidLOC = false;
boolean prevValidSOG = false;
boolean prevValidCOG = false;
boolean prevValidTime = false;

float prevLAT;
float prevLON;
float prevSOG;
float prevCOG;
String prevTime = String('-');

void displayInfo() {
  ez.canvas.font(&FreeMonoBold12pt7b);
  ez.canvas.lmargin(10);
  ez.canvas.y(ez.canvas.top() + 10);
  ez.canvas.x(ez.canvas.lmargin());

  boolean locValid = gps.location.isValid();
  printCoord("LAT", locValid, gps.location.lat(), prevValidLOC, prevLAT, 'N', 'S'); 
  prevLAT = gps.location.lat();
  
  printCoord("LON", locValid, gps.location.lng(), prevValidLOC, prevLON, 'E', 'W'); 
  prevLON = gps.location.lng();
  prevValidLOC = locValid;
  
  printSpeed("SOG", gps.speed.isValid(), gps.speed.knots(), prevValidSOG, prevSOG, "kt"); 
  prevSOG = gps.speed.knots(); 
  prevValidSOG = gps.speed.isValid();
  
  printAngle("COG", gps.course.isValid(), gps.course.deg(), prevValidCOG, prevCOG); 
  prevCOG = gps.course.deg(); 
  prevValidCOG = gps.course.isValid();

  String gpsTime = toTimestamp(gps.time.isValid(), gps.time, gps.date);
  printTime("UTC", gps.time.isValid(), gpsTime, prevValidTime, prevTime); 
  prevTime = gpsTime; 
  prevValidTime = gps.time.isValid();
}

String toTimestamp(boolean valid, TinyGPSTime& time, TinyGPSDate& date) {
  if (!valid) {
    return String('-');
  }
  char buf[20];
  sprintf(buf, "%02d/%02d %02d:%02d:%02d", date.day(), date.month(), time.hour(), time.minute(), time.second());
  return String(buf);  
}

void mainmenu_sys() {
  ezMenu images;
  images.imgBackground(TFT_BLACK);
  images.imgFromTop(40);
  images.imgCaptionColor(TFT_WHITE);
  images.addItem(wifi_jpg, "WiFi Settings", ez.wifi.menu);
  images.addItem(sysinfo_jpg, "System Status", sysInfo);
  images.addItem(about_jpg, "About M5 Boat Display", aboutM5boat);
  images.addItem(sleep_jpg, "Power Off", powerOff);
  images.addItem(return_jpg, "Back");
  images.run();
}

void powerOff() { m5.powerOFF(); }

void aboutM5boat() {
  ez.msgBox("About M5 Boat Display", "By Bareboat-Necessities | | bareboat-necessities.github.io");
}
