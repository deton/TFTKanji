#include <SPI.h>
#include <SdFat.h>

#include <Adafruit_GFX.h>    // Core graphics library
#include "SWTFT.h" // Hardware-specific library
#include "Fontx2.h"

#define SD_SS_PIN 10
#define kanji_file "GONZN16X.TLF"
#define ascii_file "GONHN16X.TLF"

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
Fontx2 asciiFont;

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

int init_sd() {
  if (!sd.begin(SD_SS_PIN)) {
    sd.initErrorHalt();
    return -1;
  }
  return 0;
}

int initdone = 0;

void setup() {
  Serial.begin(115200);
  while (!Serial) { // これがないとIDEからの書き込みがエラーになる
    // wait for Leonardo
  }
  pinMode(SD_SS_PIN, OUTPUT);

  tft.reset();
  uint16_t identifier = tft.readID();
  tft.begin(identifier);
  tft.fillScreen(GREEN);
}

int init_font() {
  int ret = kanjiFont.open(&sd, kanji_file);
  Serial.print("kanjiFont.open()=");
  Serial.println(ret);
  if (ret != 0) {
    return ret;
  }

  ret = asciiFont.open(&sd, ascii_file);
  Serial.print("asciiFont.open()=");
  Serial.println(ret);
  if (ret != 0) {
    return ret;
  }
  return 0;
}

int init_sd_font() {
  //Serial.print("SPI_MOSI_PIN=");
  //Serial.println(SPI_MOSI_PIN);
  int ret = init_sd();
  Serial.print("init_sd()=");
  Serial.println(ret);
  if (ret != 0) {
    return ret;
  }

  ret = init_font();
  if (ret != 0) {
    return ret;
  }
  return 0;
}

bool iskanji1(int c) {
  return (c >= 0x81 && c <= 0x9f || c >= 0xe0 && c <= 0xff);
}

void loop() {
  static int x = 0;
  static int y = 0;
  static uint16_t kanji1 = 0;
  Fontx2 *font;
  uint16_t code;
  int ret;
  if (Serial.available()) {
    int ch = Serial.read();
    Serial.print(ch, HEX);
    if (kanji1) {
      code = (kanji1 << 8) | ch;
      kanji1 = 0;
      font = &kanjiFont;
    } else if (iskanji1(ch)) {
      kanji1 = ch;
      return;
    } else {
      code = ch;
      font = &asciiFont;
    }

    if (!initdone) {
      ret = init_sd_font();
      if (ret == 0) {
        initdone = 1;
      }
    }
    if (initdone) {
      tft.fillRect(x, y, font->width(), font->height(), BLACK);
      ret = font->draw(&tft, code, x, y, WHITE);
      x += font->width();
      if (x >= tft.width()) {
        x = 0;
        y += font->height();
        if (y >= tft.height()) {
          y = 0;
        }
      }

      if (ret < -1) { // ignore seek error
        ret = init_font(); // re-init
        if (ret != 0) {
          initdone = 0;
        }
      }
    }
  }
}
