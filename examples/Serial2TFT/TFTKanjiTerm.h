#ifndef __TFTKANJITERM_H__
#define __TFTKANJITERM_H__
#include "TFTKanji.h"

#if USE_ITKSCREEN
class ITKFScreen: public ITKScreen {
  public:
    virtual void fillScreen(uint16_t color) = 0;
};
#else
# define ITKFScreen Adafruit_GFX
#endif

/*!
 * 漢字対応テキスト端末画面。
 * エスケープシーケンス対応。"ESC[row;colH", "ESC[2J", "ESC[K"
 * ("\n"はTFTKanjiクラスで対応)
 */
class TFTKanjiTerm {
  public:
    TFTKanjiTerm(ITKFScreen* tft);
    virtual ~TFTKanjiTerm();
    int open(SdFatBase* sd, const char* kanjifile, const char* ankfile);
    bool close();
    int addch(int ch);
#if WRAP_LONGLINE
    void setWrap(bool b) {
      wrap = b;
    }
#endif

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
#if WRAP_LONGLINE
    bool wrap;
#endif
    TFTKanji tftkanji;
    ITKFScreen* tft;
};
#endif // __TFTKANJITERM_H__
