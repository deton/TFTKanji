#include <SdFat.h>
#include <Adafruit_GFX.h>

class Fontx2 {
  public:
	Fontx2();
	virtual ~Fontx2();
	int open(SdFatBase* sd, const char* filepath);
	bool close();
	int draw(Adafruit_GFX *tft, uint16_t sjis, int16_t x, int16_t y, uint16_t color);
	int getXSize() {
	  return XSize;
	}
	int getYSize() {
	  return YSize;
	}

  private:
	//int getKanjiBlock(uint16_t sjis);
	int getKanjiAddr(uint16_t sjis);

	// TODO: allocate table for kanji font only
	unsigned short  start[92], end[92]; // table

	char            FontName[8+1];      // 06-13 Font name    
	unsigned char   XSize;              // 14            
	unsigned char   YSize;              // 15
	unsigned char   CodeType;           // 16 0:ASCII, 1:KANJI   
	unsigned char   Tnum;               // 17 Table entry number

	SdFile sdfile;
};
