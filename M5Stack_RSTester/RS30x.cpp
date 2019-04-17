/*
 * RS30x.cpp
 * 
 * 
 */

#include "RS30x.h"
//#include "DispJPFont.h"
//int cmsp[4];

void trqueONOFF(int id, int n) {
  byte trq[]={0xFA, 0xAF, id, 0x00, 0x24, 0x01, 0x01, 0x01, 0x25};
  if(n>0) {
    //トルクON
    //CHR$(T,U,V,0,X,1,1,1,#25^V)
    trq[5]=1;
    trq[8]=0x25^trq[2];
  } else {
    //CHR$(T,U,V,0,X,1,1,0,#24^V)
    trq[5]=0;
    trq[8]=0x24^trq[2];
  }
  Serial2.write(trq,9);
  Serial2.flush();
  delay(500);
}

void rotateServo(int id, int n) {
  byte rot[]={0xFA, 0xAF, id, 0x00, 0x1E, 0x02, 0x01, 0x00, 0x00, 0xE2};
  //CHR$(T,U,V,0,Y,2,1,A,B,C)
  //A=Q&#FF:B=(Q>>8)&#FF:C=((#1D^V)^A)^B
  rot[7]=n&0xFF;
  rot[8]=(n>>8)&0xFF;
  rot[9]=((0x1D^rot[2])^rot[7])^rot[8];
  Serial2.write(rot,10);
  Serial2.flush();
  delay(1500);
}

/************************************************
 * Chenge settings
 */

//サーボID
void changeServoID(int id, int n) {
  byte chg[]={0xFA, 0xAF, id, 0x00, 0x04, 0x01, 0x01, n, (4^id)^n};
  //?CHR$(T,U,S,0,4,1,1,V,(4^S)^V)
  Serial2.write(chg,9);
  Serial2.flush();
  delay(1000);
}

//回転方向
void servoReverse(int id, int n) {
  byte rev[]={0xFA, 0xAF, id, 0x00, 0x05, 0x01, 0x01, n, (5^n)^id};
  //?CHR$(T,U,V,0,5,1,1,X,(5^X)^V)
  Serial2.write(rev,9);
  Serial2.flush();
  delay(1000);  
}

//リターンパケットを要求された時の返信ディレイ時間
void returnDelay(int id, int n) {
  byte ret[]={0xFA, 0xAF, id, 0x00, 0x07, 0x01, 0x01, n, (7^n)^id};
  //?CHR$(T,U,V,0,7,1,1,X,(7^X)^V)
  Serial2.write(ret,9);
  Serial2.flush();
  delay(1000);  
}

//最大動作角度
void maxRotationAngle(int id, int cw, int n) {
  byte rot[]={0xFA, 0xAF, id, 0x00, 0x08, 0x02, 0x01, 0x00, 0x00, 0x0B };
  //?CHR$(T,U,V,0,8,2,1,0,0,B)
  rot[4]=0x8+cw*2;
  rot[7]=n&0xFF;
  rot[8]=(n>>8)&0xFF;
  if(cw==0) {
    rot[9]=((0x0B^rot[2])^rot[7])^rot[8];
  } else {
    rot[9]=((0x09^rot[2])^rot[7])^rot[8];
  }
  Serial2.write(rot,10);
  Serial2.flush();
  delay(1000);
}

void compMagin(int id, int cw, int n) {
  byte mgn[]={0xFA, 0xAF, id, 0x00, 0x18, 0x01, 0x01, 0x01, 0x18};
  //CHR$(T,U,V,0,A,1,1,X,(A^X)^V)
  mgn[4]=0x18+cw;
  mgn[7]=n;
  mgn[8]=(mgn[4]^n)^mgn[2];
  Serial2.write(mgn,9);
  Serial2.flush();
  delay(1000);
}

void compSlop(int id, int cw, int n) {
  byte slp[]={0xFA, 0xAF, id, 0x00, 0x1A, 0x01, 0x01, 0x01, 0x1A};
  //CHR$(T,U,V,0,A,1,1,X,(A^X)^V)
  slp[4]=0x1A+cw;
  slp[7]=n;
  slp[8]=(slp[4]^n)^slp[2];
  Serial2.write(slp,9);
  Serial2.flush();
  delay(1000);
}

void panch(int id, int n) {
  byte pnc[]={0xFA, 0xAF, id, 0x00, 0x1C, 0x02, 0x01, 0x00, 0x00, 0x1F};
  //CHR$(T,U,V,0,#1C,2,1,A,B,C)
  //A=X&#FF:B=(X>>8)&#FF:C=((#1F^V)^A)^B
  pnc[7]=n&0xFF;
  pnc[8]=(n>>8)&0xFF;
  pnc[9]=((0x1F^pnc[2])^pnc[7])^pnc[8];
  Serial2.write(pnc,10);
  Serial2.flush();
  delay(1000);
}

/************************************************
 * Write & Restert
 */

void writeRom(int id) {
  byte wrm[]={0xFA, 0xAF, id, 0x40, 0xFF, 0x00, 0x00, 0xBF};
  //CHR$(T,U,V,#40,#FF,0,0,#BF^V)
  wrm[7]=0xBF^wrm[2];
  Serial2.write(wrm,8);
  Serial2.flush();
  delay(3000);
}

void restertRom(int id) {
  byte rrm[]={0xFA, 0xAF, id, 0x20, 0xFF, 0x00, 0x00, 0xDF};
  //CHR$(T,U,V,#20,#FF,0,0,#DF^V)
   rrm[7]=0xDF^rrm[2];
  Serial2.write(rrm,8);
  Serial2.flush();
  delay(3000);
}

/************************************************
 * Request setting
 */

bool requestConnectServo(int id) {
  //?CHR$(0xFA,0xAF,I,0xF,4,1,0,0xA^I);
  byte rcs[]={0xFA, 0xAF, id, 0x0F, 0x04, 0x01, 0x00, 0x0A^id};
  Serial2.write(rcs,8);
  Serial2.flush();
  delay(2);
  return searchConnectServo(id);
}

bool searchConnectServo(int n) {
  int j=0,k;
  while(Serial2.available()) {
    k=Serial2.read();
    j=j+1;
    if(j>9) return false;
    if(k==0xFD) {
      k=Serial2.read();
      if(k==0xDF) return true;
    }
  }
  return false;
}

/* 
 *  Information #1
 *  ROM   
 */

bool loadServoMem(int id, int m, uint8_t *d) {
  for(int i=0;i<30;i++) d[i]=0;
  if(id==255) return true;
  //?CHR$(#FA,#AF,V,3,0,0,1,2^V) ROM....3
  //?CHR$(#FA,#AF,V,5,0,0,1,4^V) RAM....5
  byte mem[]={0xFA, 0xAF, id, m, 0x00, 0x00, 0x01, (m-1)^id};
  Serial2.write(mem,8);
  Serial2.flush();
  delay(100);
  return loadServoData(id,d);
}

bool loadServoData(int id, uint8_t *d) {
  int i,j=0,k;
  while(Serial2.available()) {
    k=Serial2.read();
    j=j+1;
    if(j>12) return false;
    if(k==0xFD) {
      k=Serial2.read();
      if(k==0xDF) {
        for(i=1;i<=5;i++) k=Serial2.read();
        for(i=0;i<30;i++) {
          d[i]=(uint8_t)Serial2.read();
          Serial.print(d[i],HEX);
        }
        Serial.println("");
        return true;
      }
    }
  }
  return false;  
}
