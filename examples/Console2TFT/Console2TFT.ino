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
  Serial.print("tftkanji.open()=");
  Serial.println(ret);
  return ret;
}

int init_sd_font() {
  int ret = init_sd();
  Serial.print("init_sd()=");
  Serial.println(ret);
  if (ret != 0) {
    return ret;
  }

  return init_font();
}

enum State {ST_WAITCMD, ST_SETX, ST_SETY, ST_TEXT, ST_SJIS1};
State state = ST_WAITCMD;
int16_t x = 0;
int16_t y = 0;
const uint16_t color = BLACK;
const uint16_t bgcolor = WHITE;
int row;
int col;
char buf[3];

void loop() {
  if (Serial.available()) {
    int ch = Serial.read();
    if (ch < 0) {
      return;
    }
    Serial.print(ch, HEX);
    if (!initdone) {
      if (init_sd_font() == 0) {
        initdone = 1;
      }
    }
    if (initdone) {
      if (state == ST_SJIS1) {
        state = ST_TEXT;
        buf[1] = ch;
        buf[2] = '\0';
      } else if (state == ST_SETX) {
        if (isdigit(ch)) {
          col = col * 10 + ch - '0';
        } else {
          state = ST_WAITCMD;
          if (col < 1) {
            col = 1;
          }
          // 左上位置はrow=1,col=1→x=0,y=0
          x = (col-1) * tftkanji.ankWidth();
        }
        return;
      } else if (state == ST_SETY) {
        if (isdigit(ch)) {
          row = row * 10 + ch - '0';
        } else {
          state = ST_WAITCMD;
          if (row < 1) {
            row = 1;
          }
          y = (row-1) * tftkanji.height();
        }
        return;
      } else if (state == ST_WAITCMD) {
        switch (ch) {
        case 'J':
          tft.fillScreen(bgcolor);
          return;
        case 'X':
          state = ST_SETX;
          col = 0;
          break;
        case 'Y':
          state = ST_SETY;
          row = 0;
          break;
        case 'K':
          tft.fillRect(x, y, tft.width() - x, tftkanji.height(), bgcolor);
          return;
        case '\n':
          state = ST_WAITCMD;
          return;
        default:
          state = ST_TEXT;
          break;
        }
      }
      if (TFTKanji::issjis1(ch)) {
        state = ST_SJIS1;
        buf[0] = ch;
        return;
      } else {
        buf[0] = ch;
        buf[1] = '\0';
      }
      // scrollというかpage down
      if (y >= tft.height()) {
        tft.fillScreen(bgcolor);
        x = 0;
        y = 0;
      }
      int ret = tftkanji.drawText(&x, &y, buf, color, bgcolor);
      if (ret <= 0) {
        Serial.print("NG drawText():");
        Serial.println(ret);
      }
    }
  }
}