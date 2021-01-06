/*
 * STMFlash.c
 *
 *  Created on: 2021年1月5日
 *      Author: HHS007
 */
#include "STMFlash.h"

struct AboutFlash_typeDef MyFlashParams;

/**
 * 读取指定位置的字
 * @addr 地址
 * @return 对应数据
 */
u32 STMFLASH_ReadWord(u32 addr)
{
	return *(vu32*) addr;
}
/**
 * 获取某个地址所在的flash扇区
 * @addr flash地址
 * @return 0~11,即addr所在的扇区
 */
uint16_t STMFLASH_GetFlashSector(u32 addr)
{
	if (addr < ADDR_FLASH_SECTOR_1)
		return FLASH_Sector_0;
	else if (addr < ADDR_FLASH_SECTOR_2)
		return FLASH_Sector_1;
	else if (addr < ADDR_FLASH_SECTOR_3)
		return FLASH_Sector_2;
	else if (addr < ADDR_FLASH_SECTOR_4)
		return FLASH_Sector_3;
	else if (addr < ADDR_FLASH_SECTOR_5)
		return FLASH_Sector_4;
	else if (addr < ADDR_FLASH_SECTOR_6)
		return FLASH_Sector_5;
	else if (addr < ADDR_FLASH_SECTOR_7)
		return FLASH_Sector_6;
	else if (addr < ADDR_FLASH_SECTOR_8)
		return FLASH_Sector_7;
	else if (addr < ADDR_FLASH_SECTOR_9)
		return FLASH_Sector_8;
	else if (addr < ADDR_FLASH_SECTOR_10)
		return FLASH_Sector_9;
	else if (addr < ADDR_FLASH_SECTOR_11)
		return FLASH_Sector_10;
	return FLASH_Sector_11;
}
/**
 * 从指定地址开始写入指定长度的数据
 * @WriteAddr 起始地址(此地址必须为4的倍数!!)
 * @pBuffer 数据指针
 * @NumToWrite 字(32位)数(就是要写入的32位数据的个数.)
 * @return 无
 */
void STMFLASH_Write(u32 WriteAddr, u32 *pBuffer, u32 NumToWrite)
{
	FLASH_Status status = FLASH_COMPLETE;
	u32 addrx = 0;
	u32 endaddr = 0;
	if (WriteAddr < STM32_FLASH_BASE || WriteAddr % 4)
		return;	//非法地址
	FLASH_Unlock();	//解锁
	FLASH_DataCacheCmd(DISABLE); //FLASH擦除期间,必须禁止数据缓存

	addrx = WriteAddr;				//写入的起始地址
	endaddr = WriteAddr + NumToWrite * 4;	//写入的结束地址
	if (addrx < 0X1FFF0000)			//只有主存储区,才需要执行擦除操作!!
	{
		while (addrx < endaddr)		//扫清一切障碍.(对非FFFFFFFF的地方,先擦除)
		{
			if (STMFLASH_ReadWord(addrx) != 0XFFFFFFFF)	//有非0XFFFFFFFF的地方,要擦除这个扇区
			{
				status = FLASH_EraseSector(STMFLASH_GetFlashSector(addrx),
				VoltageRange_3);		//VCC=2.7~3.6V之间!!
				if (status != FLASH_COMPLETE)
					break;	//发生错误了
			}
			else
				addrx += 4;
		}

	}
	if (status == FLASH_COMPLETE)
	{
		while (WriteAddr < endaddr)	//写数据
		{
			if (FLASH_ProgramWord(WriteAddr, *pBuffer) != FLASH_COMPLETE)//写入数据
			{
				break;	//写入异常
			}
			WriteAddr += 4;
			pBuffer++;
		}
	}
	FLASH_DataCacheCmd(ENABLE);	//FLASH擦除结束,开启数据缓存
	FLASH_Lock();	//上锁
}
/**
 * 从指定地址开始读出指定长度的数据
 * @ReadAddr:起始地址
 * @pBuffer:数据指针
 * @NumToRead:字(4位)数
 */
void STMFLASH_Read(u32 ReadAddr, u32 *pBuffer, u32 NumToRead)
{
	u32 i;
	for (i = 0; i < NumToRead; i++)
	{
		pBuffer[i] = STMFLASH_ReadWord(ReadAddr);	//读取4个字节.
		ReadAddr += 4;	//偏移4个字节.
	}
}
/**
 * 将参数从flash中读出来
 */
void STMFlash_Init(void)
{
	//填充参数
	STMFLASH_Read(DEVICE_ID_ADDR, (u32 *) &MyFlashParams.DeviceID[0], 2);
	STMFLASH_Read(VERSION_ADDR, (u32 *) &MyFlashParams.Version[0], 5);
	STMFLASH_Read(IGNORE_LOCK_ADDR, (u32 *) &MyFlashParams.IgnoreLock[0], 3);
	STMFLASH_Read(APP_SERVER_ADDR, (u32 *) &MyFlashParams.ServerParams[0], 25);
	STMFLASH_Read(WIFI_SSID_ADDR, (u32 *) &MyFlashParams.WifiSSID[0], 25);
	STMFLASH_Read(WIFI_PWD_ADDR, (u32 *) &MyFlashParams.WifiPWD[0], 2);
	STMFLASH_Read(WIFI_FLAG_ADDR, (u32 *) &MyFlashParams.WifiFlag, 1);
	STMFLASH_Read(IAP_FLAG_ADDR, (u32 *) &MyFlashParams.IAPFlag, 1);
	//添加结束符
	MyFlashParams.DeviceID[8] = '\0';
	MyFlashParams.Version[20] = '\0';
	MyFlashParams.ServerParams[100] = '\0';
	MyFlashParams.WifiSSID[100] = '\0';
	MyFlashParams.WifiPWD[100] = '\0';
}
/**
 * 写入所有参数
 */
void MyFlash_Write(struct AboutFlash_typeDef *def)
{
	STMFLASH_Write(DEVICE_ID_ADDR, (u32 *) &def->DeviceID[0], 2);
	STMFLASH_Write(VERSION_ADDR, (u32 *) &def->Version[0], 5);
	STMFLASH_Write(APP_SERVER_ADDR, (u32 *) &def->ServerParams[0], 25);
	STMFLASH_Write(IGNORE_LOCK_ADDR, (u32 *) &def->IgnoreLock[0], 3);
	STMFLASH_Write(WIFI_SSID_ADDR, (u32 *) &def->WifiSSID[0], 25);
	STMFLASH_Write(WIFI_PWD_ADDR, (u32 *) &def->WifiPWD[0], 25);
	STMFLASH_Write(WIFI_FLAG_ADDR, (u32 *) &def->WifiFlag, 1);
	STMFLASH_Write(IAP_FLAG_ADDR, (u32 *) &def->IAPFlag, 1);
}
