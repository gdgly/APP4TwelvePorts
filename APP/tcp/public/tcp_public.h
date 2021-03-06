/*
 * tcp_public.h
 *
 *  Created on: 2021年1月5日
 *      Author: HHS007
 */

#ifndef _TCP_PUBLIC_H_
#define _TCP_PUBLIC_H_

#include "sys.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stdlib.h>
#include "delay.h"
#include "malloc.h"
#include "STMFlash.h"
#include "task.h"
#include "motor.h"

#define TCP_MAX_LEN 640		  //最大接收缓存字节数
#define BASE64_BUF_LEN 512

struct STRUCT_USART_Fram  //定义一个全局串口数据帧的处理结构体
{
	unsigned char RxBuf[TCP_MAX_LEN];
	unsigned char TxBuf[BASE64_BUF_LEN];
//	unsigned char DeData[BASE64_BUF_LEN];
//	unsigned char EnData[BASE64_BUF_LEN];
//	char *base64Str;
//	unsigned char ServerData[BASE64_BUF_LEN];
//	unsigned char *Server_Command[2];
	__IO u8 Online;
//	__IO u8 linkedClosed;
//	__IO u8 allowProcessServerData;
//	__IO u8 init;
//	__IO u8 registerSuccess;
	__IO u8 firstStatuHeartNotSucc;
//	__IO u8 serverStatuCnt;
	__IO u8 allowHeart;
//	__IO u8 forceHeart_32;
//	__IO u8 forceHeart_90;
//	__IO u8 NowInAT;
	__IO u8 AT_test_OK;
	vu8 DMA_Tx_Busy;
	vu16 AccessLen;
//	union
//	{
//		__IO u16 InfAll;
//		struct
//		{
//			__IO u16 Length :15;                               // 14:0
//			__IO u16 FinishFlag :1;                                // 15
//		} InfBit;
//	};
};

extern struct STRUCT_USART_Fram F4G_Fram;
extern struct STRUCT_USART_Fram WIFI_Fram;
extern struct STRUCT_USART_Fram USART1_Fram;

struct RegisterFram	  //定义一个全局串口数据帧的处理结构体
{
	char key[17];
	u8 motor_TCtime;
	u8 motor_HTtime;
	char ip[50];
	u8 needConfirmParams;
	u8 heartTime;
	u8 statuHeartTime;
	char port[6];
};
extern struct RegisterFram RegisterParams;
//===================枚举========================================================
typedef enum
{
	STA, AP, STA_AP
} ENUM_Net_ModeTypeDef;

typedef enum
{
	enumTCP, enumUDP
} ENUM_NetPro_TypeDef;

typedef enum
{
	Multiple_ID_0 = 0,
	Multiple_ID_1 = 1,
	Multiple_ID_2 = 2,
	Multiple_ID_3 = 3,
	Multiple_ID_4 = 4,
	Single_ID_0 = 5
} ENUM_ID_NO_TypeDef;

typedef enum
{
	In4G = 1, InWifi = 2, InUsart1 = 3
} ENUM_Internet_TypeDef;

typedef enum
{
	DOWN_RegiseterSucc = 31,
	DOWN_RecivedAllPortsSTA = 33,
	DOWN_SystemPopupPowerbank = 34,
	DOWN_RecivedSystemPopupSTA = 36,
	DOWN_RecivedPowerbankSTAChanged = 38,
	DOWN_RecivedStatuHeart = 91,
	DOWN_ForceHeart = 92,
	DOWN_RecivedForceHeart = 94,
	DOWN_DeviceReset = 60,
	DOWN_SetWifiHeart = 62,
	DOWN_SetWifiSsidAndPwd = 66,
	DOWN_OrderPopupPowerbank = 40,
	DOWN_RecivedOrderPopupPowerbank = 42,
	DOWN_VoiceBroadcast = 71,
	DOWN_PopupAllPowerbanks = 99,
	DOWN_IgnoreLock = 77,
	DOWN_SetID = 12,
	DOWN_RemoteCtrMotor = 73
} ENUM_tcpDOWN_TypeDef;

typedef enum
{
	UP_Regiser = 30,
	UP_AllPortsSTA = 32,
	UP_SystemPopupSTA = 35,
	UP_PowerbankSTAChanged = 37,
	UP_StatuHeart = 90,
	UP_ForceHeart = 93,
	UP_DeviceRest = 61,
	UP_SetWifiHeart = 63,
	UP_SetWifiSsidAndPwd = 67,
	UP_OrderPopupPowerbank = 41,
} ENUM_tcpUP_TypeDef;

void AnalyzeServerParams(void);
void _USART_Printf(ENUM_Internet_TypeDef net, const char *fmt, ...);
bool Send_AT_Cmd(ENUM_Internet_TypeDef internet, char *cmd, char *ack1,
		char *ack2, u32 time, u16 retry, FunctionalState printState);
bool AT_Test(ENUM_Internet_TypeDef internet);
void DEBUG(const char *fmt, ...);
void getWifiSsidAndPwd(char *datas);
void getRegisterStr(char *strBuf, int len, ENUM_tcpUP_TypeDef upCMD,
		char moduleType);
void getRequestStrWithoutParam(char *strBuf, int len, ENUM_tcpUP_TypeDef upCMD);
void getPowerbankSTAStr(char *strBuf, int len, ENUM_tcpUP_TypeDef upCMD,
		u8 rssi, int portNum, char *portSTAStr, ...);
void getPowerbankSTAStrWithoutRSSI(char *strBuf, int len,
		ENUM_tcpUP_TypeDef upCMD, int portNum, char *portSTAStr, ...);
void ProcessServerCmd(ENUM_Internet_TypeDef internet,
		ENUM_tcpDOWN_TypeDef DownCMD, char *SData);
void systemPopup(ENUM_Internet_TypeDef internet, char *data);
void orderPopup(ENUM_Internet_TypeDef internet, char *data);
void modifyLockSTA(char *data);
void commonHeart(ENUM_Internet_TypeDef internet);
void forceHeart(ENUM_Internet_TypeDef internet, ENUM_tcpUP_TypeDef upCmd,
		FunctionalState state);
void responseReset(ENUM_Internet_TypeDef internet);
void reportPortStatuChanged(ENUM_Internet_TypeDef internet, u8 port);
void request4Register(ENUM_Internet_TypeDef internet);
void setWifiSsidAndPwd(ENUM_Internet_TypeDef internet, char *data);
void getRegisterParams(ENUM_Internet_TypeDef internet, char *data);
void setMotorRun(char *data);

#endif /* _TCP_PUBLIC_H_ */
