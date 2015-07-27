#include "TFTKanji.h"

#define DBGLOG 0

TFTKanji::TFTKanji(ITKScreen* tft) :tft(tft) {
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

// cf. Adafruit_GFX::drawBitmap()
void drawBitmap(ITKScreen* tft, int16_t x, int16_t y,
    const uint8_t *bitmap, int16_t w, int16_t h,
    uint16_t color) {

  int16_t i, j, byteWidth = (w + 7) / 8;

  for (j=0; j<h; j++, bitmap += byteWidth) {
    for (i=0; i<w; i++) {
      if (*(bitmap + i / 8) & (128 >> (i & 7))) {
        tft->drawPixel(x + i, y + j, color);
      }
    }
  }
}

int loadFontAndDraw(ITKScreen* tft, int16_t x, int16_t y,
    Fontx2* font, uint16_t code, uint16_t color, uint16_t bgcolor) {
  int len = font->bitmapLen();
  uint8_t buf[len];
  int ret = font->load(code, buf, len);
  if (ret == 0) {
    // bgcolorがcolorと同じ場合はbgcolorでのfillは行わない。フラグ不要にするため
    // cf. Adafruit_GFX::setTextColor()
    if (bgcolor != color) {
      tft->fillRect(x, y, font->width(), font->height(), bgcolor);
    }
    drawBitmap(tft, x, y, buf, font->width(), font->height(), color);
  }
  return ret;
}

int TFTKanji::drawText(int16_t* x, int16_t* y, const char* str, uint16_t color, uint16_t bgcolor
#if WRAP_LONGLINE
    , bool wrap
#endif
    ) {
  uint16_t sjis1 = 0;
  const char* p = str;
  for (; *p != '\0'; p++) {
    uint8_t ch = (uint8_t)*p;
    uint16_t code;
    Fontx2* font;
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
#if WRAP_NEWLINE
      if (ch == '\n') {
        *y += height();
        *x = 0;
        if (*y >= tft->height()) {
          break;
        } else {
          continue;
        }
      } else if (ch == '\r') { // ignore
        continue;
      }
#endif
      code = ch;
      font = &ankFont;
    }

    int16_t tftWidth = tft->width();
#if WRAP_LONGLINE
    if (wrap && *x + font->width() > tftWidth) {
      *y += height();
      *x = 0;
      if (*y >= tft->height()) {
        break;
      }
    }
#endif
    int ret = loadFontAndDraw(tft, *x, *y, font, code, color, bgcolor);
    if (ret < -1) {
      return ret;
    } // -1の場合、指定した文字のフォントデータ無し
    *x += font->width();
    if (*x >= tftWidth) {
      break;
    }
  }
  return p - str;
}
