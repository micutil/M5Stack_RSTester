/*
* RS30x.h
* 
*/

#ifndef __RS30x_H__
#define __RS30x_H__

#include <M5Stack.h>

//Torque ON/OFF
void trqueONOFF(int id, int n);

//Rotate of servo
void rotateServo(int id, int n);

//IDを変更
void changeServoID(int id, int n);

//回転方向を変更
void servoReverse(int id, int n);

//返信遅延時間
void returnDelay(int id, int n);

//最大回転角度
void maxRotationAngle(int id, int cw, int n);

//Compliance Margin
void compMagin(int id, int cw, int n);

//Compliance Slope
void compSlop(int id, int cw, int n);

//Panch
void panch(int id, int n);

//Write to ROM
void writeRom(int id);

//Restert of ROM
void restertRom(int id);

//Request
bool requestConnectServo(int id);
bool searchConnectServo(int n);

typedef struct rsrom {
  uint16_t  model;
  uint8_t   firmware;
  uint8_t   reserve1;
  uint8_t   id;
  uint8_t   reverse;
  uint8_t   baudrate;
  uint8_t   returndelay;
  int16_t   RLmt;
  int16_t   LLmt;
  uint8_t   reserve2[2];
  int16_t   tempLmt;
  uint8_t   reserve3[6];
  uint8_t   torqueInSilence;
  uint8_t   warmUpTime;
  uint8_t   RCmpMgn;
  uint8_t   LCmpMgn;
  uint8_t   RCmpSlp;
  uint8_t   LCmpSlp;
  uint16_t  punch;
};

typedef struct rsram {
  int16_t   gPos;
  uint16_t  gTime;
  uint8_t   returve1;
  uint8_t   torque;
  uint8_t   torqueEnable;
  uint8_t   reserve2[5];
  int16_t   pPos;
  uint16_t  pTime;
  uint16_t  Speed;
  uint16_t  pCurr;
  uint16_t  pTemp;
  uint16_t  pVolt;
  uint8_t   reserve3[6];
};

#define kLoadROM 3
#define kLoadRAM 5

bool loadServoMem(int id, int m, uint8_t *d);
bool loadServoData(int id, unsigned char *d);

#endif //__RS30x_H__
