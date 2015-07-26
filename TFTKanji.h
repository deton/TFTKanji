#ifndef __TFTKANJI_H__
#define __TFTKANJI_H__
#include "Fontx2.h"

// drawText()する文字列中に'\n'があったら、次の行に移動するかどうか
//#define WRAP_NEWLINE 1

// 長い行の折り返し表示を設定できるようにするかどうか
#define WRAP_LONGLINE 1

#define USE_ITKSCREEN 0
#if USE_ITKSCREEN
/*! スクリーンへの描画やサイズ取得を行うためのインタフェースクラス */
class ITKScreen { // ITK: Interface class for TftKanji
  public:
    virtual void drawPixel(int16_t x, int16_t y, uint16_t color) = 0;
    virtual void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) = 0;
    virtual int16_t width() = 0;
    virtual int16_t height() = 0;
};
#else
# include <Adafruit_GFX.h>
# define ITKScreen Adafruit_GFX
#endif

/*!
 * TFT LCDに漢字を含む文字列を描画するクラス。
 * 各文字の描画時に、SDカード上のfontx2ファイルから文字のビットマップを読み込む
 */
class TFTKanji {
  public:
    TFTKanji(ITKScreen* tft);
    virtual ~TFTKanji();
    /*!
     * SDカード上の指定されたフォントファイルを開く。
     * フォントファイルは、SDカードのルートディレクトリにある必要あり。
     * drawText()の前に呼んでおく必要あり。
     * 既にopen()済で再度open()を呼んだ場合は、内部でclose()した後open()を行う。
     * \param kanjifname 漢字フォントファイル名
     * \param ankfname ANKフォントファイル名
     * \return 0: 成功時。それ以外: エラー発生
     */
    int open(SdFatBase* sd, const char* kanjifname, const char* ankfname);
    bool close();
    /*!
     * 文字列を描画する。
     * 事前にopen()を呼んでおく必要あり。
     * \param [in,out] x, y 描画開始位置。描画終了時に次の位置に更新される。
     * \return 描画したバイト数
     */
    int drawText(int16_t* x, int16_t* y, const char* str, uint16_t color
#if WRAP_LONGLINE
        , bool wrap = false
#endif
        );
    int drawText(int16_t* x, int16_t* y, const char* str, uint16_t color, uint16_t bgcolor
#if WRAP_LONGLINE
        , bool wrap = false
#endif
        );

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
    ITKScreen* tft;
    Fontx2 kanjiFont;
    Fontx2 ankFont;
};
#endif // __TFTKANJI_H__
