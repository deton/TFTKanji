#include <SPI.h>
#include <SdFat.h>

#include <Adafruit_GFX.h>    // Core graphics library
#include "SWTFT.h" // Hardware-specific library
#include "TFTKanji.h"

struct {
  const char* kanjifile;
  const char* ankfile;
  const char* message;
} fonts[] = {
  // XXX: messageはShift_JISエンコード
  // ILゴシック
  // http://ayati.cocolog-nifty.com/blog/2012/08/ipalx322416-64a.html
  {"ILGZ32XB.FNT", "ILGH32XB.FNT", "IL\x83\x53\x83\x56\x83\x62\x83\x4e(32x32)"},
  // jiskan24
  // http://openlab.ring.gr.jp/efont/japanese/
  {"jiskan24.fnt", "12x24rk.fnt", "jiskan24(24x24)"},
  // Kappa 20dot Font
  // http://www.vector.co.jp/soft/data/writing/se083507.html
  {"k20m.fnt", "10x20rkm.fnt", "Kappa20(20x20)"},
  // Ayu 18ドットゴシック
  // http://x-tt.osdn.jp/ayu.html
  {"k18gm.fnt", "9x18gm.fnt", "Ayu 18\x83\x68\x83\x62\x83\x67\x83\x53\x83\x56\x83\x62\x83\x4e(18x18)"},
  // ぱう
  // http://udumaki.s36.xrea.com/
  {"PAW16K.FNT", "PAW16A.FNT", "\x82\xcf\x82\xa4(16x16)"},
  // 小伝馬町16
  // http://homepage3.nifty.com/silo/FONTV/
  {"GONZN16X.TLF", "GONHN16X.TLF", "\x8f\xac\x93\x60\x94\x6e\x92\xac\x31\x36(16x16)"},
  // http://www.vector.co.jp/soft/data/writing/se010319.html
  {"K14.FNT", "7X14RK.FNT", "ak14(14x14)"},
  // 赤城
  // http://hp.vector.co.jp/authors/VA007690/
  {"Akagi11k.fnt", "Akagi11a.fnt", "\x90\xd4\x8f\xe9(16x11)"},
  // 要町
  // http://www.vector.co.jp/soft/data/writing/se002908.html
  {"KNMZN12X.MNF", "KNMHN12X.MNF", "\x97\x76\x92\xac(12x12)"},
  // http://z.apps.atjp.jp/k12x10/
  {"k12x10.fnt", "k6x10.fnt", "k12x10(12x10)"},
  // http://www.vector.co.jp/soft/data/writing/se026704.html
  {"GENZN08X.FNT", "GENHN08X.FNT", "genfont(16x8)"},
  // ナガ10
  // http://hp.vector.co.jp/authors/VA013391/fonts/
  {"knj10.fnt", "5x10rk.fnt", "\x83\x69\x83\x4b\x31\x30(10x10)"},
  // http://www.geocities.jp/littlimi/k8x12.htm
  {"k8x12.fnt", "k4x12.fnt", "k8x12(8x12)"},
  // 美咲
  // http://www.geocities.jp/littlimi/misaki.htm
  {"MISAKI.FNT", "4X8.FNT", "\x94\xfc\x8d\xe7(8x8)"},
  {NULL, NULL, NULL},
};

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

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    // wait for Leonardo
  }
  pinMode(SD_CHIP_SELECT_PIN, OUTPUT);

  tft.reset();
  uint16_t identifier = tft.readID();
  tft.begin(identifier);
  tft.fillScreen(BLUE);

  int ret = init_sd();
  Serial.print("init_sd()=");
  Serial.println(ret);
  if (ret != 0) {
    Serial.println("init_sd failed. halt");
    tft.fillScreen(RED);
    //tft.setTextColor(BLACK);
    //tft.println("init_sd failed. halt");
    while (true) {
    }
  }
  tft.fillScreen(GREEN);
}

void drawTexts() {
  int16_t x = 0;
  int16_t y = 0;
  for (int i = 0; fonts[i].kanjifile != NULL; i++) {
    int ret = tftkanji.open(&sd, fonts[i].kanjifile, fonts[i].ankfile);
    if (ret != 0) {
      Serial.print("NG tftkanji.open():");
      Serial.println(ret);
      continue;
    }
    tftkanji.drawText(&x, &y, fonts[i].message, BLACK, WHITE);
    // XXX: Shift_JISエンコード
    // "Vimは最もたくさんのコンピューター/OSで利用できるテキストエディタです。"
    tftkanji.drawText(&x, &y, ":Vim\x82\xcd\x8d\xc5\x82\xe0\x82\xbd\x82\xad\x82\xb3\x82\xf1\x82\xcc\x83\x52\x83\x93\x83\x73\x83\x85\x81\x5b\x83\x5e\x81\x5b/OS\x82\xc5\x97\x98\x97\x70\x82\xc5\x82\xab\x82\xe9\x83\x65\x83\x4c\x83\x58\x83\x67\x83\x47\x83\x66\x83\x42\x83\x5e\x82\xc5\x82\xb7\x81\x42", BLACK, WHITE);
    y += tftkanji.height() + 1; // +1: 小サイズフォントはくっついて読みにくい
    x = 0;
  }
}

void loop() {
  for (int rotation = 0; rotation < 4; rotation++) {
    tft.setRotation(rotation);
    tft.fillScreen(CYAN);
    drawTexts();
    delay(10000);
  }
}
