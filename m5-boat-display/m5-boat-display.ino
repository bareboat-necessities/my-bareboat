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

void on_nmea_sentence(String &line) {
  for (int i = 0; i < line.length(); i++) {
    gps.encode(line.charAt(i));
  }
  //if (gps.location.isUpdated()) {
    displayInfo();
  //}
}

void on_nmea_sentence_debug(String &line) {
  ez.canvas.println(line);
}

void displayInfo() {
  ez.canvas.font(&FreeSans12pt7b);
  ez.canvas.lmargin(10);
  ez.canvas.y(ez.canvas.top() + 10);
  ez.canvas.x(ez.canvas.lmargin());
  if (gps.location.isValid()) {
    float latRaw = gps.location.lat();
    String northSouth = "N ";
    if (latRaw < 0) {
      northSouth = "S ";
    }
    String latiTude = northSouth;
    latiTude += degreesToDegMin(latRaw);

    M5.lcd.fillRect(ez.canvas.lmargin(), ez.canvas.top() + 10, 320, 23, ez.theme->background); //erase partial place for updating data
    ez.canvas.print("LAT: ");
    ez.canvas.println(latiTude);
    // Kludge to get a degree symbol
    M5.Lcd.drawEllipse(ez.canvas.lmargin() + 164, ez.canvas.top() + 13, 3, 3, ez.theme->foreground);

    float lonRaw = gps.location.lng();
    String eastWest = "E ";
    if (lonRaw < 0) {
      eastWest = "W ";
    }
    String longiTude = eastWest;
    longiTude += degreesToDegMin(lonRaw);
    M5.lcd.fillRect(10, ez.canvas.top() + 40, 320, 23, ez.theme->background); //erase partial place for updating data
    ez.canvas.print("LON: ");
    ez.canvas.println(longiTude);
    // Kludge to get a degree symbol
    M5.Lcd.drawEllipse(ez.canvas.lmargin() + 164, ez.canvas.top() + 43, 3, 3, ez.theme->foreground);
  } else {
    ez.canvas.println("LAT:  -");
    ez.canvas.println("LON:  -");
  }

  // Speed over ground
  if (gps.speed.isValid()) {
    M5.lcd.fillRect(10, ez.canvas.top() + 70, 320, 23, ez.theme->background); //erase partial place for updating data
    ez.canvas.print("SOG:");
    if (gps.speed.knots() < 1) ez.canvas.print(" ");
    ez.canvas.println(gps.speed.knots(), 1);
  } else {
    ez.canvas.println("SOG:  -");
  }

  // Course over ground
  if (gps.course.isValid()) {
    ez.canvas.print("COG:");
    ez.canvas.println(gps.course.deg(), 0);
    M5.Lcd.drawEllipse(ez.canvas.lmargin() + 276, ez.canvas.top() + 73, 3, 3, ez.theme->foreground);
  } else {
    ez.canvas.println("COG:  -");
  }
}

String degreesToDegMin(float x) {
  if (x < 0) {
    x = (-x);
  }
  int degRaw = x;
  float degFloat = float(degRaw);
  float minutesRemainder = (x - degFloat) * 60.0;
  String degMin = "";
  if (degRaw < 100) degMin = "0";
  if (degRaw < 10) degMin = degMin + "0";
  degMin = degMin + degRaw;
  degMin = degMin + " "; //leave some space for the degree character which comes later
  if (minutesRemainder < 10) degMin = degMin + "0";
  degMin = degMin + String(minutesRemainder, 4);
  degMin = degMin + "\'";

  return (degMin);
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
