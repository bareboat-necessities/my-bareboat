#include <M5ez.h>
#include <ezTime.h>
#include <WiFi.h>
#include <Preferences.h>

#include "UbuntuMono_Bold12pt7b.h"
#include "TinyGPS++.h"
#include "images.h"

#define MAIN_DECLARED

const char* host;
const int port = 10110;

char* nmea_sources[] = {"openplotter", "gl-x750"};

const char* EXIT = "Exit";
const String back_button = EXIT;

TinyGPSPlus gps;

// Sample: $WIMWV,27,R,00,N,A*26
char* wind_prefix[] = {"WIMWV", "IIMWV"};

TinyGPSCustom windAngle(gps, wind_prefix[0], 1); // Example: 214.8
TinyGPSCustom windReference(gps, wind_prefix[0], 2); // Reference: R = Relative, T = True
TinyGPSCustom windSpeed(gps, wind_prefix[0], 3); // Example: 0.1
TinyGPSCustom windSpeedUnit(gps, wind_prefix[0], 4); // Units: M = Meter per second, N = Knots, K = Kilometres per hour

TinyGPSCustom windAngleI(gps, wind_prefix[1], 1); // Example: 214.8
TinyGPSCustom windReferenceI(gps, wind_prefix[1], 2); // Reference: R = Relative, T = True
TinyGPSCustom windSpeedI(gps, wind_prefix[1], 3); // Example: 0.1
TinyGPSCustom windSpeedUnitI(gps, wind_prefix[1], 4); // Units: M = Meter per second, N = Knots, K = Kilometres per hour

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
  ez.settings.menuObj.addItem("NMEA source", nmea_source_menu);
  nmea_source_begin();
  ez.begin();
}

void loop() {
  ezMenu mainmenu(F("M5 Boat Display"));
  mainmenu.txtSmall();
  mainmenu.addItem(F("Location"), mainmenu_location);
  mainmenu.addItem(F("Wind"), mainmenu_wind);
  mainmenu.addItem(F("NMEA Debug"), mainmenu_nmea_debug);
  mainmenu.addItem(F("System Info"), mainmenu_sys);
  mainmenu.addItem(F("Built-in WiFi & Settings"), ez.settings.menu);
  mainmenu.upOnFirst(F("last|up"));
  mainmenu.downOnLast(F("first|down"));
  mainmenu.run();
}

void nmea_source_begin() {
  host = nmea_sources[0];
  Preferences prefs;
  prefs.begin("M5ez", true);  // read-only
  nmea_source_select(prefs.getString("nmeaSource", "").c_str());
  prefs.end();
}

void nmea_source_select(const char* selectedSource) {
  host = nmea_sources[0];
  uint8_t size = sizeof(nmea_sources) / sizeof(nmea_sources[0]);
  for (uint8_t n = 0; n < size; n++) {
    if (!strcmp(selectedSource, nmea_sources[n])) {
      host = nmea_sources[n];
    }
  }
}

void nmea_source_menu() {
  const char* orig_name = host;
  ezMenu thememenu("NMEA Source");
  thememenu.txtSmall();
  thememenu.buttons("up#Back#select##down#");
  uint8_t size = sizeof(nmea_sources) / sizeof(nmea_sources[0]);
  for (uint8_t n = 0; n < size; n++) {
    if (!strcmp(orig_name, nmea_sources[n])) {
      thememenu.addItem(nmea_sources[n]);
    }
  }
  for (uint8_t n = 0; n < size; n++) {
    if (strcmp(orig_name, nmea_sources[n])) {
      thememenu.addItem(nmea_sources[n]);
    }
  }
  while (thememenu.runOnce()) {
    if (thememenu.pick()) {
      nmea_source_select(thememenu.pickName().c_str());
    }
  }
  if (!strcmp(orig_name, host)) {
    Preferences prefs;
    prefs.begin("M5ez");
    prefs.putString("nmeaSource", host);
    prefs.end();
  }
}

void mainmenu_nmea_debug() {
  ez.screen.clear();
  ez.header.show(F("NMEA Debug"));
  ez.buttons.show("#" + back_button + "#");
  ez.canvas.font(&FreeSans9pt7b);
  boolean done = nmea_loop(true, 5, on_nmea_sentence_debug);
  checkDone(done);  
}

void mainmenu_location() {
  ez.screen.clear();
  ez.header.show(F("Location"));
  ez.buttons.show("#" + back_button + "#");
  ez.canvas.font(&FreeSans9pt7b);
  boolean done = nmea_loop(false, -1, on_nmea_sentence_loc);  
  checkDone(done);  
}

void mainmenu_wind() {
  ez.screen.clear();
  ez.header.show(F("Wind"));
  ez.buttons.show("#" + back_button + "#");
  ez.canvas.font(&FreeSans9pt7b);
  boolean done = nmea_loop(false, -1, on_nmea_sentence_wind);  
  checkDone(done);  
}

void checkDone(boolean done) {
  if (!done) {
    while (!nmea_loop_interrupted()) {}
  }
}

boolean nmea_loop_interrupted() {
  String btn = ez.buttons.poll();
  return btn == EXIT;
}

boolean nmea_loop(boolean debug, int maxLines, void (&onMessage)(const char*)) {
  WiFiClient client;
  if (!client.connect(host, port)) {
    ez.canvas.print(host);
    ez.canvas.print(": ");
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
      onMessage(line.c_str());
      boolean done = nmea_loop_interrupted();
      if ((maxLines > 0 && lines >= maxLines) || done) {
        client.stop();
        return done;
      }
    }
  }  
}

void on_nmea_sentence_loc(const char* line) {
  parse_sentence(line);
  displayLocInfo();
}

void on_nmea_sentence_wind(const char* line) {
  // parse_sentence("$WIMWV,27,R,00,N,A*26\r");
  parse_sentence(line);
  displayWindInfo();
}

void parse_sentence(const char* line) {
  for (int i = 0; *line != '\0'; i++, line++) {
    gps.encode(*line);
  }
}

void on_nmea_sentence_debug(const char* line) {
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
  sprintf(valueStr, "%c %03d", p, (int) value);
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
  sprintf(valueStr, "%08.5f'", minutesRemainder);
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

void displayWindInfo() {
  drawWindScreen();
}

#define DEG2RAD 0.0174532925

int fillArc(int x, int y, int start_angle, int seg_count, int rx, int ry, int w, unsigned int color) {

  byte seg = 5; // Angle a single segment subtends
  byte inc = 4; // Draw segments every 6 degrees

  // Draw colour blocks every inc degrees
  for (int i = start_angle; i < start_angle + seg * seg_count; i += inc) {
    // Calculate pair of coordinates for segment start
    float sx = cos((i - 90) * DEG2RAD);
    float sy = sin((i - 90) * DEG2RAD);
    uint16_t x0 = sx * (rx - w) + x;
    uint16_t y0 = sy * (ry - w) + y;
    uint16_t x1 = sx * rx + x;
    uint16_t y1 = sy * ry + y;

    // Calculate pair of coordinates for segment end
    float sx2 = cos((i + seg - 90) * DEG2RAD);
    float sy2 = sin((i + seg - 90) * DEG2RAD);
    int x2 = sx2 * (rx - w) + x;
    int y2 = sy2 * (ry - w) + y;
    int x3 = sx2 * rx + x;
    int y3 = sy2 * ry + y;

    M5.Lcd.fillTriangle(x0, y0, x1, y1, x2, y2, color);
    M5.Lcd.fillTriangle(x1, y1, x2, y2, x3, y3, color);
  }
  return 0;
}

float degToRad(int angleDeg) {
  return angleDeg * 71 / 4068.0;
}

void drawTick(int angleDeg, int circleCenterX, int circleCenterY, int rLow, int rHigh, float archDeg, unsigned int color) {
    float roseAnglemarkradian = degToRad(angleDeg - 90);
    float roseAnglemarkradianH = degToRad(angleDeg - 90 + archDeg);
    float roseAnglemarkradianL = degToRad(angleDeg - 90 - archDeg);
    float co = cos(roseAnglemarkradian);
    float si = sin(roseAnglemarkradian);
    float coH = cos(roseAnglemarkradianH);
    float siH = sin(roseAnglemarkradianH);
    float coL = cos(roseAnglemarkradianL);
    float siL = sin(roseAnglemarkradianL);
    M5.Lcd.fillTriangle(round(circleCenterX + rLow * co), round(circleCenterY + rLow * si), 
                        round(circleCenterX + rHigh * coH), round(circleCenterY + rHigh * siH), 
                        round(circleCenterX + rHigh * coL), round(circleCenterY + rHigh * siL), color);
}

void drawWindScreen() {
  int circleCenterX = ez.canvas.lmargin() + 160;
  int circleCenterY = ez.canvas.top() + 100;
  // do the small ticks every 10 degrees
  int roseAnglemark = 0;
  while (roseAnglemark < 360) {
    drawTick(roseAnglemark, circleCenterX, circleCenterY, 83, 89, 1.0, ez.theme->foreground);
    roseAnglemark += 10;
  }
  // do the longer ticks every 30 degrees
  roseAnglemark = 0;
  while (roseAnglemark < 360) {
    drawTick(roseAnglemark, circleCenterX, circleCenterY, 71, 89, 1.0, ez.theme->foreground);
    roseAnglemark += 30;
  }
  // put red and green arcs on each side
  fillArc(circleCenterX, circleCenterY, 20, 8, 97, 97, 8, TFT_GREEN);
  fillArc(circleCenterX, circleCenterY, 300, 8, 97, 97, 8, TFT_RED);

  // put App and True on left and right
  ez.canvas.pos(ez.canvas.lmargin() + 10, ez.canvas.top() + 10);
  ez.canvas.println("App");
  ez.canvas.pos(ez.canvas.lmargin() + 250, ez.canvas.top() + 10);
  ez.canvas.print("True");
}

void displayLocInfo() {
  ez.canvas.font(&UbuntuMono_Bold12pt7b);
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
  if (!valid || date.day() == 0) {
    initNA(buf);
  } else {
    sprintf(buf, "%02d/%02d %02d:%02d:%02d", date.day(), date.month(), time.hour(), time.minute(), time.second());
  }
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

void powerOff() { 
  m5.powerOFF(); 
}

void aboutM5boat() {
  ez.msgBox(F("About M5 Boat Display"), F("By Bareboat-Necessities | | bareboat-necessities.github.io"));
}
