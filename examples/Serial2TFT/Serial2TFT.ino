#include <SPI.h>
#include <SdFat.h>

#include <Adafruit_GFX.h>    // Core graphics library
#include "SWTFT.h" // Hardware-specific library
#include "TFTKanji.h"

// 小伝馬町16
#define kanji_file "GONZN16X.TLF"
#define ank_file   "GONHN16X.TLF"

// Pin numbers in templates must be constants.
const uint8_t SOFT_MISO_PIN = 12;
const uint8_t SOFT_MOSI_PIN = 11;
const uint8_t SOFT_SCK_PIN  = 13;
//
// Chip select may be constant or RAM variable.
const uint8_t SD_CHIP_SELECT_PIN = 10;

// SdFat software SPI template
SdFatSoftSpi<SOFT_MISO_PIN, SOFT_MOSI_PIN, SOFT_SCK_PIN> sd;

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
TFTKanji tftkanji;

int init_sd() {
  if (!sd.begin(SD_CHIP_SELECT_PIN)) {
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
  pinMode(SD_CHIP_SELECT_PIN, OUTPUT);

  tft.reset();
  uint16_t identifier = tft.readID();
  tft.begin(identifier);
  tft.fillScreen(GREEN);
}

int init_font() {
  int ret = tftkanji.open(&sd, kanji_file, ank_file);
  Serial.print("tftkanji.open()=");
  Serial.println(ret);
  return ret;
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

  return init_font();
}

void loop() {
  static int y = 0;
  char buf[80];
  memset(buf, 0, sizeof buf);
  if (Serial.available()) {
    int n = Serial.readBytesUntil('\n', buf, sizeof buf);
    Serial.println(buf);
    if (n <= 0) {
      return;
    }

    if (!initdone) {
      if (init_sd_font() == 0) {
        initdone = 1;
      }
    }
    if (initdone) {
      int ret = tftkanji.draw(&tft, buf, 0, y, WHITE, BLACK);
      y += tftkanji.height();
      if (y >= tft.height()) {
        y = 0;
      }

      if (ret < -1) { // ignore no kanji data error(-1)
        ret = init_font(); // re-init
        if (ret != 0) {
          initdone = 0;
        }
      }
    }
  }
}
