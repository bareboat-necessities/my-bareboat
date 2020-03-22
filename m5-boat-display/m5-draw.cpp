#include <M5ez.h>

#include "m5-draw.h"

#define DEG2RAD 0.0174532925

void fillArc(int x, int y, int start_angle, int seg_count, int rx, int ry, int w, unsigned int color) {

  int seg = 5; // Angle a single segment subtends
  int inc = 4; // Draw segments every 6 degrees

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
}

float degToRad(float angleDeg) {
  return angleDeg * DEG2RAD;
}
