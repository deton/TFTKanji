#include "TFTKanjiTerm.h"

#define DBGLOG 0

const uint16_t WHITE = 0xFFFF;
const uint16_t BLACK = 0x0000;

TFTKanjiTerm::TFTKanjiTerm(Adafruit_GFX *tft)
  :tft(tft), tftkanji(tft), state(ST_PRINTABLE), x(0), y(0),
   color(BLACK), bgcolor(WHITE) {
  sjisbuf[0] = '\0';
}

TFTKanjiTerm::~TFTKanjiTerm() {
  tftkanji.close();
}

int TFTKanjiTerm::open(SdFatBase* sd, const char* kanjifile, const char* ankfile) {
  return tftkanji.open(sd, kanjifile, ankfile);
}

bool TFTKanjiTerm::close() {
  return tftkanji.close();
}

int TFTKanjiTerm::addch(int ch) {
  if (sjisbuf[0] != '\0') { // SJIS 2nd byte
    sjisbuf[1] = ch;
    sjisbuf[2] = '\0';
  } else if (TFTKanji::issjis1(ch)) { // SJIS 1st byte
    sjisbuf[0] = ch;
    return 1;
  } else {
    sjisbuf[0] = ch;
    sjisbuf[1] = '\0';
  }

  // scrollというかpage down
  if (y >= tft->height()) {
    tft->fillScreen(bgcolor);
    y = 0;
  }
  int ret = tftkanji.drawText(&x, &y, sjisbuf, color, bgcolor);
  sjisbuf[0] = '\0';
  if (ret <= 0) {
    return ret;
  }
  return 1;
}
