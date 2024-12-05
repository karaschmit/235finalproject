#include <Adafruit_NeoPixel.h>

int neoPin = 6; 
int neoAmt = 60;
int trigPin = 9;
int echoPin = 8; 
int threshold = 10; 

int state = 0;

Adafruit_NeoPixel strip(neoAmt, neoPin);

uint32_t softYellow = strip.Color(255, 155, 30);
uint32_t pink = strip.Color(255, 20, 147);
uint32_t blue = strip.Color(135, 206, 235);

void setup() {
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  strip.begin();
  strip.show();
  Serial.begin(9600);
}

void loop() {
  int distance = getDistance();

  if (distance > 0 && distance <= threshold) {
    delay(300);
    distance = getDistance();
    if (distance > 0 && distance <= threshold) {
      state = (state + 1) % 5;
      while (getDistance() <= threshold) {
        delay(50);
      }
    }
  }
  runCurrentState();
}

int getDistance() {
  digitalWrite(trigPin, LOW);
  delay(20);
  digitalWrite(trigPin, HIGH);
  delay(20);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH);
  int distance = duration * 0.034 / 2;
  if (distance > 0 && distance <= 400) {
    return distance;
  } else {
    return -1;
  }
}

void runCurrentState() {
  switch (state) {
    case 0:
      turnOffStrip();
      break;
    case 1:
      rainbow(10);
      break;
    case 2:
      twinkleEffect(softYellow, 100);
      break;
    case 3:
      twoColorSweep(pink, blue, 500);
      break;
    case 4:
      rainbowChase();
      break;
  }
}

void turnOffStrip() {
  for (int i = 0; i < neoAmt; i++) {
    strip.setPixelColor(i, 0);
  }
  strip.show();
}

void rainbow(int wait) {
  static long firstPixelHue = 0;
  static unsigned long lastUpdate = 0;
  if (millis() - lastUpdate >= wait) {
    lastUpdate = millis();
    firstPixelHue += 256;
    
    if (firstPixelHue >= 5 * 65536) {
      firstPixelHue = 0;
    }

    strip.rainbow(firstPixelHue);
    strip.show();
  }
}

void twinkleEffect(uint32_t color, int interval) {
  static unsigned long lastChangeTime = 0;

  if (millis() - lastChangeTime >= interval) {
    lastChangeTime = millis();

    for (int i = 0; i < strip.numPixels(); i++) {
      if (random(10) < 3) { 
        strip.setPixelColor(i, color);
      } else {
        strip.setPixelColor(i, 0); 
      }
    }
    strip.show();
  }
}

void twoColorSweep(uint32_t c1, uint32_t c2, int interval) {
  static int state = 0;
  static unsigned long lastState = 0;

  if (millis() - lastState >= interval) {
    lastState = millis();
    state = (state + 1) % 2;  // Cycle between two states
  }

  strip.clear();
  for (int i = 0; i < strip.numPixels(); i++) {
    switch (state) {
      case 0:
        if (i % 2 == 0) {
          strip.setPixelColor(i, c1);
        } else {
          strip.setPixelColor(i, c2);
        }
        break;

      case 1:
        if (i % 2 == 0) {
          strip.setPixelColor(i, c2);
        } else {
          strip.setPixelColor(i, c1);
        }
        break;
    }
  }
  strip.show();
}

void rainbowChase() {
  static int chaseOffset = 0;
  static unsigned long lastUpdate = 0;

  if (millis() - lastUpdate >= 20) {
    lastUpdate = millis();
    for (int i = 0; i < neoAmt; i++) {
      strip.setPixelColor(i, hue((i + chaseOffset) & 255));
    }
    strip.show();
    chaseOffset = (chaseOffset + 1) % 256;
  }
}

uint32_t hue(byte hue) {
  hue = hue % 255; 

  switch (hue / 85) {
    case 0:
      return strip.Color(hue * 3, 255 - hue * 3, 0);
    case 1:
      hue -= 85;
      return strip.Color(255 - hue * 3, 0, hue * 3);
    case 2:
      hue -= 170;
      return strip.Color(0, hue * 3, 255 - hue * 3);
  }
  return 0;
}