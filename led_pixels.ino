/*
 * This ESP32 code is created by esp32io.com
 *
 * This ESP32 code is released in the public domain
 *
 * For more detail (instruction and wiring diagram), visit https://esp32io.com/tutorials/esp32-ws2812b-led-strip
 */

#include <Adafruit_NeoPixel.h>


#define PIN_WS2812B 2
#define NUM_PIXELS 60
#define BRIGHTNESS 100
#define BAUD_RATE 9600

Adafruit_NeoPixel ws2812b(NUM_PIXELS, PIN_WS2812B, NEO_GRB + NEO_KHZ800);

void setup() {
  Serial.begin(BAUD_RATE);
  Serial.println("Starting");
  ws2812b.begin();
}

int red = 128;
int green = 0;
int blue = 0;

void loop() {
  if(Serial.available()) {
    byte msg[4];
    Serial.readBytesUntil('\r', msg, 4);
    ws2812b.clear();

    red = msg[0];
    green = msg[1];
    blue = msg[2];

    Serial.print("Received RGB: ");
    Serial.print(red);
    Serial.print(" ");
    Serial.print(green);
    Serial.print(" ");
    Serial.print(blue);
    Serial.println();
  }

  ws2812b.setBrightness(30);

  for(int i = 0; i < NUM_PIXELS / 2; i++) {
    ws2812b.setPixelColor(i, ws2812b.Color(red, green, blue));
  }

  ws2812b.show();
}
