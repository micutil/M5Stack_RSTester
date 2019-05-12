/*************************************************
 * M5Stack RSTester
 * 
 * ver 1.0.1 (2019/05/12) ODROID-GO対応/FACES対応
 * ver 1.0.0 (2019/04/07)
 * 
 * This application is M5Stack version of RSTester.
 * RSTester is compatible with RS30x servo by Futaba.
 * We can chenage servo ID, panch et, al., settings, 
 * and, get information of these settings.
 * 
 * build for Odroid-go
 * Replace library of M5Stack to
 * https://github.com/tobozo/ESP32-Chimera-Core
 * 
 * LISENCE
 *  CC 4.0 BY-NC-ND Micono Utilities (Michio Ono)
 *  
 *  About Creative Commons
 *  https://creativecommons.org/licenses/by-nc-nd/4.0/
 * 
 *************************************************/

#include <M5Stack.h>
#include "M5StackUpdater.h" //for SD updater

#include "RS30x.h"

bool startkey = false;
#ifndef ARDUINO_ODROID_ESP32
  #define useFACES
  //For Faces
  #ifdef useFACES
    #define KEYBOARD_I2C_ADDR     0x08
    #define KEYBOARD_INT          5
  #endif
#endif

#define useJPFont
#ifdef useJPFont
  #include "DispJPFont.h"
  #include "SD.h"
  #define SD_PN 4
#endif

int selnum=0;
int viewType=0;
enum {
  viewMain=0,
  viewMove,
  viewChgID,
  viewInfo
};

int stepmode=0;
enum {
  selID=0,    //ID選択
  selAgl,     //角度
  selMove,    //動作テスト

  selToID=1,  //変更先ID
  selChgID,   //ID変更

  selSet=1,   //設定選択
  selValue,   //設定値
  selChgVlu,   //設定変更
  selChgVlu2   //設定変更
};

int chgSetID=0;
enum {
  chgModel=0,
  chgFirm,
  chgBaud,
  chgTorq,
  chgOn,
  chgCurr,
  chgVolt,

  chgTemp,
  chgTempLmt,
  chgMgnR,
  chgMgnL,
  chgSlpR,
  chgSlpL,
  chgPanch,

  chgRev,
  chgRet,
  chgRLmt,
  chgLLmt,
  chgPos,
  chgCancel,
  chgWrite,
};

int targetID=0;
int servoNum=0;
int servoID[10];

int targetAgl=1;
int angleNum=10;
int angleID[]={5,15,30,45,60,75,90,105,120,135,150};

int changeToID=1;

rsrom rsrom;
rsram rsram;

int btnType=0;
enum {
  btnABC=0, //A, B, C
  btnDwnOKUp, //Down, OK, Up
  btnDwnSelRight, //Down, Sel, Right
  btnDwnNonRight, //Down, Non, Right
  btnNonBckNon, //Non, Back, Non
  btnNone, //None
};

int working=0;

int smpX = 40;
int smpS = 96;
int smpY = 220;
int fntsz = 12;

void drawButtonName() {
  M5.Lcd.fillRect(0, smpY, 320, 240-smpY, 0);
  
  SDfonts.open(); // フォントのオープン
  switch (btnType) {
    case btnABC:
      #ifdef ARDUINO_ODROID_ESP32
      fontDump(70, smpY, "(C)rossキー", fntsz, TFT_WHITE, false);
      //fontDumpCenter(70, smpY, "(C)rossキー", 9, fntsz, TFT_WHITE, false);
      //fontDumpCenter(160, smpY, "B", 1, fntsz, TFT_WHITE, false);
      //fontDumpCenter(250, smpY, "A", 1, fntsz, TFT_WHITE, false);
      #else
      fontDumpCenter(70, smpY, "A", 1, fntsz, TFT_WHITE, false);
      fontDumpCenter(160, smpY, "B", 1, fntsz, TFT_WHITE, false);
      fontDumpCenter(250, smpY, "C", 1, fntsz, TFT_WHITE, false);
      #endif
      break;
    case btnDwnOKUp:
      fontDumpCenter(70, smpY, "↓", 2, fntsz, TFT_WHITE, false);
      fontDumpCenter(160, smpY, "ＯＫ(B)", 5, fntsz, TFT_GREEN, false);
      fontDumpCenter(260, smpY, "↑", 2, fntsz, TFT_WHITE, false);
      break;
    case btnDwnSelRight:
      fontDumpCenter(70, smpY, "↓", 2, fntsz, TFT_WHITE, false);
      fontDumpCenter(160, smpY, "選択(B)", 5, fntsz, TFT_GREEN, false);
      fontDumpCenter(260, smpY, "→", 2, fntsz, TFT_WHITE, false);
      break;
    case btnDwnNonRight:
      fontDumpCenter(70, smpY, "↓", 2, fntsz, TFT_WHITE, false);
      fontDumpCenter(160, smpY, "", 2, fntsz, TFT_GREEN, false);
      fontDumpCenter(260, smpY, "→", 2, fntsz, TFT_WHITE, false);
      break;
    case btnNonBckNon:
      fontDumpCenter(70, smpY, "", 2, fntsz, TFT_WHITE, false);
      fontDumpCenter(160, smpY, "戻る(B)", 5, fntsz, TFT_YELLOW, false);
      fontDumpCenter(260, smpY, "", 2, fntsz, TFT_WHITE, false);
      break;
    default:
      break;
  }
  SDfonts.close(); // フォントのクローズ  
}

int vyps;
int vfntsz;
int vmrgn;

void drawConnectServo(bool w) {
  int xps=50;
  int yps=190;
  int fntsz=14;
  uint16_t c=TFT_WHITE;
  M5.Lcd.fillRect(xps, yps, 320, fntsz, 0);
  
  String s="";
  if(w) {
    c=TFT_RED;
    s="接続サーボを検索しています...";
  } else {
    s="接続ID:";
    for(int i=0;i<servoNum;i++) {
      s=s+String(servoID[i]);
      if(i<servoNum-1) s=s+",";
    }
    //s=s+"(255)";
  }
  
  fontDump(xps, yps, (char*)s.c_str(), fntsz, c);
}

String baudrate[]={"9600","14400","19200","28800","38400","57600","76800","115200","15300","230400"};
typedef struct infoList {
  int     type;
  String  title;
  String  desc;
  String  disp;
  int     defv;
  int     value;
  int     minv;
  int     maxv;
  double  unit;
  String  unin;
};
infoList ilt[3][7];

int lstX=0,lstY=0;

void drawInfoDesc(int x, int y) {
  int xps=10;
  int yps=195;
  int fntsz=14;
  M5.Lcd.fillRect(xps, yps, 320, fntsz, 0);
  String s="->"+ilt[x][y].desc;
  fontDump(xps, yps, (char*)s.c_str(), fntsz, TFT_YELLOW);
  if(stepmode==selValue) {
    yps-=20;
    M5.Lcd.fillRect(xps, yps, 320, fntsz, 0);
    String s="->初期値："+String(ilt[x][y].defv);
    switch(chgSetID) {
      case chgMgnR: s=s+" / 1"; break;
      case chgMgnL: s=s+" / 1"; break;
      case chgSlpR: s=s+" / 1"; break;
      case chgSlpL: s=s+" / 1"; break;
      case chgPanch: s=s+" / 8"; break;
    }
    fontDump(xps, yps, (char*)s.c_str(), fntsz, TFT_ORANGE);
  }
}

void drawInfoList(int x, int y, bool b) {
  Serial.print(x); Serial.print(" ")+Serial.println(y);
  if(b&&(lstX!=x||lstY!=y)) drawInfoList(lstX,lstY,false);
  String s="";
  int fntsz=12;
  int mrgn=fntsz+2;
  int xps[]={10,110,200};
  int yps=vyps+y*mrgn;
  uint32_t c;
  switch(ilt[x][y].type) {
    case 0: c=TFT_WHITE; break; //Read only
    case 1: c=TFT_GREEN; break; //Read & Write
    case 2: c=TFT_BLUE; break; //Retue to menu
    case 3: c=TFT_RED; break; //Go
  }
  if(b) {c=TFT_YELLOW; lstX=x; lstY=y;}
  s=ilt[x][y].title+ilt[x][y].disp;
  fontDump(xps[x], yps, (char*)s.c_str(), fntsz, c);
  if(b) {
    drawInfoDesc(x,y);
    switch(ilt[x][y].type) {
      case 0://btnType=btnDwnNonRight; break;
      case 1:
      case 2:
      case 3:
        btnType=btnDwnSelRight;
        break;
    }
    drawButtonName();
  }
}

void initInfoList() {
  int x=0,y=0;
  ilt[x][y].type=0;ilt[x][y].title="モデル:";ilt[x][y].desc="モデル番号(サーボ機種)を表します";ilt[x][y].unit=1;ilt[x][y].defv=0x3040;y++;
  ilt[x][y].type=0;ilt[x][y].title="Ｖｅｒ:";ilt[x][y].desc="ファームウエアバージョン...";ilt[x][y].unit=1;ilt[x][y].defv=3;y++;
  ilt[x][y].type=0;ilt[x][y].title="通信速:";ilt[x][y].desc="";ilt[x][y].unin="bps";ilt[x][y].unit=1;ilt[x][y].defv=7;y++;
  ilt[x][y].type=0;ilt[x][y].title="トルク:";ilt[x][y].desc="トルク　(％)";ilt[x][y].unit=1;ilt[x][y].unin="%";ilt[x][y].defv=0x64;y++;
  ilt[x][y].type=0;ilt[x][y].title="オ　ン:";ilt[x][y].desc="0->OFF, 1-> ON";ilt[x][y].unit=1;ilt[x][y].defv=0;y++;
  ilt[x][y].type=0;ilt[x][y].title="現負荷:";ilt[x][y].desc="電流(mA)";ilt[x][y].unit=1;ilt[x][y].unin="mA";ilt[x][y].defv=0;y++;
  ilt[x][y].type=0;ilt[x][y].title="現電圧:";ilt[x][y].desc="供給電圧(10mV)";ilt[x][y].unit=10;ilt[x][y].unin="mV";ilt[x][y].defv=0;y++;
  x++;y=0;
  ilt[x][y].type=0;ilt[x][y].title="現温度:";ilt[x][y].desc="基板温度";ilt[x][y].unit=1;ilt[x][y].unin="℃";ilt[x][y].defv=0;y++;
  ilt[x][y].type=0;ilt[x][y].title="限温度:";ilt[x][y].desc="基板上限温度";ilt[x][y].unit=1;ilt[x][y].unin="℃";ilt[x][y].defv=0x4D;y++;
  ilt[x][y].type=1;ilt[x][y].title="右許容:";ilt[x][y].desc="許容範囲：0~25.5度(0.1度)";ilt[x][y].minv=0;ilt[x][y].maxv=255;ilt[x][y].unit=0.1;ilt[x][y].unin="度";ilt[x][y].defv=2;y++;
  ilt[x][y].type=1;ilt[x][y].title="左許容:";ilt[x][y].desc="許容範囲：0~25.5度(0.1度)";ilt[x][y].minv=0;ilt[x][y].maxv=255;ilt[x][y].unit=0.1;ilt[x][y].unin="度";ilt[x][y].defv=2;y++;
  ilt[x][y].type=1;ilt[x][y].title="右反発:";ilt[x][y].desc="ハンチング：0~255度";ilt[x][y].minv=0;ilt[x][y].maxv=255;ilt[x][y].unit=1;ilt[x][y].unin="度";ilt[x][y].defv=8;y++;
  ilt[x][y].type=1;ilt[x][y].title="左反発:";ilt[x][y].desc="ハンチング：0~255度";ilt[x][y].minv=0;ilt[x][y].maxv=255;ilt[x][y].unit=1;ilt[x][y].unin="度";ilt[x][y].defv=8;y++;
  ilt[x][y].type=1;ilt[x][y].title="パンチ:";ilt[x][y].desc="モータの最小電流(0.01or0.4%)";ilt[x][y].minv=0;ilt[x][y].maxv=2000;ilt[x][y].unit=0.4;ilt[x][y].unin="%";ilt[x][y].defv=0x0258;y++;
  x++;y=0;
  ilt[x][y].type=1;ilt[x][y].title="反　転:";ilt[x][y].desc="回転方向 0:正転, 1:反転";ilt[x][y].minv=0;ilt[x][y].maxv=1;ilt[x][y].unit=1;ilt[x][y].defv=0;y++;
  ilt[x][y].type=1;ilt[x][y].title="返信遅:";ilt[x][y].desc="リターンパケット返信遅延";ilt[x][y].minv=0;ilt[x][y].maxv=100;ilt[x][y].unit=50;ilt[x][y].unin="µs";ilt[x][y].defv=0;y++;
  ilt[x][y].type=1;ilt[x][y].title="右限界:";ilt[x][y].desc="限界角度：0〜150度(0.1度)";ilt[x][y].minv=-1500;ilt[x][y].maxv=1500;ilt[x][y].unit=0.1;ilt[x][y].unin="度";ilt[x][y].defv=0x05DC;y++;
  ilt[x][y].type=1;ilt[x][y].title="左限界:";ilt[x][y].desc="限界角度：-150~0度(0.1度)";ilt[x][y].minv=-1500;ilt[x][y].maxv=1500;ilt[x][y].unit=0.1;ilt[x][y].unin="度";ilt[x][y].defv=0xFA24;y++;
  ilt[x][y].type=0;ilt[x][y].title="現位置:";ilt[x][y].desc="現在の角度：-150~150度(0.1度)";ilt[x][y].unit=0.1;ilt[x][y].unin="度";ilt[x][y].defv=0;y++;
  ilt[x][y].type=2;ilt[x][y].title="保存しないで戻る";ilt[x][y].desc="トップメニューに戻ります";y++;
  ilt[x][y].type=3;ilt[x][y].title="保存して戻る";ilt[x][y].desc="書込んでトップに戻ります";y++;

  //ilt[x][y].type=0;ilt[x][y].title="指位置:";ilt[x][y].desc="";y++;
  //ilt[x][y].type=0;ilt[x][y].title="指時間:";ilt[x][y].desc="";y++;
  //ilt[x][y].type=0;ilt[x][y].title="現時間:";ilt[x][y].desc="";y++;
  //ilt[x][y].type=0;ilt[x][y].title="現速度:";ilt[x][y].desc="";y++;
}

void setServoInfo() {
  int x=0,y=0;
  ilt[x][y].disp=String(rsrom.model,HEX);ilt[x][y].value=rsrom.model;y++;
  ilt[x][y].disp=String(rsrom.firmware);ilt[x][y].value=rsrom.firmware;y++;
  ilt[x][y].disp=String(rsrom.baudrate);ilt[x][y].value=rsrom.baudrate;ilt[x][y].desc="通信速度："+baudrate[ilt[x][y].value=rsrom.baudrate]+"bps";y++;
  ilt[x][y].disp=String(rsram.torque);ilt[x][y].value=rsram.torque;y++;
  ilt[x][y].disp=String(rsram.torqueEnable);ilt[x][y].value=rsram.torqueEnable;y++;
  ilt[x][y].disp=String(rsram.pCurr);ilt[x][y].value=rsram.pCurr;y++;
  ilt[x][y].disp=String(rsram.pVolt);ilt[x][y].value=rsram.pVolt;y++;
  x++;y=0;
  ilt[x][y].disp=String(rsram.pTemp);ilt[x][y].value=rsram.pTemp;y++;
  ilt[x][y].disp=String(rsrom.tempLmt);ilt[x][y].value=rsrom.tempLmt;y++;
  ilt[x][y].disp=String(rsrom.RCmpMgn);ilt[x][y].value=rsrom.RCmpMgn;y++;
  ilt[x][y].disp=String(rsrom.LCmpMgn);ilt[x][y].value=rsrom.LCmpMgn;y++;
  ilt[x][y].disp=String(rsrom.RCmpSlp);ilt[x][y].value=rsrom.RCmpSlp;y++;
  ilt[x][y].disp=String(rsrom.LCmpSlp);ilt[x][y].value=rsrom.LCmpSlp;y++;
  ilt[x][y].disp=String(rsrom.punch);ilt[x][y].value=rsrom.punch;y++;  
  x++;y=0;
  ilt[x][y].disp=String(rsrom.reverse);ilt[x][y].value=rsrom.reverse;y++;
  ilt[x][y].disp=String(rsrom.returndelay);ilt[x][y].value=rsrom.returndelay;y++;
  ilt[x][y].disp=String(rsrom.RLmt);ilt[x][y].value=rsrom.RLmt;y++;
  ilt[x][y].disp=String(rsrom.LLmt);ilt[x][y].value=rsrom.LLmt;y++;
  ilt[x][y].disp=String(rsram.pPos);ilt[x][y].value=rsram.pPos;y++;
  //ilt[x][y].disp=String(rsram.gPos);ilt[x][y].value=rsram.gPos;y++;
  //ilt[x][y].disp=String(rsrom.gTime);ilt[x][y].value=rsram.gTime;y++;
  //ilt[x][y].disp=String(rsram.pTime);ilt[x][y].value=rsram.pTime;y++;  
  //ilt[x][y].disp=String(rsram.Speed);ilt[x][y].value=rsram.Speed;y++;  
}

void drawServoInfo() {
  int yps=vyps;
  M5.Lcd.fillRect(0, yps, 320, 220-yps, 0);
  
  for(int x=0; x<3; x++) {
    for(int y=0; y<7; y++) {
      drawInfoList(x,y,lstX==x&&lstY==y);
    }
  }
}

void drawMenuName() {
  M5.Lcd.fillRect(0, 60, 320, smpY-60-50, 0);
  SDfonts.open(); // フォントのオープン

  String s="";
  int xps=50;
  int yps=70;
  int fntsz=14;
  int mrgn=fntsz+4;
  switch(viewType) {
    case viewMain://メイン
      yps=85;fntsz=20; mrgn=fntsz+10;
      fontDump(xps, yps, "A: 動作テスト", fntsz, TFT_CYAN);
      fontDump(xps, yps+mrgn, "B: ID変更", fntsz, TFT_CYAN);
      fontDump(xps, yps+mrgn*2, "C: 各種設定", fntsz, TFT_CYAN);
      break;
      
    case viewMove://動作テスト
      xps=30; yps=70;fntsz=20; mrgn=fntsz+10;
      fontDump(xps, yps, "動作テスト", fntsz, TFT_WHITE);
      yps=yps+mrgn;fntsz=16; mrgn=fntsz+6;
      switch(stepmode) {
        case selID:
          fontDump(xps, yps, "サーボIDを選択して下さい", fntsz, TFT_CYAN);
          xps=50; vyps=yps=yps+mrgn;vfntsz=fntsz=20; mrgn=fntsz+10;
          fontDump(xps, yps, "ＩＤ：", fntsz, TFT_YELLOW);
          drawIDValue();
          break;
        case selAgl:
          fontDump(xps, yps, "テスト角度を選択して下さい", fntsz, TFT_CYAN);
          xps=50; vyps=yps=yps+mrgn;vfntsz=fntsz=20; mrgn=fntsz+10;    
          fontDump(xps, yps, "角度：", fntsz, TFT_YELLOW);
          drawAngleValue();
          break;
        case selMove:
          s="ID"+String(servoID[targetID])+"サーボの動作テスト";
          fontDump(xps, yps, (char*)s.c_str(), fntsz, TFT_CYAN);
          xps=50; yps=yps+mrgn;fntsz=20; mrgn=fntsz+10;
          fontDump(xps, yps, "-->", fntsz, TFT_YELLOW);
          break;
      }
      break;

    case viewChgID://ID変更
      xps=30; yps=70;fntsz=20; mrgn=fntsz+10;
      fontDump(xps, yps, "サーボIDの変更", fntsz, TFT_WHITE);
      yps=yps+mrgn;fntsz=16; mrgn=fntsz+6;
      switch(stepmode) {
        case selID:
          fontDump(xps, yps, "どのサーボを変更しますか？", fntsz, TFT_CYAN);
          xps=50; vyps=yps=yps+mrgn;vfntsz=fntsz=20; mrgn=fntsz+10;
          fontDump(xps, yps, "ＩＤ：", fntsz, TFT_YELLOW);
          drawIDValue();
          break;
        case selToID:
          fontDump(xps, yps, "いくつに変更しますか？", fntsz, TFT_CYAN);
          xps=50; vyps=yps=yps+mrgn;vfntsz=fntsz=20; mrgn=fntsz+10;    
          fontDump(xps, yps, "ＩＤ：", fntsz, TFT_YELLOW);
          drawToIDValue();
          break;
        case selChgID:
          s="ID"+String(servoID[targetID])+"を "+String(changeToID)+"に変更";
          fontDump(xps, yps, (char*)s.c_str(), fntsz, TFT_CYAN);
          vyps=yps+mrgn;
          break;
      }
      break;

    case viewInfo://各種設定
      xps=30; yps=70;fntsz=20; mrgn=fntsz+10;
      switch(stepmode) {
        case selID:
          fontDump(xps, yps, "各種設定値の表示と変更", fntsz, TFT_WHITE);
          yps=yps+mrgn;fntsz=16; mrgn=fntsz+6;
          fontDump(xps, yps, "どのサーボを表示しますか？", fntsz, TFT_CYAN);
          xps=50; vyps=yps=yps+mrgn;vfntsz=fntsz=20; mrgn=fntsz+10;
          fontDump(xps, yps, "ＩＤ：", fntsz, TFT_YELLOW);
          drawIDValue();
          break;
        case selSet:
          s="ID"+String(servoID[targetID])+"の設定一覧と変更";
          fontDump(xps, yps, (char*)s.c_str(), fntsz, TFT_CYAN);
          vyps=yps+mrgn-5;
          drawServoInfo();
          break;
        case selValue:
          fontDump(xps, yps, "いくつに変更しますか？", fntsz, TFT_CYAN);
          xps=50; vyps=yps=yps+mrgn;vfntsz=fntsz=20; mrgn=fntsz+10;
          s=ilt[lstX][lstY].title;//+ilt[lstX][lstY].disp;
          fontDump(xps, yps, (char*)s.c_str(), fntsz, TFT_YELLOW);
          drawChgSetValue();
          drawInfoDesc(lstX,lstY);
          break;
        case selChgVlu:
          yps=85;fntsz=20; mrgn=fntsz+10;
          fontDump(xps, yps,        "A:変更しないで一覧に戻る", fntsz, TFT_YELLOW);
          fontDump(xps, yps+mrgn,   "B:変更して一覧に戻る", fntsz, TFT_CYAN);
          fontDump(xps, yps+mrgn*2, "C:変更保存後トップに戻る", fntsz, TFT_GREEN);
          break;
        case selChgVlu2:
          s="設定を変更します...";
          fontDump(xps, yps, (char*)s.c_str(), fntsz, TFT_CYAN);
          vyps=yps+mrgn;
          break;
      }
      break;
      
  }
  SDfonts.close(); // フォントのクローズ
  
  drawButtonName();
}


void drawTitleName() {
  M5.Lcd.fillRect(0, 0, 320, 60, 0);
  
  //M5Stack IJUploader
  SDfonts.open(); // フォントのオープン
  fontDump(20, 5, "RSTester", 24, TFT_YELLOW, false);
  fontDump(20, 30, "RS30x系サーボ設定表示・変更", 16, TFT_WHITE, false);
  fontDump(20, 47, "by Micono v1.0.1", 12, TFT_GREEN, false); 
  SDfonts.close(); // フォントのクローズ
  
}

void returnToTop() {
  M5.Lcd.fillRect(0, 60, 320, 180, 0);
  viewType=viewMain;
  btnType=btnABC;
  drawMenuName();
  checkConnectServo(1);//3//drawConnectServo();
  working=0;
}
/*
 * 動作テスト
 */

void drawServoMove(String s) {
  int xps=125;
  M5.Lcd.fillRect(xps, vyps, 320, vfntsz, 0);
  fontDump(xps, vyps, (char*)s.c_str(), vfntsz, TFT_YELLOW);
}

void drawIDValue() {
  drawServoMove(String(servoID[targetID]));
}

void drawAngleValue() {
  String s=String(angleID[targetAgl]);
  s="±"+s;
  drawServoMove(s);
}

void drawToIDValue() {
  drawServoMove(String(changeToID));
}

void drawChgSetValue() {
  int x=lstX,y=lstY;
  drawServoMove(String(ilt[x][y].value));
  double v,w;
  v=ilt[x][y].value; v=v*ilt[x][y].unit;
  String s="";
  
  switch(chgSetID) {
    case chgModel:
      s=String(ilt[x][y].value,HEX);
      break;
    case chgBaud:
      s=baudrate[ilt[x][y].value]+ilt[x][y].unin;
      break;
    case chgOn:
      if(ilt[x][y].value==0) s="トルクオフ";
      else s="トルクオン";
      break;
    case chgRev:
      if(ilt[x][y].value==0) s="正転";
      else s="反転";
      break;
    case chgRet:
      v=ilt[x][y].value*18+100;
      s=String(v)+ilt[x][y].unin;
      break;
    case chgPanch:
      w=ilt[x][y].value; w=w*0.01;
      s=String(w)+" / "+String(v)+ilt[x][y].unin;
      break;
    default:
      s=String(v)+ilt[x][y].unin;
      break;
  }
  int xps=125;
  int yps=vyps+vfntsz+10;
  int fntsz=14;
  M5.Lcd.fillRect(0, yps, 320, fntsz, 0);
  fontDumpCenter(160, yps, (char*)s.c_str(), (uint8_t)s.length(), fntsz, TFT_WHITE,true);
}

void doServoTest() {
  working=1;

  int id=servoID[targetID];
  int agl=angleID[targetAgl];
  String s="トルクON";
  drawServoMove(s);
  trqueONOFF(id,1);//ON

  s=String(agl);
  drawServoMove(s); rotateServo(id,agl*10);
  drawServoMove("-"+s); rotateServo(id,-agl*10);
  drawServoMove(s); rotateServo(id,agl*10);
  drawServoMove("0"); rotateServo(id,0);
  
  s="トルクOFF";
  drawServoMove(s);
  trqueONOFF(id,0);//OFF
  
  s="テスト終了...";
  drawServoMove(s);
  delay(1000);
  
  returnToTop();
}

void drawChangeInfo(String s, uint32_t c) {
  int xps=50;
  M5.Lcd.fillRect(0, vyps, 320, 240-vyps, 0);
  fontDump(xps, vyps, (char*)s.c_str(), vfntsz, c);
  if(c==TFT_GREEN) vyps=vyps+vmrgn;
}

void doWriteReset(int id) {
  
  String s="ＲＯＭに書込み";
  drawChangeInfo(s,TFT_YELLOW); //delay(2000);
  writeRom(id);
  drawChangeInfo(s,TFT_GREEN);
  
  s="ＲＯＭをリセット";
  drawChangeInfo(s,TFT_YELLOW); //delay(2000);
  restertRom(id);
  drawChangeInfo(s,TFT_GREEN);

  returnToTop();

  checkConnectServo(1);//3
}

void doChangeServoID() {
  working=1;

  vfntsz=16;
  vmrgn=fntsz+6;
  
  String s="トルクOFF";
  int id=servoID[targetID];
  drawChangeInfo(s,TFT_YELLOW); //delay(2000);
  trqueONOFF(id,0);//OFF
  drawChangeInfo(s,TFT_GREEN);

  s="ＩＤ変更";
  drawChangeInfo(s,TFT_YELLOW); //delay(2000);
  changeServoID(id,changeToID);
  drawChangeInfo(s,TFT_GREEN);

  doWriteReset(id);
}

void doWriteResetOnly() {
  working=1;
  vfntsz=16;
  vmrgn=fntsz+6;
  doWriteReset(servoID[targetID]);
}

void doChangeValue(int id, int t, bool wr) {
  working=1;

  vfntsz=16;
  vmrgn=fntsz+6;
  
  String s="トルクOFF";
  //int id=servoID[targetID];
  drawChangeInfo(s,TFT_YELLOW); //delay(2000);
  trqueONOFF(id,0);//OFF
  
  drawChangeInfo(s,TFT_GREEN);
  switch(t) {
    case chgMgnR: s="右マージン変更"; break;
    case chgMgnL: s="左マージン変更"; break;
    case chgSlpR: s="右スロープ変更"; break;
    case chgSlpL: s="左スロープ変更"; break;
    case chgPanch: s="パンチ変更"; break;
    case chgRev: s="回転方向を変更"; break;
    case chgRet: s="返信ディレイ変更"; break;
    case chgRLmt: s="右最大回転変更"; break;
    case chgLLmt: s="左最大回転変更"; break;
  }
  drawChangeInfo(s,TFT_YELLOW); //delay(2000);

  int n=ilt[lstX][lstY].value;
  switch(t) {
    case chgMgnR: compMagin(id,0,n); break;
    case chgMgnL: compMagin(id,1,n); break;
    case chgSlpR: compSlop(id,0,n); break;
    case chgSlpL: compSlop(id,1,n); break;
    case chgPanch: panch(id,n); break;
    case chgRev: servoReverse(id,n); break;
    case chgRet: returnDelay(id,n); break;
    case chgRLmt: maxRotationAngle(id,0,n); break;
    case chgLLmt: maxRotationAngle(id,1,n); break;
  }
  drawChangeInfo(s,TFT_GREEN);  
  
  if(wr) doWriteReset(id);
  delay(1000);

  working=0;
}

void checkConnectServo(int n) {
  drawConnectServo(true);
  
  for(int j=0; j<n; j++) {
    servoNum=0;
    for(int i=1; i<40; i++) {
      if(requestConnectServo(i)) {
        servoID[servoNum]=i;
        servoNum=servoNum+1;
      }
    }
    servoID[servoNum]=255;

    //delay(200);
  }

 drawConnectServo(false);

}


/*************************************************
 * setup
 */
 
void setup() {
  // put your setup code here, to run once:
  M5.begin();
  //Wire.begin();
  if (digitalRead(BUTTON_A_PIN) == 0) {
    Serial.println("Will Load menu binary");
    updateFromFS(SD);
    ESP.restart();
  }
  
  //Serial.begin(115200);
  #ifdef ARDUINO_ODROID_ESP32
  //#1 GND
  //14 IO12 EXT-Pin #3 : EMAC_TXD3
  //23 IO15 EXT-Pin #4                            : EMAC_RXD3
  //#10 POWER
  Serial2.begin(115200, SERIAL_8N1, 15, 12);
  #else
  Serial2.begin(115200, SERIAL_8N1, 16, 17);
  #endif
  delay(500);

  M5.Lcd.setTextSize(2);
  M5.Lcd.setBrightness(100);
  M5.Lcd.fillScreen(TFT_BLACK);
  
  SDfonts.init(SD_PN);
  Serial.println(F("sdfonts liblary"));
  drawTitleName();
  drawMenuName();

  checkConnectServo(1);

  //情報リスト初期化
  initInfoList();

  //For Faces
  #ifdef useFACES
  Wire.begin();
  pinMode(KEYBOARD_INT, INPUT_PULLUP);
  Serial.println("Faces: Enabled");//M5.Lcd.println("Faces: Enabled");
  #endif
  //M5.Speaker.beep();
}

/*************************************************
 * loop
 */

unsigned long noOpr=millis();
unsigned long initPress=0;//millis();
bool enableLongPress=false;

//#ifdef ARDUINO_ODROID_ESP32
int joyXisPressed=0;
int joyYisPressed=0;
int joyXwasPressed=0;
int joyYwasPressed=0;
//#endif
int crsXdir=1;
int crsYdir=1;
int selBtn=-1;

void crossButtonDirection() {
  if(joyXisPressed==2) crsXdir=-1;//left
  if(joyYisPressed==2) crsYdir=-1;//Up
}

void loop() {
  // put your main code here, to run repeatedly:
  M5.update();

  //#ifdef ARDUINO_ODROID_ESP32
  joyXisPressed=0;//C button
  joyYisPressed=0;//A button
  crsXdir=1;//Direction
  crsYdir=1;//Direction
  //#endif

  if(working>0) {
    noOpr=millis();
  } else {
    selBtn=-1;
    if(enableLongPress) {
      if(initPress==0) initPress=millis();
      else if(millis()>initPress+500) {
        #ifdef ARDUINO_ODROID_ESP32
        joyXisPressed=M5.JOY_X.isAxisPressed();
        joyYisPressed=M5.JOY_Y.isAxisPressed();
        if(joyYisPressed>0) selBtn=0;
        if(joyXisPressed>0) selBtn=2;
        #else
        if(M5.BtnA.isPressed()) selBtn=0;
        if(M5.BtnC.isPressed()) selBtn=2;
        #endif
      }
    }
    #ifdef ARDUINO_ODROID_ESP32
    //Start button
    if(M5.BtnStart.wasPressed()) {
      returnToTop();
      return;
    }
    #endif
    #ifdef ARDUINO_ODROID_ESP32
    joyYwasPressed=M5.JOY_Y.wasAxisPressed();
    if(joyYwasPressed>0||M5.BtnA.wasPressed()) {
      joyYisPressed=joyYwasPressed;
    #else
    if(M5.BtnA.wasPressed()) {
    #endif
      selBtn=0;initPress=0;
    }
    if(M5.BtnB.wasPressed()) {
      selBtn=1;initPress=0;
    }
    #ifdef ARDUINO_ODROID_ESP32
    joyXwasPressed=M5.JOY_X.wasAxisPressed();
    if(joyXwasPressed>0||M5.BtnC.wasPressed()) {
      joyXisPressed=joyXwasPressed;
    #else
    if(M5.BtnC.wasPressed()) {
    #endif
      selBtn=2;initPress=0;
    }

    #ifdef useFACES
    if (digitalRead(KEYBOARD_INT) == LOW) {
      Wire.requestFrom(KEYBOARD_I2C_ADDR, 1);  // request 1 byte from keyboard
      while (Wire.available()) {
        doFacesKey(Wire.read()); // receive a byte as character
      }
    }
    #endif //useFaces
    
    if(selBtn>-1) {
      noOpr=millis();
      #ifdef ARDUINO_ODROID_ESP32
      crossButtonDirection();
      #endif
      switch(viewType) {
        case viewMain:
          changeView(selBtn);
          break;
        default:
          switch(selBtn) {
            case 0: doBtnAC(1); break;//A
            case 1: doBtnB(); break;//B
            case 2: doBtnAC(-1); break;//C
          }
          break;
      }
    }
    #ifndef ARDUINO_ODROID_ESP32
    else {
      if (300000<millis()-noOpr) {
        Serial.println( "Power OFF" );
        M5.powerOFF();
      }      
    }
    #endif
  }
  
  delay(1);
}

/*************************************************
 * Select Buttun
 */

void changeView(int n) {
  viewType=n+1;
  stepmode=selID;
  btnType=btnDwnOKUp;
  targetID=0;
  drawMenuName();
  checkConnectServo(1);
}

void doBtnAC(int n) {
  switch(viewType) {
    case viewMove:
      switch(stepmode) {
        case selID:
          targetID=targetID-n*crsXdir*crsYdir;
          if(targetID>servoNum) {
            targetID=0;
          } else if(targetID<0) {
            targetID=servoNum;
          }
          drawIDValue();
          break;
        case selAgl:
          targetAgl=targetAgl-n*crsXdir*crsYdir;
          if(targetAgl>angleNum) {
            targetAgl=0;
          } else if(targetAgl<0) {
            targetAgl=angleNum;
          }
          drawAngleValue();
          break;
      }
      break;

    case viewChgID:
      switch(stepmode) {
        case selID:
          targetID=targetID-n*crsXdir*crsYdir;
          if(targetID>servoNum) {
            targetID=0;
          } else if(targetID<0) {
            targetID=servoNum;
          }
          drawIDValue();
          break;
        case selToID:
          changeToID=changeToID-n*crsXdir*crsYdir;
          if(changeToID==40) {
            changeToID=255;
          } else if(changeToID==254) {
            changeToID=39;
          } else if(changeToID>255) {
            changeToID=1;
          } else if(changeToID<1) {
            changeToID=255;
          }
          drawToIDValue();
          break;
      }
      break;

    case viewInfo:
      int x=lstX,y=lstY;
      switch(stepmode) {
        case selID:
          targetID=targetID-n*crsXdir*crsYdir;
          if(targetID>servoNum) {
            targetID=0;
          } else if(targetID<0) {
            targetID=servoNum;
          }
          drawIDValue();
          break;
        case selSet:
          if(n>0) {
            y=lstY+1*crsYdir;
            if(y>=7) y=0;
            if(y<0) y=6;
          } else {
            x=lstX+1*crsXdir;
            if(x>=3) x=0;
            if(x<0) x=2;
          }
          drawInfoList(x, y, true);
          break;
        case selValue:
          if(ilt[x][y].type==0) return;
          ilt[x][y].value-=n*crsXdir*crsYdir;
          if(ilt[x][y].value<ilt[x][y].minv) ilt[x][y].value=ilt[x][y].maxv;
          if(ilt[x][y].value>ilt[x][y].maxv) ilt[x][y].value=ilt[x][y].minv;
          drawChgSetValue();
          break;
        case selChgVlu:
          enableLongPress=true;
          if(n>0) {//A 変更しないで戻る
            M5.Lcd.fillRect(0, 60, 320, 160, 0);
            stepmode=selID;
            doBtnB();
          } else { //C 変更保存後トップに戻る
            //変更,保存
            stepmode=selChgVlu2;
            drawMenuName();//M5.Lcd.fillRect(0, 60, 320, 160, 0);
            doChangeValue(servoID[targetID],chgSetID,true);
            returnToTop();
          }
          break;
      }
      break;
  }
}

void doBtnB() {
  switch(viewType) {
    case viewMove:
      switch(stepmode) {
        case selID:
          stepmode=selAgl;
          drawMenuName();
          break;
        case selAgl:
          stepmode=selMove;
          btnType=btnNone;
          drawMenuName();
          doServoTest();
          break;
      }
      break;

    case viewChgID:
      switch(stepmode) {
        case selID:
          stepmode=selToID;
          drawMenuName();
          break;
        case selToID:
          stepmode=selChgID;
          btnType=btnNone;
          drawMenuName();
          doChangeServoID();
          break;
      }
      break;

    case viewInfo:
      switch(stepmode) {
        case selID:
          stepmode=selSet;
          loadServoMem(servoID[targetID],kLoadROM,(uint8_t*)&rsrom);
          loadServoMem(servoID[targetID],kLoadRAM,(uint8_t*)&rsram);
          setServoInfo();
          btnType=btnDwnOKUp;
          drawMenuName();
          break;
        case selSet:
          switch(ilt[lstX][lstY].type) {
            case 0: btnType=btnNonBckNon; break;
            case 3: doWriteResetOnly();
            case 2: returnToTop(); return;
            default:
              enableLongPress=true;
              btnType=btnDwnOKUp;
              break;
          }
          M5.Lcd.fillRect(0, 60, 320, 160, 0);
          chgSetID=lstX*7+lstY;
          stepmode=selValue;
          drawMenuName();
          break;
        case selValue:
          M5.Lcd.fillRect(0, 60, 320, 160, 0);
          switch(ilt[lstX][lstY].type) {
            case 0: //もどる
              stepmode=selID;
              doBtnB();
              return;
          }
          btnType=btnABC;
          stepmode=selChgVlu;
          drawMenuName();
          break;
        case selChgVlu:
          enableLongPress=true;
          stepmode=selChgVlu2;
          drawMenuName();//M5.Lcd.fillRect(0, 60, 320, 160, 0);
          doChangeValue(servoID[targetID],chgSetID,false);
          stepmode=selID;
          doBtnB();
          break;
      }
      break;

  }
}

#ifndef ARDUINO_ODROID_ESP32
#ifdef useFACES

void doFacesKey(int key_val) {
  Serial.println(key_val);
  if (key_val != 0) {
    if (key_val >= 0x30 && key_val <= 0x39) {
      /*
      if (startkey) {
        voiceID = (voiceID + 1) * 10 + key_val - 0x30 - 1;
      } else {
        startkey = true;
        voiceID = key_val - 0x30 - 1;
      }
      checkVoiceID();
      drawListVoiceID();
      return;
      */
    } else {
      doTenKey(key_val);
      return;
    }
  }
}

void doTenKey(int key_val) {
  switch (key_val) {
    case 239://A
      selBtn=0;initPress=0;
      break;
      
    case 223://B
      selBtn=1;initPress=0;
      break;

    case 253://Down
      joyYisPressed=1;
      selBtn=0;initPress=0;
      break;

    case 254://Up
      joyYisPressed=2;
      selBtn=0;initPress=0;
      break;

    case 247://Right
      joyXisPressed=1;
      selBtn=2;initPress=0;
      break;

    case 251://Left
      joyXisPressed=2;
      selBtn=2;initPress=0;
      break;

    case 127://Start
      returnToTop();
      break;

    /*
    case 191://Select
      break;
    case '=':
      break;
    case '+':
      break;
    case '-':
      break;
    case '*':
      break;
    case '/':
      break;
    default:
      break;
    */
  }

  //startkey = false;
}

#endif //useFACES
#endif //ARDUINO_ODROID_ESP32
