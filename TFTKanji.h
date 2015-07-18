#include "Fontx2.h"

class IScreen {
  public:
    virtual void drawPixel(int16_t x, int16_t y, uint16_t color) = 0;
    virtual void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) = 0;
    virtual int16_t width() = 0;
    virtual int16_t height() = 0;
};

class TFTKanji {
  public:
    TFTKanji(IScreen* tft);
    virtual ~TFTKanji();
    int open(SdFatBase* sd, const char* kanjifile, const char* ankfile);
    bool close();
    /*!
     * 文字列を描画する
     * \param [in,out] x, y 描画開始位置。描画終了時に次の位置に更新される。
     * \return 描画したバイト数
     */
    int drawText(int16_t* x, int16_t* y, const char* str, uint16_t color);
    int drawText(int16_t* x, int16_t* y, const char* str, uint16_t color, uint16_t bgcolor);

    /*! 文字の高さ */
    int height() const {
      return max(kanjiFont.height(), ankFont.height());
    }

    /*! ANK文字の幅 */
    int ankWidth() const {
      return ankFont.width();
    }

    /*! SJISの第1バイトかどうか */
    static bool issjis1(int c) {
      return (c >= 0x81 && c <= 0x9f || c >= 0xe0 && c <= 0xff);
    }

  private:
    IScreen* tft;
    Fontx2 kanjiFont;
    Fontx2 ankFont;
};
