# Arduino用TFT LCD漢字表示ライブラリ

漢字のビットマップデータを、
microSDカード上のfontx2形式ファイルから読み込みながら
TFT LCDに表示するためのArduino用ライブラリです。

![TFTKanjiDemo写真](../img/TFTKanjiDemo.jpg)

## 構成
* Fontx2クラス: fontx2ファイルの読み込みを行う。
* TFTKanjiクラス: TFT LCDに漢字を含む文字列を描画する。
  各文字の描画時に、SDカード上のfontx2ファイルから文字のビットマップを読み込む

サンプル:

* examples/TFTKanjiDemo: 各種フォントを表示するデモ。
* examples/Serial2TFT: SerialからShift_JIS文字列を読んでTFT LCDに表示。
 * TFTKanjiTermクラス: 漢字対応テキスト端末画面。エスケープシーケンス対応。"ESC[row;colH", "ESC[2J", "ESC[K"
* examples/Console2TFT: Linux側からのメッセージを受けてTFT LCDに表示。
 * cgi-bin/tftkanji.py: HTTPで受けた文字列をTFT LCDに表示するCGIスクリプト。
 * yoteihyo/: [TFT LCDに行動予定表を表示](#適用例-行動予定表)。
* examples/DumbDraw: TFT LCD以外に描画するサンプル。

## 使用例
* [Linino ONE](https://www.switch-science.com/catalog/2152/)
* [2.6インチ液晶 for Arduino UNO](http://www.aitendo.com/product/9482)。
  microSDカードソケット付き。

* [SWTFT-Shieldライブラリ](https://github.com/Smoke-And-Wires/TFT-Shield-Example-Code)。
  ただし、Linino ONEで動かすために、
  [TFTLCDライブラリ](https://github.com/adafruit/TFTLCD-Library)の
  pin_magic.hを使用。
  LCDコントローラst7781と、ATmega32U4なLininoONE/Arduino Yun/Leonardoの
  両方に対応しているライブラリを見つけられず組み合わせて使用。
 * [Adafruiit-GFX-Library](https://github.com/adafruit/Adafruit-GFX-Library)
   も必要。
* [SdFatライブラリ](https://github.com/greiman/SdFat)。
  Linino ONEの場合、SPIはMPU側に接続されているため(?)、
  電源オン直後はSDカードが読めるが、Linuxが起動すると読めなくなる。
  software spiを使うためSdFatライブラリを使用。

### BDFフォントとFontx2フォントの変換
* [bdf2fontx.cをANKフォント対応するパッチ](https://gist.github.com/deton/acf8f59e27b25d547bb1)。
  [bdf2fontx.c](http://www.wheel.gr.jp/~dai/fonts/fontx.html) 0.1に対するパッチ。
  jiskan24、Kappa20、Ayu 18、ナガ10フォントを、
  BDF形式からfontx2形式に変換する際に使用。

* [bdfunicode2jis.pl](https://gist.github.com/deton/df79c92195e1ebf9316e)。
  iso10646(unicode)のBDFフォントから、
  jisx0208またはjisx0201のBDFフォントを生成するスクリプト。
  k8x12フォントを変換するために作成。
  (mona font付属のjis2unicode.plの逆)
 * [bdf2fontx.cでno ENDCHARになる問題の修正パッチ](https://gist.github.com/deton/f48b9eff706a10d7312c)。
   k8x12フォントを変換できるように作成。

## 適用例: 行動予定表
Microsoft Exchangeサーバから各人の予定を取得して表示。

![行動予定表写真](../img/yoteihyo.jpg)

(写真は一部ぼかしを入れています)

* examples/Console2TFT/yoteihyo/
 * yoteihyo.py: 予定表表示スクリプト。
 * drawusername.py: メンバの名前を表示するスクリプト。
 * syuttaikin.py: 出退勤表示用スクリプト。出勤時は名前の背景色を青で表示。
 * lyncpresence.py: Lyncプレゼンスを名前の背景色として表示。名前の背景色に関しては、syuttaikin.pyかlyncpresence.pyのいずれかを使う想定。
 * userdata.py: 予定表に表示するメンバの名前と表示位置。
 * location.py: 場所文字列の短縮を行うためのデータ。
 * ignore_patters.py: 無視したい予定の正規表現パターン。

LininoONEのLinux側でスクリプトを実行。
* 1回だけ実行: `python -m yoteihyo.drawusername`
* cronで5分おきに実行: `python -m yoteihyo.syuttaikin deton@example.com http://example.com/cgi-bin/syuttaikin.cgi`
* cronで1時間おきに実行: `python -m yoteihyo.yoteihyo 'http://example.com:8080/YoteihyoServlet/yoteihyo?emails=deton@example.com,taro@example.com'`

syuttaikin.pyに指定するURLは、[自席PCのオン・オフに応じて、onlineまたはofflineを返すCGIスクリプト](https://github.com/deton/syuttaikin/blob/master/server/syuttaikin.cgi)。

lyncpresence.pyに指定するURLは、[Lyncプレゼンスを返すServlet](https://github.com/deton/java-lync-client)。

yoteihyo.pyに指定するURLは、Microsoft Exchangeサーバからの予定取得を行う、
[YoteihyoServlet](https://github.com/deton/YoteihyoServlet)。

## 参考
* [Arduino Due用](http://projectc3.seesaa.net/article/366244240.html)。
  フォントデータをプログラム中に埋め込み。
* [外部フラッシュメモリにフォントデータを入れる](http://nuneno.cocolog-nifty.com/blog/2015/02/arduinorom-a11f.html)
* [外部EEPROMにフォントデータを入れる](http://nuneno.cocolog-nifty.com/blog/2014/01/arduinoi2ceepro.html)
* [mbed用。microSDカード](http://kanpapa.com/today/2011/11/mbed-16dotfont-vfd.html)
* [STM32F4xxxx用?](http://nemuisan.blog.bai.ne.jp/?search=fontx2)
