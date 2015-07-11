#include "TFTKanji.h"

#define DBGLOG 0

TFTKanji::TFTKanji() {
}

TFTKanji::~TFTKanji() {
  close();
}

int TFTKanji::open(SdFatBase* sd, const char* kanjifile, const char* ankfile) {
  int ret = kanjiFont.open(sd, kanjifile);
#if DBGLOG
  Serial.print("kanjiFont.open()=");
  Serial.println(ret);
#endif
  if (ret != 0) {
    return ret;
  }

  ret = ankFont.open(sd, ankfile);
#if DBGLOG
  Serial.print("ankFont.open()=");
  Serial.println(ret);
#endif
  if (ret != 0) {
    return ret;
  }
  return 0;
}

bool TFTKanji::close() {
  ankFont.close();
  kanjiFont.close();
}

int TFTKanji::draw(Adafruit_GFX *tft, const char* str, int16_t x, int16_t y, uint16_t color) {
  return draw(tft, str, x, y, color, 0, false);
}

int TFTKanji::draw(Adafruit_GFX *tft, const char* str, int16_t x, int16_t y, uint16_t color, uint16_t bgcolor) {
  return draw(tft, str, x, y, color, bgcolor, true);
}

int TFTKanji::draw(Adafruit_GFX *tft, const char* str, int16_t startx, int16_t y, uint16_t color, uint16_t bgcolor, bool drawbg) {
  int x = startx;
  uint16_t kanji1 = 0;
  uint16_t code;
  const char* p = str;
  for (; *p != '\0'; p++) {
    Fontx2 *font;
    uint8_t ch = (uint8_t)*p;
#if DBGLOG
    Serial.print(ch, HEX);
#endif
    if (kanji1) {
      code = (kanji1 << 8) | ch;
      kanji1 = 0;
      font = &kanjiFont;
    } else if (iskanji1(ch)) {
      kanji1 = ch;
      continue;
    } else {
      code = ch;
      font = &ankFont;
    }
    // TODO: '\n'があったら次の行

    // XXX: 画面をはみ出るかチェックして、はみ出る場合は描画しない?
    if (drawbg) {
      tft->fillRect(x, y, font->width(), font->height(), bgcolor);
    }
    int ret = font->draw(tft, code, x, y, color);
    x += font->width();
    if (x >= tft->width()) {
      break;
    }

    if (ret < -1) {
      return ret;
    }
  }
  return p - str;
}
