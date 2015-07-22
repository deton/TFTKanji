#include "TFTKanjiTerm.h"

#define DBGLOG 0

const uint16_t WHITE = 0xFFFF;
const uint16_t BLACK = 0x0000;

const char ESC = '\x1b';

TFTKanjiTerm::TFTKanjiTerm(ITKScreen* tft)
  :tft(tft), tftkanji(tft), state(ST_PRINT), x(0), y(0),
   color(BLACK), bgcolor(WHITE) {
  buf[0] = '\0';
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
  switch (state) {
  case ST_ESC:
    if (ch == '[') {
      state = ST_CSI;
      nparam = 0;
      return 1;
    } else {
      state = ST_PRINT;
      buf[0] = ESC;
      buf[1] = '\0';
    }
    break;
  case ST_CSI: // ESC '['
    if (isdigit(ch)) {
      state = ST_ESC_DIGIT;
      escparam[0] = ch - '0';
      nparam = 1;
      return 1;
    } else if (ch == 'K') { // ESC[K clear to end of line
      state = ST_PRINT;
      tft->fillRect(x, y, tft->width() - x, tftkanji.height(), bgcolor);
      return 1;
    } else { // ignore unsupport escape sequence
      state = ST_PRINT;
      return 1;
    }
    break;
  case ST_ESC_DIGIT: // ESC '[' digit
    if (isdigit(ch)) {
      escparam[nparam-1] = escparam[nparam-1] * 10 + ch - '0';
      return 1;
    } else if (ch == ';') {
      nparam++;
      escparam[nparam-1] = 0;
      return 1;
    } else if (ch == 'J') {
      state = ST_PRINT;
      if (nparam == 1 && escparam[nparam-1] == 2) { // ESC[2J clear screen
        tft->fillScreen(bgcolor);
        x = 0;
        y = 0;
      }
      return 1;
    } else if (ch == 'H') {
      state = ST_PRINT;
      if (nparam == 2) { // ESC[row;colH move cursor to row;col
        int row = escparam[0];
        if (row < 1) {
          row = 1;
        }
        int col = escparam[1];
        if (col < 1) {
          col = 1;
        }
        // 左上位置はrow=1,col=1→x=0,y=0
        y = (row-1) * tftkanji.height();
        x = (col-1) * tftkanji.ankWidth();
      }
      return 1;
    } else { // ignore unsupport escape sequence
      state = ST_PRINT;
      return 1;
    }
    break;
  case ST_SJIS1: // ch is SJIS 2nd byte
    state = ST_PRINT;
    buf[1] = ch;
    buf[2] = '\0';
    break;
  case ST_PRINT:
  default:
    if (TFTKanji::issjis1(ch)) { // SJIS 1st byte
      state = ST_SJIS1;
      buf[0] = ch;
      return 1;
    } else if (ch == ESC) {
      state = ST_ESC;
      return 1;
    } else {
      buf[0] = ch;
      buf[1] = '\0';
    }
    break;
  }

  // scrollというかpage down
  if (y >= tft->height()) {
    tft->fillScreen(bgcolor);
    x = 0;
    y = 0;
  }
  int ret = tftkanji.drawText(&x, &y, buf, color, bgcolor);
  if (ret <= 0) {
    return ret;
  }
  return 1;
}
