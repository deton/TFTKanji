#include "TFTKanji.h"

#define DBGLOG 0

TFTKanji::TFTKanji(Adafruit_GFX *tft) :tft(tft) {
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
  return kanjiFont.close();
}

int TFTKanji::drawText(int16_t* x, int16_t* y, const char* str, uint16_t color) {
  // bgcolorがcolorと同じ場合はbgcolorでのfillは行わない。フラグ不要にするため
  // cf. Adafruit_GFX::setTextColor()
  return drawText(x, y, str, color, color);
}

int TFTKanji::drawText(int16_t* x, int16_t* y, const char* str, uint16_t color, uint16_t bgcolor) {
  uint16_t sjis1 = 0;
  uint16_t code;
  const char* p = str;
  for (; *p != '\0'; p++) {
    Fontx2 *font;
    uint8_t ch = (uint8_t)*p;
#if DBGLOG
    Serial.print(ch, HEX);
#endif
    if (sjis1) { // SJIS 2nd byte
      code = (sjis1 << 8) | ch;
      sjis1 = 0;
      font = &kanjiFont;
    } else if (issjis1(ch)) { // SJIS 1st byte
      sjis1 = ch;
      continue;
    } else {
      code = ch;
      font = &ankFont;

      if (code == '\n') {
        *y += height();
        *x = 0;
        if (*y >= tft->height()) {
          break;
        } else {
          continue;
        }
      } else if (code == '\r') { // ignore
        continue;
      }
    }

    // XXX: 画面をはみ出るかチェックして、はみ出る場合は描画しない?
    if (color != bgcolor) {
      tft->fillRect(*x, *y, font->width(), font->height(), bgcolor);
    }
    int ret = font->draw(tft, x, y, code, color);
    // TODO: 長い行のwrap
    if (*x >= tft->width()) {
      break;
    }

    if (ret < -1) {
      return ret;
    }
  }
  return p - str;
}
