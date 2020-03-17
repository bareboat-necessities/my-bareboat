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

boolean prevValidLOC = false;
boolean prevValidSOG = false;
boolean prevValidCOG = false;

float prevLAT;
float prevLON;
float prevSOG;
float prevCOG;

char prevLATprefix[6];
char prevLONprefix[6];
char prevLATsuffix[10];
char prevLONsuffix[10];
char prevSOGstr[10];
char prevCOGstr[5];

char LATprefix[6];
char LONprefix[6];
char LATsuffix[10];
char LONsuffix[10];
char SOGstr[10];
char COGstr[5];

char prevTime[20];
char gpsTime[20];

void setup() {
  #include <themes/dark.h>
  #include <themes/default.h>
  initNA(prevTime);
  initNA(gpsTime);
  ezt::setDebug(INFO);
  ez.begin();
}

void loop() {
  ezMenu mainmenu(F("M5 Boat Display"));
  mainmenu.txtSmall();
  mainmenu.addItem(F("Location"), mainmenu_location);
  mainmenu.addItem(F("NMEA Debug"), mainmenu_nmea_debug);
  mainmenu.addItem(F("System Info"), mainmenu_sys);
  mainmenu.addItem(F("Built-in WiFi & Settings"), ez.settings.menu);
  mainmenu.upOnFirst(F("last|up"));
  mainmenu.downOnLast(F("first|down"));
  mainmenu.run();
}

void mainmenu_nmea_debug() {
  ez.screen.clear();
  ez.header.show(F("NMEA Debug"));
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
  ez.header.show(F("Location"));
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
    ez.canvas.println(F("connection failed"));
    return false;
  }
  if (debug) ez.canvas.println(F("connection succeeded"));

  int lines = 0;
  while (true) {
    unsigned long timeout = millis();
    while (client.available() <= 0) {
      if (millis() - timeout > 5000) {
        ez.canvas.println(F(">> client timeout!"));
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

void printSpeed(char* label, boolean valid, float value, boolean prevValid, float prevValue, char* ending, char* speed, char* prevSpeed) {
  printLabel(label);
  formatSpeed(prevSpeed, prevValue, prevValid, ending);
  formatSpeed(speed, value, valid, ending);
  if (strcmp(prevSpeed, speed)) {
    erase(prevSpeed);
  }
  print(speed);
  ez.canvas.println();
}

void formatSpeed(char* valueStr, float value, boolean valid, char* ending) {
  if (!valid) {
    initNA(valueStr);
  } else {
    sprintf(valueStr, "%.1f %s", value, ending);
  }  
}

void printAngle(char* label, boolean valid, float value, boolean prevValid, float prevValue, char* valueStr, char* prevValueStr) {
  printLabel(label);
  formatAngle(prevValueStr, prevValue, prevValid);
  formatAngle(valueStr, value, valid);
  if (strcmp(prevValueStr, valueStr)) {
    erase(prevValueStr);
  }
  print(valueStr);
  if (valid) {
    printDegree();
  }
  ez.canvas.println();
}

void formatAngle(char* valueStr, float value, boolean valid) {
  if (!valid) {
    initNA(valueStr);
  } else {
    sprintf(valueStr, "%d", (int) value);
  }  
}

void printCoord(char* label, boolean valid, float value, boolean prevValid, float prevValue, char a, char b, 
                char* prefix, char* prevPrefix, char* suffix, char* prevSuffix) {
  printLabel(label);
  coordPrefix(prefix, value, valid, a, b);
  coordPrefix(prevPrefix, prevValue, prevValid, a, b);
  coordSuffix(suffix, value, valid);
  coordSuffix(prevSuffix, prevValue, prevValid);
  if (strcmp(prevPrefix, prefix)) {
    erase(prevPrefix);
  }
  print(prefix);
  if (valid) {
    printDegree();
  }
  if (strcmp(prevSuffix, suffix)) {
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

void coordPrefix(char* valueStr, float value, boolean valid, char a, char b) {
  if (!valid) {
    initNA(valueStr);
    return;
  }
  char p = a;
  if (value < 0) {
    p = b;
    value = (-value);
  }
  sprintf(valueStr, "%c %03d", p, (int)value);
}

void coordSuffix(char* valueStr, float value, boolean valid) {
  if (!valid) {
    valueStr[0] = '\0';
    return;
  }
  float deg = value;
  if (deg < 0) {
    deg = (-deg);
  }
  float minutesRemainder = (deg - ((int)deg)) * 60.0;
  sprintf(valueStr, "%08.5f", minutesRemainder);
}

void erase(char* value) {  
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

void print(char* value) {
  ez.canvas.print(value);
}

void printLabel(char* label) {
  print(label);  
  print(": ");
}

void printTime(char* label, char* value, char* prevValue) {
  printLabel(label);
  if (strcmp(prevValue, value)) {
    erase(prevValue);
  }
  print(value);
  ez.canvas.println();
}

void displayInfo() {
  ez.canvas.font(&FreeMonoBold12pt7b);
  ez.canvas.lmargin(10);
  ez.canvas.y(ez.canvas.top() + 10);
  ez.canvas.x(ez.canvas.lmargin());

  boolean locValid = gps.location.isValid();
  printCoord("LAT", locValid, gps.location.lat(), prevValidLOC, prevLAT, 'N', 'S', 
             LATprefix, prevLATprefix, LATsuffix, prevLATsuffix); 
  prevLAT = gps.location.lat();
  
  printCoord("LON", locValid, gps.location.lng(), prevValidLOC, prevLON, 'E', 'W',
             LONprefix, prevLONprefix, LONsuffix, prevLONsuffix); 
  prevLON = gps.location.lng();
  prevValidLOC = locValid;
  
  printSpeed("SOG", gps.speed.isValid(), gps.speed.knots(), prevValidSOG, prevSOG, "kt", SOGstr, prevSOGstr); 
  prevSOG = gps.speed.knots(); 
  prevValidSOG = gps.speed.isValid();
  
  printAngle("COG", gps.course.isValid(), gps.course.deg(), prevValidCOG, prevCOG, COGstr, prevCOGstr); 
  prevCOG = gps.course.deg(); 
  prevValidCOG = gps.course.isValid();

  toTimestamp(gpsTime, gps.time.isValid(), gps.time, gps.date);
  printTime("UTC", gpsTime, prevTime); 
  strcpy(prevTime, gpsTime); 
}

void toTimestamp(char* buf, boolean valid, TinyGPSTime& time, TinyGPSDate& date) {
  if (!valid) {
    initNA(buf);
  }
  sprintf(buf, "%02d/%02d %02d:%02d:%02d", date.day(), date.month(), time.hour(), time.minute(), time.second());
}

void initNA(char* buf) {
  buf[0] = '-';
  buf[1] = '\0';
}

void mainmenu_sys() {
  ezMenu images;
  images.imgBackground(TFT_BLACK);
  images.imgFromTop(40);
  images.imgCaptionColor(TFT_WHITE);
  images.addItem(wifi_jpg, F("WiFi Settings"), ez.wifi.menu);
  images.addItem(sysinfo_jpg, F("System Status"), sysInfo);
  images.addItem(about_jpg, F("About M5 Boat Display"), aboutM5boat);
  images.addItem(sleep_jpg, F("Power Off"), powerOff);
  images.addItem(return_jpg, F("Back"));
  images.run();
}

void powerOff() { m5.powerOFF(); }

void aboutM5boat() {
  ez.msgBox(F("About M5 Boat Display"), F("By Bareboat-Necessities | | bareboat-necessities.github.io"));
}
