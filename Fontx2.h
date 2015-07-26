#ifndef __FONTX2_H__
#define __FONTX2_H__
#include <SdFat.h>

/*! fontx2フォントデータの読み込みを行うためのクラス */
class Fontx2 {
  public:
    Fontx2();
    virtual ~Fontx2();
    /*!
     * フォントファイルを開く。
     * 既にopen()済で再度open()を呼んだ場合は、内部でclose()した後open()を行う。
     * \param filepath SDカードのルートディレクトリにあるファイル名。
     * \return 0: 成功時。それ以外: エラー発生
     */
    int open(SdFatBase* sd, const char* filepath);
    bool close();
    /*!
     * 指定したShift_JISコードの文字のビットマップデータを読み込む。
     * \param [out] buf 読み込み先バッファ
     * \param bufsize bufのバイト数
     * \return 0: 成功。
     *  -1: 指定された文字コードに対する文字データ無し。
     *  それ以外: エラー発生
     */
    int load(uint16_t sjis, uint8_t* buf, int bufsize) const;
    /*! 1文字ぶんのビットマップデータのバイト数 */
    int bitmapLen() const;
    /*! 文字幅 */
    int width() const {
      return _width;
    }
    /*! 文字高さ */
    int height() const {
      return _height;
    }

  private:
    uint32_t getAnkAddr(uint16_t ank) const;
    uint32_t getKanjiAddr(uint16_t sjis) const;

    uint8_t _width;
    uint8_t _height;
    uint8_t tnum;
    struct Table {
      uint16_t start;
      uint16_t end;
    } *table;

    mutable FatFile sdfile;
};
#endif // __FONTX2_H__
