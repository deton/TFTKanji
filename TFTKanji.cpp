#include "TFTKanji.h"

#define DBGLOG 0

TFTKanji::TFTKanji(Adafruit_GFX *tft): tft(tft) {
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

int TFTKanji::drawText(int16_t* x, int16_t* y, const char* str, uint16_t color) {
  // bgcolorがcolorと同じ場合はbgcolorでのfillは行わない。フラグ不要にするため
  // cf. Adafruit_GFX::setTextColor()
  return drawText(x, y, str, color, color);
}

int TFTKanji::drawText(int16_t* x, int16_t* y, const char* str, uint16_t color, uint16_t bgcolor) {
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
    // TODO: 長い行のwrap

    // XXX: 画面をはみ出るかチェックして、はみ出る場合は描画しない?
    if (color != bgcolor) {
      tft->fillRect(*x, *y, font->width(), font->height(), bgcolor);
    }
    int ret = font->draw(tft, x, y, code, color);
    if (*x >= tft->width()) {
      break;
    }

    if (ret < -1) {
      return ret;
    }
  }
  return p - str;
}
