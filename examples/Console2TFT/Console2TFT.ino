#include <SPI.h>
#include <SdFat.h>

// accept command from MPU for Arduino Yun/Linino ONE
#define CMD_FROM_MPU 1
#if CMD_FROM_MPU  // accept command from MPU for Arduino Yun/Linino ONE
#include <Console.h> // includeするとスケッチサイズが増える
#define Serial Console
#endif

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

#if USE_ITKSCREEN
class Screen: public ITKScreen {
  public:
    virtual void drawPixel(int16_t x, int16_t y, uint16_t color) {
      tft.drawPixel(x, y, color);
    }
    virtual void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
      tft.fillRect(x, y, w, h, color);
    }
    virtual int16_t width() {
      return tft.width();
    }
    virtual int16_t height() {
      return tft.height();
    }
} screen;
TFTKanji tftkanji(&screen);
#else
TFTKanji tftkanji(&tft);
#endif

int init_sd() {
  if (!sd.begin(SD_CHIP_SELECT_PIN)) {
    sd.initErrorHalt();
    return -1;
  }
  return 0;
}

int initdone = 0;

void setup() {
#if CMD_FROM_MPU
  Bridge.begin(115200);
  Console.begin();
#else
  Serial.begin(115200);
  while (!Serial) { // これがないとIDEからの書き込みがエラーになる
    // wait for Leonardo
  }
#endif
  pinMode(SD_CHIP_SELECT_PIN, OUTPUT);

  tft.reset();
  uint16_t identifier = tft.readID();
  tft.begin(identifier);
  tft.fillScreen(GREEN);
}

int init_font() {
  int ret = tftkanji.open(&sd, kanji_file, ank_file);
#if DBGLOG
  Serial.print(F("tftkanji.open()="));
  Serial.println(ret);
#endif
  return ret;
}

int init_sd_font() {
  int ret = init_sd();
#if DBGLOG
  Serial.print(F("init_sd()="));
  Serial.println(ret);
#endif
  if (ret != 0) {
    return ret;
  }

  return init_font();
}

int16_t x = 0;
int16_t y = 0;
uint16_t color = BLACK;
uint16_t bgcolor = WHITE;
bool wrap = false;

void parse(const char* buf) {
  const char* p = buf + 1;
  int n;
  uint8_t cmd = *buf;
  switch (cmd) { // command char
  case 'J':
    tft.fillScreen(bgcolor);
    break;
  case 'K':
    tft.fillRect(x, y, tft.width() - x, tftkanji.height(), bgcolor);
    break;
  case 'T':
    n = tftkanji.drawText(&x, &y, p, color, bgcolor, wrap);
    if (n <= 0) {
#if DBGLOG
      //Serial.print(F("NG drawText():"));
      Serial.print(F("NGd"));
#endif
      Serial.println(n);
    }
    break;
  case 'C': // Col
    // 左上位置はrow=1,col=1→x=0,y=0
    x = (atoi(p)-1) * tftkanji.ankWidth();
    break;
  case 'R': // Row
    y = (atoi(p)-1) * tftkanji.height();
    break;
  case 'c': // color
    // XXX: スケッチサイズを減らすためatoi()
    color = atoi(p);
    break;
  case 'b': // bgcolor
    bgcolor = atoi(p);
    break;
  case 'W': // wrap
    wrap = (*p != '0');
    break;
  default:
#if DBGLOG
    Serial.print(F("NG cmd:"));
    Serial.println(cmd);
#endif
    break;
  }
}

void loop() {
  char buf[41]; // 320[dot screen]/8[dot font] + 1
  if (Serial.available()) {
    int n = Serial.readBytesUntil('\n', buf, sizeof buf);
#if DBGLOG
    Serial.println(buf);
#endif
    buf[n] = '\0';
    if (!initdone) {
      if (init_sd_font() == 0) {
        initdone = 1;
      }
    }
    if (initdone) {
      parse(buf);
    }
    Serial.println();
    Serial.print(F("> "));
  }
}
