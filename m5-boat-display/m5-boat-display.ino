#include <M5ez.h>
#include <ezTime.h>
#include <WiFi.h>
#include <Preferences.h>

#include "TinyGPS++.h"
#include "m5-draw.h"
#include "images.h"
#include "ubuntumono_regular14pt7b.h"

#define MAIN_DECLARED

const char* host;
const int port = 10110;

char* nmea_sources[] = {"openplotter", "gl-x750"};

const char* EXIT = "Exit";
const String back_button = EXIT;

TinyGPSPlus gps;

// Sample: $WIMWV,27,R,00,N,A*26
char* wind_prefix[] = {"WIMWV", "IIMWV"};

int wind_prefix_index = 0;

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

char tmp_buf[20];

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
  drawWindCircle();
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
        ez.canvas.println(F("timeout!"));
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

#define NMEA_END_CHAR_1 '\n'
#define NMEA_MAX_LENGTH 128

uint8_t nmea_get_checksum(const char *sentence) {
  const char *n = sentence + 1; // Plus one, skip '$'
  uint8_t chk = 0;
  /* While current char isn't '*' or sentence ending (newline) */
  while ('*' != *n && NMEA_END_CHAR_1 != *n) {
    if ('\0' == *n || n - sentence > NMEA_MAX_LENGTH) {
      /* Sentence too long or short */
      return 0;
    }
    chk ^= (uint8_t) *n;
    n++;
  }
  return chk;
}


char foo[128];
char foo2[128];

void gen_sentence() {
  float angle = (rand() % 3600) / 10.0;
  float speed = (rand() % 1000) / 10.0; 
  sprintf(foo, "$WIMWV,%.1f,R,%.1f,K,A*", angle, speed);
  uint8_t sum = nmea_get_checksum(foo);  
  sprintf(foo2, "%s%02X\r", foo, sum);
}

void on_nmea_sentence_wind(const char* line) {
  //parse_sentence("$WIMWV,27,R,00,N,A*26\r");
  //parse_sentence(line);

  gen_sentence();
  parse_sentence(foo2);
  parse_sentence(line);
  
  displayWindInfo();
}

void parse_sentence(const char* line) {
  for (; *line != '\0'; line++) {
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

void drawTicks(int circleCenterX, int circleCenterY, int rLow, int rHigh, int step, unsigned int color) {  
  int roseAnglemark = 0;
  while (roseAnglemark < 360) {
    drawTick(roseAnglemark, circleCenterX, circleCenterY, rLow, rHigh, 1.0, color);
    roseAnglemark += step;
  }
}

const char* wind_ref() {
  if (wind_prefix_index == 0) {
    return windReference.value();
  } else {
    return windReferenceI.value();
  }
}

const char* wind_speed() {
  if (wind_prefix_index == 0) {
    return windSpeed.value();
  } else {
    return windSpeedI.value();
  }
}

const char* wind_units() {
  if (wind_prefix_index == 0) {
    return windSpeedUnit.value();
  } else {
    return windSpeedUnitI.value();
  }
}

const char* wind_angle() {
  if (wind_prefix_index == 0) {
    return windAngle.value();
  } else {
    return windAngleI.value();
  }
}

const char* units_name(const char* units) {
  if (!strcmp("N", units)) {
    return "kt";
  } else if (!strcmp("K", units)) {
    return "km/h";
  } else if (!strcmp("M", units)) {
    return "m/s";
  } else {
    return "";
  }
}

int getCenterX() {
  return ez.canvas.lmargin() + 160;
}

int getCenterY() {
  return ez.canvas.top() + 100;
}

void drawWindCircle() {
  int circleCenterX = getCenterX();
  int circleCenterY = getCenterY();
  // do the small ticks every 10 degrees
  drawTicks(circleCenterX, circleCenterY, 83, 89, 10, ez.theme->foreground);
  // do the longer ticks every 30 degrees
  drawTicks(circleCenterX, circleCenterY, 71, 89, 30, ez.theme->foreground);
  // put red and green arcs on each side
  fillArc(circleCenterX, circleCenterY, 20, 8, 97, 97, 8, TFT_GREEN);
  fillArc(circleCenterX, circleCenterY, 300, 8, 97, 97, 8, TFT_RED);
}

void drawWindScreen() {
  int circleCenterX = getCenterX();
  int circleCenterY = getCenterY();

  boolean trueWind = !strcmp("T", wind_ref());
  
  const char* windType = trueWind ? "True" : "App";
  const char* windUnits = wind_units();
  const char* windAngle = wind_angle();

  // print wind speed
  int left = trueWind ? ez.canvas.lmargin() + 255 : ez.canvas.lmargin() + 1;
  ez.canvas.pos(left, ez.canvas.top() + 125);
  ez.canvas.println(windType);
  ez.canvas.pos(left, ez.canvas.top() + 145);
  sprintf(tmp_buf, "%.0f", parse_float(wind_speed()));
  print_speed(tmp_buf, units_name(windUnits));

  if (gps.course.isValid()) {
    float realWindangle = parse_float(windAngle);
    if ((gps.course.deg() + realWindangle) <= 360) {
      realWindangle = gps.course.deg() + realWindangle;
    } else {
      realWindangle = (gps.course.deg() + realWindangle) - 360;
    }
    ez.canvas.pos(left, ez.canvas.top() + 165);
    sprintf(tmp_buf, "%.0f", realWindangle);
    print_angle(tmp_buf);
  }

  if (!trueWind) {
    float angleDeg = parse_float(wind_angle());
    drawPointer(angleDeg, circleCenterX, circleCenterY, ez.theme->foreground);

    sprintf(tmp_buf, "%.0f", parse_float(wind_speed()));
    ez.canvas.pos(circleCenterX - (6 * (strlen(tmp_buf) + 1 + strlen(windUnits))) - 2, circleCenterY - 13);
    print_speed(tmp_buf, units_name(windUnits));

    // print angle in center
    ez.canvas.pos(circleCenterX - 21, circleCenterY + 7);
    if (angleDeg > 180) {
      sprintf(tmp_buf, "%03.0f", abs(angleDeg - 360));
    } else {
      sprintf(tmp_buf, "%03.0f", abs(angleDeg));
    }
    print_angle(tmp_buf);
    ez.canvas.pos(circleCenterX - 33, circleCenterY + 27);
  }
}

void drawPointer(float angleDeg, int circleCenterX, int circleCenterY, unsigned int color) {
  float windRad = degToRad(angleDeg - 90);
  float co = cos(windRad);
  float si = sin(windRad);
  float pointerWidth = 0.1745; // 10 degrees in radians. 
  // Get the coords of the pointer
  int xp = round(circleCenterX + (68 * cos(windRad)));
  int yp = round(circleCenterY + (68 * sin(windRad)));
  int xl = round(circleCenterX + (32 * cos(windRad - pointerWidth)));
  int yl = round(circleCenterY + (32 * sin(windRad - pointerWidth)));
  int xr = round(circleCenterX + (32 * cos(windRad + pointerWidth)));
  int yr = round(circleCenterY + (32 * sin(windRad + pointerWidth)));
  M5.Lcd.fillTriangle(xp, yp, xl, yl, xr, yr, color);
}

void print_angle(const char* angle) {
  ez.canvas.print(angle);
  printDegree();
}

void print_speed(const char* speed, const char* units) {
  ez.canvas.print(speed);
  ez.canvas.print(' ');
  ez.canvas.print(units);
}

float parse_float(const char* str) {
  return strtof(str, NULL);
}

void displayLocInfo() {
  ez.canvas.font(&ubuntumono_regular14pt7b);
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
