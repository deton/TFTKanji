#include "Fontx2.h"

class TFTKanji {
  public:
    TFTKanji(Adafruit_GFX *tft);
    virtual ~TFTKanji();
    int open(SdFatBase* sd, const char* kanjifile, const char* ankfile);
    bool close();
    /**
     * 文字列を描画する
     * @param x, y 描画開始位置。描画終了時に次の位置に更新される。
     * @return 描画したバイト数
     */
    int drawText(int16_t* x, int16_t* y, const char* str, uint16_t color);
    int drawText(int16_t* x, int16_t* y, const char* str, uint16_t color, uint16_t bgcolor);

    /** 文字の高さ */
    int height() const {
      return max(kanjiFont.height(), ankFont.height());
    }

    static bool iskanji1(int c) {
      return (c >= 0x81 && c <= 0x9f || c >= 0xe0 && c <= 0xff);
    }

  private:
    Adafruit_GFX *tft;
    Fontx2 kanjiFont;
    Fontx2 ankFont;
};
