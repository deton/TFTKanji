#include "TFTKanji.h"

/** 漢字対応テキスト端末画面 */
class TFTKanjiTerm {
  public:
    TFTKanjiTerm(Adafruit_GFX *tft);
    virtual ~TFTKanjiTerm();
    int open(SdFatBase* sd, const char* kanjifile, const char* ankfile);
    bool close();
    int addch(int ch);

  private:
    enum State { ST_PRINTABLE, ST_SJIS1, ST_ESC, ST_ESC_DIGIT };
    State state;
    char sjisbuf[3];
    int16_t x;
    int16_t y;
    uint16_t color;
    uint16_t bgcolor;
    TFTKanji tftkanji;
    Adafruit_GFX* tft;
};
