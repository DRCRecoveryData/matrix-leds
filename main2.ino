#include <Adafruit_NeoPixel.h>
#include "MIDIUSB.h"

const byte NUM_LEDS = 16;
const byte LED_PIN = 2;
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

const byte _R[128] = {0, 61, 125, 190, 255, 255, 61, 125, 190, 255, 255, 61, 125, 190, 255, 255, 61, 125, 190, 255, 255, 61, 125, 190, 255, 255, 45, 93, 142, 223, 190, 28, 61, 93, 190, 125, 12, 28, 45, 158, 61, 0, 0, 0, 125, 0, 0, 0, 0, 125, 0, 0, 0, 0, 125, 0, 0, 0, 0, 125, 0, 0, 0, 0, 125, 0, 0, 0, 0, 125, 0, 0, 0, 0, 125, 0, 0, 0, 0, 125, 0, 0, 0, 0, 125, 0, 12, 28, 45, 158, 61, 28, 61, 93, 190, 125, 45, 93, 142, 223, 190, 61, 125, 190, 255, 255, 61, 125, 190, 255, 255, 61, 125, 190, 255, 255, 61, 125, 190, 255, 255, 36, 73, 109, 146, 182, 219, 255};
const byte _G[128] = {0, 0, 0, 0, 125, 0, 12, 28, 45, 158, 61, 28, 61, 93, 190, 125, 45, 93, 142, 223, 190, 61, 125, 190, 255, 255, 61, 125, 190, 255, 255, 61, 125, 190, 255, 255, 61, 125, 190, 255, 255, 61, 125, 190, 255, 255, 61, 125, 190, 255, 255, 61, 125, 190, 255, 255, 61, 125, 190, 255, 255, 61, 125, 190, 255, 255, 45, 93, 142, 223, 190, 28, 61, 93, 190, 125, 12, 28, 45, 158, 61, 0, 0, 0, 125, 0, 0, 0, 0, 125, 0, 0, 0, 0, 125, 0, 0, 0, 0, 125, 0, 0, 0, 0, 125, 0, 0, 0, 0, 125, 0, 0, 0, 0, 125, 0, 0, 0, 0, 125, 0, 36, 73, 109, 146, 182, 219, 255};
const byte _B[128] = {0, 0, 0, 0, 125, 0, 0, 0, 0, 125, 0, 0, 0, 0, 125, 0, 0, 0, 0, 125, 0, 0, 0, 0, 125, 0, 0, 0, 0, 125, 0, 0, 0, 0, 125, 0, 0, 0, 0, 125, 0, 0, 0, 0, 125, 0, 12, 28, 45, 158, 61, 28, 61, 93, 190, 125, 45, 93, 142, 223, 190, 61, 125, 190, 255, 255, 61, 125, 190, 255, 255, 61, 125, 190, 255, 255, 61, 125, 190, 255, 255, 61, 125, 190, 255, 255, 61, 125, 190, 255, 255, 61, 125, 190, 255, 255, 61, 125, 190, 255, 255, 61, 125, 190, 255, 255, 45, 93, 142, 223, 190, 28, 61, 93, 190, 125, 12, 28, 45, 158, 61, 36, 73, 109, 146, 182, 219, 255};

const byte ROWS = 4;
const byte COLS = 4;
const byte rowPins[ROWS] = {9, 8, 7, 6}; // Pins for the rows (from top to bottom)
const byte colPins[COLS] = {5, 4, 3, 2}; // Pins for the columns (from left to right)

const byte MATRIX[ROWS][COLS] = {
  {36, 37, 38, 39},
  {40, 41, 42, 43},
  {44, 45, 46, 47},
  {48, 49, 50, 51}
};

void setup() {
  strip.begin();
  strip.show();
  
  // Set up matrix button pins as INPUT_PULLUP
  for (byte i = 0; i < ROWS; i++) {
    pinMode(rowPins[i], INPUT_PULLUP);
  }
  for (byte i = 0; i < COLS; i++) {
    pinMode(colPins[i], OUTPUT);
    digitalWrite(colPins[i], HIGH);
  }
}

void note(byte pitch, byte velocity) {
  if (velocity > 0) {
    for (byte row = 0; row < ROWS; row++) {
      for (byte col = 0; col < COLS; col++) {
        if (MATRIX[row][col] == pitch) {
          int colorIndex = map(velocity, 1, 127, 0, 127);
          strip.setPixelColor(row * COLS + col, R[colorIndex], G[colorIndex], B[colorIndex]);
          strip.show();
        }
      }
    }
  } else {
    for (byte row = 0; row < ROWS; row++) {
      for (byte col = 0; col < COLS; col++) {
        if (MATRIX[row][col] == pitch) {
          strip.setPixelColor(row * COLS + col, 0, 0, 0);
          strip.show();
        }
      }
    }
  }
}

void loop() {
  // Read the state of matrix buttons
  for (byte col = 0; col < COLS; col++) {
    digitalWrite(colPins[col], LOW);
    for (byte row = 0; row < ROWS; row++) {
      if (digitalRead(rowPins[row]) == LOW) {
        note(MATRIX[row][col], 127); // Note-on event with maximum velocity (127)
        delay(50); // Debounce delay
      } else {
        note(MATRIX[row][col], 0); // Note-off event (velocity 0)
      }
    }
    digitalWrite(colPins[col], HIGH);
  }
  
  // Handle other MIDI events if needed
  midiEventPacket_t rx;
  do {
    rx = MidiUSB.read();
    if (rx.header == 0x9) {
      note(rx.byte2, rx.byte3);
    } else if (rx.header == 0x8) {
      note(rx.byte2, 0);
    }
  } while (rx.header == 0);
}
