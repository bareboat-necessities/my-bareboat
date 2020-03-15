#include <M5ez.h>
#include <ezTime.h>
#include <WiFi.h>

#include "images.h"

#define MAIN_DECLARED

const char* host = "gl-x750";
const int port = 10110;

const char* EXIT = "Exit";
String back_button = EXIT;

void setup() {
  #include <themes/default.h>
  #include <themes/dark.h>
  ezt::setDebug(INFO);
  ez.begin();
}

void loop() {
  ezMenu mainmenu("M5 Boat Display");
  mainmenu.txtSmall();
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
  nmea_loop();  
  while (!nmea_loop_interrupted()) {
  }
}

boolean nmea_loop_interrupted() {
    String btn = ez.buttons.poll();
    return btn == EXIT;
}

void nmea_loop() {
  WiFiClient client;
  if (!client.connect(host, port)) {
    ez.canvas.println("connection failed");
    return;
  }
  ez.canvas.println("connection succeeded");

  int lines = 0;
  const int MAX_LINES = 5;
  while (true) {
    unsigned long timeout = millis();
    while (client.available() == 0) {
      if (millis() - timeout > 5000) {
         ez.canvas.println(">>> client timeout!");
         client.stop();
         return;
      }
      delay(100);
    }
    
    while (client.available()) {
       String line = client.readStringUntil('\n');
       on_nmea_sentence(line);
       lines++;
       if (lines >= MAX_LINES) {
         client.stop();
         return;
       }
    }
  }  
}

boolean on_nmea_sentence(String line) {
  ez.canvas.println(line);
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
  ez.msgBox("About M5 Boat Display", "By Bareboat-Necessities | | https://bareboat-necessities.github.io");
}
