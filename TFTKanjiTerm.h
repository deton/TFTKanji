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
    enum State { ST_PRINT, ST_SJIS1, ST_ESC, ST_CSI, ST_ESC_DIGIT };
    State state;
    char buf[3];
    int escparam[3]; // only support "ESC[row;colH", "ESC[2J", "ESC[K"
    int nparam;

    int16_t x;
    int16_t y;
    uint16_t color;
    uint16_t bgcolor;
    TFTKanji tftkanji;
    Adafruit_GFX* tft;
};
