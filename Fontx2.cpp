#include "Fontx2.h"

#define DBGLOG 1

const int ASCII_DATA_START = 17;

Fontx2::Fontx2() {
}

Fontx2::~Fontx2() {
  close();
}

int Fontx2::open(SdFatBase* sd, const char* filepath) {
  // if already open, close and open
  close();
  if (!sdfile.open(sd, filepath, O_READ)) {
    return -1;
  }

  // read header
  // cf. http://kanpapa.com/today/2011/11/mbed-16dotfont-vfd.html
  char identifier[6+1];    // 00-05 "FONTX2"   
  if (sdfile.read(identifier, 6) < 6) {    // "FONTX2"
    sdfile.close();
    return -2;
  }
  if (sdfile.read(FontName, 8) < 8) {
    sdfile.close();
    return -3;
  }
  unsigned char b;
  if (sdfile.read(&b, 1) < 1) {
    sdfile.close();
    return -4;
  }
  XSize = b;
  if (sdfile.read(&b, 1) < 1) {
    sdfile.close();
    return -5;
  }
  YSize = b;
  if (sdfile.read(&b, 1) < 1) {
    sdfile.close();
    return -6;
  }
  CodeType = b;
#if DBGLOG
  Serial.print("Identifier: "); Serial.println(identifier);
  Serial.print("FontName: "); Serial.println(FontName);
  Serial.print("XSize: "); Serial.println(XSize);
  Serial.print("YSize: "); Serial.println(YSize);
  Serial.print("CodeType: "); Serial.println(CodeType);
#endif
  if (CodeType == 0) { // ASCII font
    return 0;
  }
  if ((Tnum = sdfile.read()) <= 0) {
    sdfile.close();
    return -7;
  }
#if DBGLOG
  Serial.print("Tnum: "); Serial.println(Tnum);
#endif
  if (Tnum >= 92) { // 固定サイズでstart[92]等を持っているので
    sdfile.close();
    return -8;
  }

  // Table read
  for (int a = 0; a < Tnum; a++) {
    if (sdfile.read(&start[a], 2) < 2) {
      sdfile.close();
      return -9;
    }
    if (sdfile.read(&end[a], 2) < 2) {
      sdfile.close();
      return -10;
    }
#if DBGLOG
    Serial.print("Table No."); Serial.print(a);
    Serial.print(" start:"); Serial.print(start[a], HEX);
    Serial.print(" end:"); Serial.println(end[a], HEX);
#endif
  }
  return 0;
}

bool Fontx2::close() {
  return sdfile.close();
}

// from Adafruit_GFX::drawBitmap()
void drawBitmap(Adafruit_GFX *tft, int16_t x, int16_t y,
    const uint8_t *bitmap, int16_t w, int16_t h,
    uint16_t color) {

  int16_t i, j, byteWidth = (w + 7) / 8;

  for(j=0; j<h; j++) {
    for(i=0; i<w; i++ ) {
      if(*(bitmap + j * byteWidth + i / 8) & (128 >> (i & 7))) {
        tft->drawPixel(x+i, y+j, color);
      }
    }
  }
}

int Fontx2::bitmapLen() {
  int byteWidth = (width() + 7) / 8;
  return byteWidth * height();
}

int Fontx2::getAsciiAddr(uint16_t ascii) {
  if (ascii > 0x7f) {
    ascii = '_'; // unknown char
  }
  return ASCII_DATA_START + ascii * bitmapLen();
}

#if 0
int Fontx2::getKanjiBlock(uint16_t sjis) {
  for (int i = 0; i < Tnum; i++) {
    if (start[i] >= sjis && end[i] <= sjis) {
      return i;
    }
  }
  return -1;
}
#endif

int Fontx2::getKanjiAddr(uint16_t sjis) {
  /*
  int block = getKanjiBlock(sjis);
  if (block == -1) {
    sjis = 0x81a0; // 文字がない場合の置き換え文字"□"
    block = getKanjiBlock(sjis);
  }
  int offset = sjis - start[block];
  */
  int c = 0;
  int adrs = 0;
  while(sjis > start[c]){
    if (sjis > end[c]){
      adrs += end[c] - start[c] + 1;
    } else {
      adrs += sjis - start[c];
    }
    c++;
  }
  c--;
#if DBGLOG
  Serial.print("sjis="); Serial.print(sjis, HEX);
  Serial.print(",table num="); Serial.print(c);
  Serial.print(",adrs="); Serial.println(adrs, HEX);
#endif
  int kanjiDataStart = ASCII_DATA_START + 1 + Tnum * sizeof(short) * 2;
  return kanjiDataStart + bitmapLen() * adrs;
}

int Fontx2::draw(Adafruit_GFX *tft, uint16_t sjis, int16_t x, int16_t y, uint16_t color) {
  if (!sdfile.isOpen()) {
    return -2;
  }

  // 24x24 font requires 72 bytes.
  unsigned char bitmap[72]; // font bitmap read buffer 

  int adrs;
  if (CodeType == 0) {
    adrs = getAsciiAddr(sjis);
  } else {
    adrs = getKanjiAddr(sjis);
  }

  // TODO: check file size before seek
  // TODO: use GETA or TOFU if no data
  // Kanji image Read
  if (!sdfile.seekSet(adrs)) {
#if DBGLOG
    Serial.println("seek failed");
#endif
    return -1;
  }
  int ret;
  int len = bitmapLen();
  if (len > sizeof(bitmap)) {
    return -3;
  }
  if ((ret = sdfile.read(&bitmap, len)) < len) {
#if DBGLOG
    Serial.print("read error:");
    Serial.println(ret);
#endif
    return -4;
  }

#if DBGLOG
  // debug dump
  for (int s = 0; s < len; s++){
    Serial.print(bitmap[s], HEX);
    Serial.print(' ');
  }
  Serial.println(); 
#endif

  drawBitmap(tft, x, y, bitmap, XSize, YSize, color);
  return 0;
}
