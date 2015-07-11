#include "Fontx2.h"

class TFTKanji {
  public:
    TFTKanji();
    virtual ~TFTKanji();
    int open(SdFatBase* sd, const char* kanjifile, const char* asciifile);
    bool close();
    /**
     * 文字列を描画する
     * @return 描画したバイト数
     */
    int draw(Adafruit_GFX *tft, const char* str, int16_t x, int16_t y, uint16_t color);
    int draw(Adafruit_GFX *tft, const char* str, int16_t x, int16_t y, uint16_t color, uint16_t bgcolor);

    /** 文字の高さ */
    int height() const {
      return max(kanjiFont.height(), asciiFont.height());
    }

    static bool iskanji1(int c) {
      return (c >= 0x81 && c <= 0x9f || c >= 0xe0 && c <= 0xff);
    }

  private:
    int draw(Adafruit_GFX *tft, const char* str, int16_t x, int16_t y, uint16_t color, uint16_t bgcolor, bool drawbg);

    Fontx2 kanjiFont;
    Fontx2 asciiFont;
};
