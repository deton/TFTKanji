# Arduino用TFT LCD漢字表示ライブラリ

漢字のビットマップデータを、
microSDカード上のfontx2形式ファイルから読み込みながら
TFT LCDに表示するためのライブラリです。

![TFTKanjiDemo写真](../img/TFTKanjiDemo.jpg)

## 構成
* Fontx2クラス: fontx2ファイルの読み込みを行う。
* TFTKanjiクラス: TFT LCDに漢字を含む文字列を描画する。
  各文字の描画時に、SDカード上のfontx2ファイルから文字のビットマップを読み込む
* TFTKanjiTermクラス: 漢字対応テキスト端末画面。
  エスケープシーケンス対応。"ESC[row;colH", "ESC[2J", "ESC[K"

* examples/TFTKanjiDemo: 各種フォントを表示するデモ。
* examples/Serial2TFT: Serialから読んでTFT LCDに表示。

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

## 拡張案
* HTTPで受信した文字列をTFT LCD表示。
  Serial2TFTをConsoleから読むようにすれば可能だが、
  そのままだとスケッチサイズが大きくてMCUに書き込めず。

## 参考
* [Arduino Due用](http://projectc3.seesaa.net/article/366244240.html)。
  フォントデータをプログラム中に埋め込み。
* [外部フラッシュメモリにフォントデータを入れる](http://nuneno.cocolog-nifty.com/blog/2015/02/arduinorom-a11f.html)
* [外部EEPROMにフォントデータを入れる](http://nuneno.cocolog-nifty.com/blog/2014/01/arduinoi2ceepro.html)
* [mbed用。microSDカード](http://kanpapa.com/today/2011/11/mbed-16dotfont-vfd.html)
* [STM32F4xxxx用?](http://nemuisan.blog.bai.ne.jp/?search=fontx2)
