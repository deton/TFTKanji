#include "Fontx2.h"

#define DBGLOG 0

const int ANK_DATA_START = 17;

Fontx2::Fontx2() :start(NULL), end(NULL) {
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
  uint8_t header[18];
  if (sdfile.read(header, 18) < 18) {
    sdfile.close();
    return -2;
  }
  // XXX: プログラムサイズを減らすためチェックは省略
  //if (strncmp(header, "FONTX2", 6) != 0) { // 00-05 "FONTX2"
  //  sdfile.close();
  //  return -2;
  //}
  // XXX: サイズを減らすためfontNameは持たない
  // 06-13 Font name
  _width = header[14];
  _height = header[15];
  uint8_t codetype = header[16];
#if DBGLOG
  Serial.print("width: "); Serial.println(_width);
  Serial.print("height: "); Serial.println(_height);
  Serial.print("codetype: "); Serial.println(codetype);
#endif
  if (codetype == 0) { // ANK font
    tnum = 0;
    return 0;
  }
  tnum = header[17]; // Table entry number
#if DBGLOG
  Serial.print("tnum: "); Serial.println(tnum);
#endif

  // Table read
  // XXX: newでなくcalloc()を使用。Arduinoだとnewは単にmalloc()してるだけなので
  start = (uint16_t*)calloc(tnum, sizeof(uint16_t));
  if (start == NULL) {
    close();
    return -8;
  }
  end   = (uint16_t*)calloc(tnum, sizeof(uint16_t));
  if (end == NULL) {
    close();
    return -8;
  }
  for (int a = 0; a < tnum; a++) {
    if (sdfile.read(&start[a], 2) < 2) {
      close();
      return -9;
    }
    if (sdfile.read(&end[a], 2) < 2) {
      close();
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
  if (end != NULL) {
    free(end);
    end = NULL;
  }
  if (start != NULL) {
    free(start);
    start = NULL;
  }
  return sdfile.close();
}

int Fontx2::bitmapLen() const {
  int byteWidth = (_width + 7) / 8;
  return byteWidth * _height;
}

uint32_t Fontx2::getAnkAddr(uint16_t ank) const {
  return ANK_DATA_START + ank * bitmapLen();
}

uint32_t Fontx2::getKanjiAddr(uint16_t sjis) const {
  int c = 0;
  uint32_t adrs = 0;
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
  uint32_t kanjiDataStart = ANK_DATA_START + 1 + tnum * sizeof(short) * 2;
  return kanjiDataStart + bitmapLen() * adrs;
}

int Fontx2::load(uint16_t sjis, uint8_t* buf, int bufsize) const {
  if (!sdfile.isOpen()) {
    return -2;
  }

  uint32_t adrs;
  if (tnum == 0) {
    adrs = getAnkAddr(sjis);
  } else {
    adrs = getKanjiAddr(sjis);
  }

  // check file size before seek
  if (adrs < ANK_DATA_START || adrs > sdfile.fileSize()) {
    return -1; // 指定された文字コードに対する文字データ無し
  }
  if (!sdfile.seekSet(adrs)) {
    return -1;
  }

  int len = bitmapLen();
  if (bufsize < len) {
    return -3;
  }

  int ret;
  if ((ret = sdfile.read(buf, len)) < len) {
#if DBGLOG
    Serial.print("read error:");
    Serial.println(ret);
#endif
    return -4;
  }

#if DBGLOG
  // debug dump
  for (int s = 0; s < len; s++){
    Serial.print(buf[s], HEX);
    Serial.print(' ');
  }
  Serial.println(); 
#endif

  return 0;
}
