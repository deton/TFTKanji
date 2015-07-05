#include <SPI.h>
#include <SdFat.h>

#include <Adafruit_GFX.h>    // Core graphics library
#include "SWTFT.h" // Hardware-specific library
#include "Fontx2.h"

#define SD_SS_PIN 10
#define kanji_file "GONZN16X.TLF"

// Pin numbers in templates must be constants.
const uint8_t SOFT_MISO_PIN = 12;
const uint8_t SOFT_MOSI_PIN = 11;
const uint8_t SOFT_SCK_PIN  = 13;
//
// Chip select may be constant or RAM variable.
const uint8_t SD_CHIP_SELECT_PIN = 10;

// SdFat software SPI template
SdFatSoftSpi<SOFT_MISO_PIN, SOFT_MOSI_PIN, SOFT_SCK_PIN> sd;
Fontx2 kanjiFont;

// Assign human-readable names to some common 16-bit color values:
#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

SWTFT tft;

int sd_init() {
  if (!sd.begin(SD_SS_PIN)) {
    sd.initErrorHalt();
    return -1;
  }
  return 0;
}

int initdone = 0;

void setup() {
  Serial.begin(115200);
  pinMode(SD_SS_PIN, OUTPUT);

  tft.reset();
  uint16_t identifier = tft.readID();
  tft.begin(identifier);
  tft.fillScreen(GREEN);
}

void loop() {
  static int x = 0;
  static int y = 0;
  int ret;
  if (Serial.available()) {
    char ch = Serial.read();
    if (!initdone) {
      //Serial.print("SPI_MOSI_PIN=");
      //Serial.println(SPI_MOSI_PIN);
      // setup FONTX2
      ret = sd_init();
      Serial.print("sd_init()=");
      Serial.println(ret);
      if (ret == 0) {
        ret = kanjiFont.open(&sd, kanji_file);
        Serial.print("kanjiFont.open()=");
        Serial.println(ret);
        if (ret == 0) {
          initdone = 1;
        }
      }
    }
    if (initdone) {
      tft.fillRect(x, y, kanjiFont.getXSize(), kanjiFont.getYSize(), BLACK);
      // to fullwidth
      if (isdigit(ch)) {
        ret = kanjiFont.draw(&tft, 0x821f + ch, x, y, WHITE);
      } else if (isupper(ch)) {
        ret = kanjiFont.draw(&tft, 0x821f + ch, x, y, WHITE);
      } else if (islower(ch)) {
        ret = kanjiFont.draw(&tft, 0x8220 + ch, x, y, WHITE);
      } else {
        ret = kanjiFont.draw(&tft, 0x938c, x, y, WHITE);
      }
      x += kanjiFont.getXSize();
      if (x >= tft.width()) {
        x = 0;
        y += kanjiFont.getYSize();
        if (y >= tft.height()) {
          y = 0;
        }
      }
      if (ret < -1) { // ignore seek error
        kanjiFont.close();
        ret = kanjiFont.open(&sd, kanji_file); // re-init
        Serial.print("kanjiFont.open()=");
        Serial.println(ret);
        if (ret != 0) {
          initdone = 0;
        }
      }
    }
  }
}
