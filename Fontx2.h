#include <SdFat.h>
#include <Adafruit_GFX.h>

class Fontx2 {
  public:
    Fontx2();
    virtual ~Fontx2();
    /**
     * フォントファイルを開く。
     * @param filepath SDカードのルートディレクトリにあるファイル名。
     * @return 0: 成功時。それ以外: エラー発生
     */
    int open(SdFatBase* sd, const char* filepath);
    bool close();
    /**
     * 指定したShift_JISコードの文字を描画する。
     * @param x, y 描画開始位置。描画終了時に次の位置に更新される。
     * @return 0: 成功。
     *  -1: 指定された文字コードに対する文字データ無し。
     *  それ以外: エラー発生
     */
    int draw(Adafruit_GFX* tft, int16_t* x, int16_t* y, uint16_t sjis, uint16_t color);
    /** 文字幅 */
    int width() const {
      return XSize;
    }
    /** 文字高さ */
    int height() const {
      return YSize;
    }

  private:
    uint32_t getAnkAddr(uint16_t ank);
    //int getKanjiBlock(uint16_t sjis);
    uint32_t getKanjiAddr(uint16_t sjis);
    /** 1文字ぶんのビットマップデータのバイト数 */
    int bitmapLen();
    int readAndDrawBitmap(Adafruit_GFX* tft, int16_t* x, int16_t* y, int len, uint16_t color);

    char            FontName[8+1];      // 06-13 Font name    
    unsigned char   XSize;              // 14            
    unsigned char   YSize;              // 15
    unsigned char   CodeType;           // 16 0:ANK, 1:KANJI   
    unsigned char   Tnum;               // 17 Table entry number

    // table
    uint16_t* start;
    uint16_t* end;

    SdFile sdfile;
};
