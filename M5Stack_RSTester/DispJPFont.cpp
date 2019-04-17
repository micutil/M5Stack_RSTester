/*
	DispJPFont.cpp

	Reference
	M5Stackで日本語表示
	https://qiita.com/taront/items/7900c88b9e9782c33b08

  1: 茶虎たま吉さんのArduino-KanjiFont-Library-SDをダウンロード
  https://github.com/Tamakichi/Arduino-KanjiFont-Library-SD
  
  /liblary/sdfonts フォルダをArduinoのliblaryにコピーする。
  /fontbin フォルダ内の FONT.BIN、FONTLCD.BIN をSDカードの直下に入れる。

  ２：　修正内容
  libraries/sdfonts.h　26行目
    // #define MYSDCLASS SDClass
    #define MYSDCLASS SDFS

  libraries/sdfonts.cpp　24行目
    // #define FONTFILE   "FONT.BIN"     // フォントファイル名
    // #define FONT_LFILE "FONTLCD.BIN"  // グラフィック液晶用フォントファイル名
    #define FONTFILE   "/FONT.BIN"
    #define FONT_LFILE "/FONTLCD.BIN"
*/

#include "DispJPFont.h"

// フォントデータの表示
// buf(in) : フォント格納アドレス
// ビットパターン表示
// d: 8ビットパターンデータ
void fontDisp(uint16_t x, uint16_t y, uint8_t* buf, uint32_t fntcol) {
  uint32_t txt_color = fntcol;
  uint32_t bg_color = TFT_BLACK;

  uint8_t bn = SDfonts.getRowLength();               // 1行当たりのバイト数取得
  /*
  Serial.print(SDfonts.getWidth(), DEC);            // フォントの幅の取得
  Serial.print("x");
  Serial.print(SDfonts.getHeight(), DEC);           // フォントの高さの取得
  Serial.print(" ");
  Serial.println((uint16_t)SDfonts.getCode(), HEX); // 直前し処理したフォントのUTF16コード表示
  */
  for (uint8_t i = 0; i < SDfonts.getLength(); i += bn ) {
    for (uint8_t j = 0; j < bn; j++) {
      for (uint8_t k = 0; k < 8; k++) {
        if (buf[i + j] & 0x80 >> k) {
          M5.Lcd.drawPixel(x + 8 * j + k , y + i / bn, txt_color);
        } else {
          M5.Lcd.drawPixel(x + 8 * j + k , y + i / bn, bg_color);
        }
      }
    }
  }
}

// 指定した文字列を指定したサイズで表示する
// pUTF8(in) UTF8文字列
// sz(in)    フォントサイズ(8,10,12,14,16,20,24)
int fontDump(uint16_t x, uint16_t y, char* pUTF8, uint8_t sz, uint32_t fntcol, bool fontfileOpenClose) {
  uint8_t buf[MAXFONTLEN]; // フォントデータ格納アドレス(最大24x24/8 = 72バイト)
  if(fontfileOpenClose) SDfonts.open();                                   // フォントのオープン
  SDfonts.setFontSize(sz);                          // フォントサイズの設定
  uint16_t mojisu = 0;
  while ( pUTF8 = SDfonts.getFontData(buf, pUTF8) ) { // フォントの取得
    fontDisp(x + mojisu * sz, y, buf, fntcol);                 // フォントパターンの表示
    ++mojisu;
  }

  if(fontfileOpenClose) SDfonts.close();                                  // フォントのクローズ

  return (x + mojisu * sz);
}

int fontDumpCenter(uint16_t x, uint16_t y, char* pUTF8, uint8_t n, uint8_t sz, uint32_t fntcol, bool fontfileOpenClose) {
  x=x-(n*sz)/2;
  fontDump(x,y,pUTF8,sz,fntcol,fontfileOpenClose);
}
