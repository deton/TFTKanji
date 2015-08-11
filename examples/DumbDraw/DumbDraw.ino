// DumbDraw.ino: TFT LCD以外に描画するサンプル。
// Shift_JISをSerialから読んで、以下のようにSerialに出力。
//   #     #  #            #
//    # ###########        #
//    #    #  #      #############
//  #                #           #
//   #    #######    #           #
//   #    #  #  #      ########
//     #  #  #  #            #
//     #  #######          ##
//     #     #             #
//    #  #########  ###############
//    #      #             #
//    # ###########        #
//   #      # #            #
//   #     #   #           #
//  #     #     #          #
//  #   ##       ##       ##
#include <SPI.h>
#include <SdFat.h>

#include "TFTKanji.h"

// Pin numbers in templates must be constants.
const uint8_t SOFT_MISO_PIN = 12;
const uint8_t SOFT_MOSI_PIN = 11;
const uint8_t SOFT_SCK_PIN  = 13;
//
// Chip select may be constant or RAM variable.
const uint8_t SD_CHIP_SELECT_PIN = 10;

// SdFat software SPI template
SdFatSoftSpi<SOFT_MISO_PIN, SOFT_MOSI_PIN, SOFT_SCK_PIN> sd;

// 小伝馬町16
#define KANJIFILE "GONZN16X.TLF"
#define ANKFILE   "GONHN16X.TLF"

// dumb screen. 3x1 chars in 16x16 dot font
const int SCREEN_WIDTH = 48;
const int SCREEN_HEIGHT = 16;
char screenbuf[SCREEN_HEIGHT][SCREEN_WIDTH];

const uint16_t COLOR = 0;
const uint16_t BGCOLOR = 0xffff;

#if !USE_ITKSCREEN
# error DumbDraw demo requires #define USE_ITKSCREEN 1
#endif
class Screen: public ITKScreen {
  public:
    virtual void drawPixel(int16_t x, int16_t y, uint16_t color) {
      if (x < 0 || x >= width() || y < 0 || y >= height()) {
        return;
      }
      int ch = '_';
      if (color == COLOR) {
        ch = '#';
      } else if (color == BGCOLOR) {
        ch = ' ';
      } // TODO: support other color
      screenbuf[y][x] = ch;
    }
    virtual void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
      for (int j = y; j < y + h; j++) {
        for (int i = x; i < x + w; i++) {
          drawPixel(i, j, color);
        }
      }
    }
    virtual int16_t width() {
      return SCREEN_WIDTH;
    }
    virtual int16_t height() {
      return SCREEN_HEIGHT;
    }
    void fillScreen(uint16_t color) {
      fillRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, color); // clear screenbuf
    }
    void begin() {
      fillScreen(BGCOLOR);
    }
    void dump() {
      for (int y = 0; y < SCREEN_HEIGHT; y++) {
        for (int x = 0; x < SCREEN_WIDTH; x++) {
          Serial.print(screenbuf[y][x]);
        }
        Serial.println();
      }
    }
} screen;
TFTKanji tftkanji(&screen);

int init_sd() {
  if (!sd.begin(SD_CHIP_SELECT_PIN)) {
    sd.initErrorHalt();
    return -1;
  }
  return 0;
}

int init_font() {
  int ret = tftkanji.open(&sd, KANJIFILE, ANKFILE);
  Serial.print("tftkani.open()=");
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

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    // wait for Leonardo
  }
  pinMode(SD_CHIP_SELECT_PIN, OUTPUT);
  screen.begin();
}

int initdone = 0;
int16_t x = 0;
int16_t y = 0;
bool sjis1 = false;
char buf[3];

void loop() {
  if (Serial.available()) {
    int ch = Serial.read();
    if (ch < 0) {
      return;
    }
    if (!initdone) {
      if (init_sd_font() == 0) {
        initdone = 1;
      }
    }
    if (initdone) {
      //tftkanji.drawText(&x, &y, "\x83\x5f\x83\x80\x92\x5b", COLOR, BGCOLOR);
      if (sjis1) {
        sjis1 = false;
        buf[1] = ch;
        buf[2] = '\0';
      } else if (TFTKanji::issjis1(ch)) {
        sjis1 = true;
        buf[0] = ch;
        return;
      } else {
        buf[0] = ch;
        buf[1] = '\0';
      }
      // 描こうとしている文字がscreen右端を越える?
      if (x + tftkanji.ankWidth() * strlen(buf) > screen.width()) {
        y += tftkanji.height(); // 次行に描画
        x = 0;
        if (y >= screen.height()) { // screen下端を越える?
          screen.fillScreen(BGCOLOR);
          x = 0;
          y = 0;
        }
      }
      int n = tftkanji.drawText(&x, &y, buf, COLOR, BGCOLOR
#if WRAP_LONGLINE
      , 0
#endif
      );
      if (n <= 0) {
        Serial.print("NG drawText():");
        Serial.println(n);
      }
      screen.dump();
    }
  }
}
