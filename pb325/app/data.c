#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <litecore.h>
#include "para.h"
#include "alarm.h"
#include "acm.h"
#include "data.h"
#include "system.h"

//Private Defines
#define ECL_DATA_MAGIC_WORD		0x52318761

#define ECL_DATA_QUAR_BASE		(68 * 0x1000)
#define ECL_DATA_QUAR_HEADER	16
#define ECL_DATA_QUAR_MSIZE		sizeof(t_data_quarter)
#define ECL_DATA_QUAR_ALLSIZE	(ECL_DATA_QUAR_HEADER + 96 * ECL_DATA_QUAR_MSIZE)

#define ECL_DATA_DAY_BASE		(170 * 0x1000)
#define ECL_DATA_DAY_HEADER		16
#define ECL_DATA_DAY_MSIZE		sizeof(t_stat)
#define ECL_DATA_DAY_ALLSIZE	(ECL_DATA_DAY_HEADER + ECL_DATA_DAY_MSIZE)

#define ECL_DATA_MIN_BASE		(326 * 0x1000)
#define ECL_DATA_MIN_HEADER		16
#define ECL_DATA_MIN_MSIZE		sizeof(t_data_min)
#define ECL_DATA_MIN_ALLSIZE	(ECL_DATA_MIN_HEADER + 1440 * ECL_DATA_MIN_MSIZE)

#define ECL_DATA_YX_BASE		(324 * 0x1000)
#define ECL_DATA_YX_HEADER		16
#define ECL_DATA_YX_SIZE		7
#define ECL_DATA_YX_QTY			100


//Private Consts
static const uint32_t data_Invalid = GW3761_DATA_INVALID;

//Private Macros
#define LED_UDISK(x)    		gpio_Set(1, (x) ^ 1)		//低电平有效



//External Functions
void data_Clear()
{
	uint_t i, nAdr;

	for (i = 0; i < 31; i++) {
		nAdr = ECL_DATA_QUAR_BASE + i * ECL_DATA_QUAR_ALLSIZE;
		spif_Write(nAdr, &data_Invalid, 3);
		nAdr = ECL_DATA_MIN_BASE + i * ECL_DATA_MIN_ALLSIZE;
		spif_Write(nAdr, &data_Invalid, 3);
	}
	spif_Write(ECL_DATA_YX_BASE, &data_Invalid, 4);
}

void data_MinRead(const uint8_t *pTime, t_data_min *pData)
{
	uint_t nAdr;
	uint8_t aBuf[6];

	nAdr = ECL_DATA_MIN_BASE + (bcd2bin8(pTime[2]) - 1) * ECL_DATA_MIN_ALLSIZE;
	spif_Read(nAdr, aBuf, 3);
	if (memcmp(aBuf, &pTime[2], 3) == 0) {
		nAdr += (ECL_DATA_MIN_HEADER + (bcd2bin8(pTime[1]) * 60 + bcd2bin8(pTime[0])) * ECL_DATA_MIN_MSIZE);
		spif_Read(nAdr, pData, sizeof(t_data_min));
		if (memcnt(pData->data, 0xEE, sizeof(t_data_min) - sizeof(time_t)) == 0) {
			if (memcnt(pData->data, 0xFF, sizeof(t_data_min) - sizeof(time_t)) == 0)
				return;
		}
	}
	memset(pData, GW3761_DATA_INVALID, sizeof(t_data_min));
}

void data_MinWrite(const uint8_t *pTime, t_data_min *pData)
{
	uint_t nAdr;
	uint8_t aBuf[6];

	nAdr = ECL_DATA_MIN_BASE + (bcd2bin8(pTime[2]) - 1) * ECL_DATA_MIN_ALLSIZE;
	spif_Read(nAdr, aBuf, 3);
	if (memcmp(aBuf, &pTime[2], 3)) {
		//时间错,初始化
		spif_Fill(nAdr, nAdr + ECL_DATA_MIN_ALLSIZE, GW3761_DATA_INVALID);
		spif_Write(nAdr, &pTime[2], 3);
	}
	nAdr += (ECL_DATA_MIN_HEADER + (bcd2bin8(pTime[1]) * 60 + bcd2bin8(pTime[0])) * ECL_DATA_MIN_MSIZE);
	spif_Write(nAdr, pData, sizeof(t_data_min));
	flash_Flush(0);
}

void data_QuarterRead(const uint8_t *pTime, t_data_quarter *pData)
{
	uint_t nAdr;
	uint8_t aBuf[6];

	if ((bcd2bin8(pTime[0]) % 15) == 0) {
		nAdr = ECL_DATA_QUAR_BASE + (bcd2bin8(pTime[2]) - 1) * ECL_DATA_QUAR_ALLSIZE;
		spif_Read(nAdr, aBuf, 3);
		if (memcmp(aBuf, &pTime[2], 3) == 0) {
			nAdr += (ECL_DATA_QUAR_HEADER + (bcd2bin8(pTime[1]) * 4 + bcd2bin8(pTime[0]) / 15) * ECL_DATA_QUAR_MSIZE);
			spif_Read(nAdr, pData, sizeof(t_data_quarter));
			if (memcnt(pData->data, 0xEE, sizeof(t_data_quarter) - sizeof(time_t)) == 0) {
				if (memcnt(pData->data, 0xFF, sizeof(t_data_quarter) - sizeof(time_t)) == 0)
					return;
			}
		}
	}
	memset(pData, GW3761_DATA_INVALID, sizeof(t_data_quarter));
}

void data_QuarterWrite(const uint8_t *pTime, t_data_quarter *pData)
{
	uint_t nAdr;
	uint8_t aBuf[6];

	nAdr = ECL_DATA_QUAR_BASE + (bcd2bin8(pTime[2]) - 1) * ECL_DATA_QUAR_ALLSIZE;
	spif_Read(nAdr, aBuf, 3);
	if (memcmp(aBuf, &pTime[2], 3)) {
		//时间错,初始化
		spif_Fill(nAdr, nAdr + ECL_DATA_QUAR_ALLSIZE, GW3761_DATA_INVALID);
		spif_Write(nAdr, &pTime[2], 3);
	}
	nAdr += (ECL_DATA_QUAR_HEADER + (bcd2bin8(pTime[1]) * 4 + bcd2bin8(pTime[0]) / 15) * ECL_DATA_QUAR_MSIZE);
	spif_Write(nAdr, pData, sizeof(t_data_quarter));
	flash_Flush(0);
}

int data_DayRead(const uint8_t *pTime, void *pData)
{
	uint_t nAdr;
	uint8_t aBuf[6];

	nAdr = ECL_DATA_DAY_BASE + (pTime[0] - 1) * ECL_DATA_DAY_ALLSIZE;
	spif_Read(nAdr, aBuf, 3);
	if (memcmp(aBuf, pTime, 3) == 0) {
		nAdr += ECL_DATA_DAY_HEADER;
		spif_Read(nAdr, pData, ECL_DATA_DAY_MSIZE);
		return 1;
	}
	memset(pData, GW3761_DATA_INVALID, ECL_DATA_DAY_MSIZE);
	return 0;
}

void data_DayWrite(const uint8_t *pTime, const void *pData)
{
	uint_t nAdr;
	uint8_t aBuf[6];

	nAdr = ECL_DATA_DAY_BASE + (pTime[0] - 1) * ECL_DATA_DAY_ALLSIZE;
	spif_Read(nAdr, aBuf, 3);
	if (memcmp(aBuf, pTime, 3)) {
		//时间错,初始化
		spif_Fill(nAdr, nAdr + ECL_DATA_DAY_ALLSIZE, GW3761_DATA_INVALID);
		spif_Write(nAdr, pTime, 3);
	}
	nAdr += ECL_DATA_DAY_HEADER;
	spif_Write(nAdr, pData, ECL_DATA_DAY_MSIZE);
	flash_Flush(0);
}

void data_YXRead(buf b)
{
	uint8_t *pBuf;

	pBuf = mem_Malloc(ECL_DATA_YX_QTY * ECL_DATA_YX_SIZE);
	if (pBuf != NULL) {
		spif_Read(ECL_DATA_YX_BASE + ECL_DATA_YX_HEADER, pBuf, ECL_DATA_YX_QTY * ECL_DATA_YX_SIZE);
		buf_Push(b, pBuf, ECL_DATA_YX_QTY * ECL_DATA_YX_SIZE);
		mem_Free(pBuf);
	}
}

void data_YXWrite(uint_t nId)
{
	uint32_t nMagic;
	uint8_t *pBuf;

	spif_Read(ECL_DATA_YX_BASE, &nMagic, 4);
	if (nMagic != ECL_DATA_MAGIC_WORD) {
		nMagic = ECL_DATA_MAGIC_WORD;
		spif_Fill(ECL_DATA_YX_BASE, ECL_DATA_YX_BASE + ECL_DATA_YX_HEADER + ECL_DATA_YX_SIZE * ECL_DATA_YX_QTY, GW3761_DATA_INVALID);
		spif_Write(ECL_DATA_YX_BASE, &nMagic, 4);
	}
	pBuf = mem_Malloc(ECL_DATA_YX_QTY * ECL_DATA_YX_SIZE);
	if (pBuf != NULL) {
		spif_Read(ECL_DATA_YX_BASE + ECL_DATA_YX_HEADER, pBuf, ECL_DATA_YX_QTY * ECL_DATA_YX_SIZE);
		memmove(&pBuf[ECL_DATA_YX_SIZE], pBuf, (ECL_DATA_YX_QTY - 1) * ECL_DATA_YX_SIZE);
		pBuf[0] = nId;
        timet2array(rtc_GetTimet(), &pBuf[1], 1);
		spif_Write(ECL_DATA_YX_BASE + ECL_DATA_YX_HEADER, pBuf, ECL_DATA_YX_QTY * ECL_DATA_YX_SIZE);
		mem_Free(pBuf);
	}
}

void data_Copy2Udisk()
{
	DIR_POSIX *d;
	t_afn04_f85 xF85;
	char str[32], sAddr[12], sFile[12], sTemp[12];
	int fd1, fd2;
	uint_t i;
	uint8_t *pTemp, aTime[6];
	time_t tNow, tTime;
	t_data_min xMin;
	t_data_quarter xQuar;
	buf b = {0};

	icp_ParaRead(4, 85, TERMINAL, &xF85, sizeof(t_afn04_f85));
	sprintf(sAddr, FS_USBMSC_PATH"%04X%04X/", xF85.area, xF85.addr);
	fs_mkdir(sAddr, 0);
 	d = fs_opendir(sAddr);
	if (d == NULL)
		return;
	fs_closedir(d);
    LED_UDISK(1);
	sprintf(str, "%s%s", sAddr, "index.txt");
	tNow = rtc_GetTimet();
	tTime = tNow - (31 * 24 * 3600);
	fd1 = fs_open(str, O_RDONLY, 0);
	if (fd1 >= 0) {
		if (fs_read(fd1, str, 19) == 19) {
			aTime[5] = bin2bcd8(atoi(&str[0]) - 2000);
			aTime[4] = bin2bcd8(atoi(&str[5]));
			aTime[3] = bin2bcd8(atoi(&str[8]));
			aTime[2] = bin2bcd8(atoi(&str[11]));
			aTime[1] = bin2bcd8(atoi(&str[14]));
			aTime[0] = bin2bcd8(atoi(&str[17]));
			tTime = MAX(tTime, array2timet(aTime, 1));
		}
		fs_close(fd1);
	}
	fd1 = -1;
	fd2 = -1;
	memset(sFile, 0, sizeof(sFile));
	for (tTime += 60; tTime < tNow; tTime += 60) {
		timet2array(tTime, aTime, 1);
		sprintf(sTemp, "%02X%02X%02X.txt", aTime[5], aTime[4], aTime[3]);

		data_MinRead(&aTime[1], &xMin);
		if (xMin.time != GW3761_DATA_INVALID) {
			if (memcmp(sFile, sTemp, 11)) {
				memcpy(sFile, sTemp, 11);
				sprintf(str, "%sD%s", sAddr, sFile);
				if (fd1 >= 0)
					fs_close(fd1);
				fd1 = fs_open(str, O_WRONLY | O_CREAT | O_APPEND, 0);
				sprintf(str, "%sQ%s", sAddr, sFile);
				if (fd2 >= 0)
					fs_close(fd2);
				fd2 = fs_open(str, O_WRONLY | O_CREAT | O_APPEND, 0);
			}
			if (fd1 >= 0) {
				fs_write(fd1, str, sprintf(str, "20%02X-%02X-%02X %02X:%02X:00", aTime[5], aTime[4], aTime[3], aTime[2], aTime[1]));
				for (i = 0; i < 3; i++) {
					pTemp = &xMin.data[ACM_MSAVE_CUR + i * 3];
					fs_write(fd1, str, sprintf(str, ",%1X%1X.%1X%1X", pTemp[2] & 0xF, pTemp[1] >> 4, pTemp[1] & 0xF, pTemp[0] >> 4));
				}
				for (i = 0; i < 3; i++) {
					pTemp = &xMin.data[ACM_MSAVE_VOL + i * 2];
					fs_write(fd1, str, sprintf(str, ",%02X%1X.%1X", pTemp[1], pTemp[0] >> 4, pTemp[0] & 0xF));
				}
				for (i = 1; i < 4; i++) {
					pTemp = &xMin.data[ACM_MSAVE_PP + i * 3];
					fs_write(fd1, str, sprintf(str, ",%1X.%1X%1X%1X", pTemp[2] & 0xF, pTemp[1] >> 4, pTemp[1] & 0xF, pTemp[0] >> 4));
				}
				for (i = 1; i < 4; i++) {
					pTemp = &xMin.data[ACM_MSAVE_PQ + i * 3];
					fs_write(fd1, str, sprintf(str, ",%1X.%1X%1X%1X", pTemp[2] & 0xF, pTemp[1] >> 4, pTemp[1] & 0xF, pTemp[0] >> 4));
				}
				for (i = 1; i < 4; i++) {
					pTemp = &xMin.data[ACM_MSAVE_COS + i * 2];
					fs_write(fd1, str, sprintf(str, ",%1X.%1X%1X", (pTemp[1] >> 4) & 0x07, pTemp[1] & 0xF, pTemp[0] >> 4));
				}
				pTemp = &xMin.data[ACM_MSAVE_CUR + 9];
				fs_write(fd1, str, sprintf(str, ",%1X%1X.%1X%1X", pTemp[2] & 0xF, pTemp[1] >> 4, pTemp[1] & 0xF, pTemp[0] >> 4));
				pTemp = &xMin.data[ACM_MSAVE_PP];
				fs_write(fd1, str, sprintf(str, ",%1X.%1X%1X%1X", pTemp[2] & 0xF, pTemp[1] >> 4, pTemp[1] & 0xF, pTemp[0] >> 4));
				pTemp = &xMin.data[ACM_MSAVE_PQ];
				fs_write(fd1, str, sprintf(str, ",%1X.%1X%1X%1X", pTemp[2] & 0xF, pTemp[1] >> 4, pTemp[1] & 0xF, pTemp[0] >> 4));
				pTemp = &xMin.data[ACM_MSAVE_COS];
				fs_write(fd1, str, sprintf(str, ",%1X.%1X%1X\r\n", (pTemp[1] >> 4) & 0x07, pTemp[1] & 0xF, pTemp[0] >> 4));
			}
		}

		if ((bcd2bin8(aTime[1]) % 15) == 0) {
			data_QuarterRead(&aTime[1], &xQuar);
			if (xQuar.time != GW3761_DATA_INVALID) {
				if (fd2 >= 0) {
					fs_write(fd2, str, sprintf(str, "20%02X-%02X-%02X %02X:%02X:00", aTime[5], aTime[4], aTime[3], aTime[2], aTime[1]));
					for (i = 0; i < 66; i++) {
						pTemp = &xQuar.data[i * 2];
						fs_write(fd2, str, sprintf(str, ",%1X.%1X%02X", pTemp[1] >> 4, pTemp[1] & 0xF, pTemp[0]));
					}
					for (i = 0; i < 6; i++) {
						pTemp = &xQuar.data[132 + i * 2];
						fs_write(fd2, str, sprintf(str, ",%02X%1X.%1X", pTemp[1], pTemp[0] >> 4, pTemp[0] & 0xF));
					}
					pTemp = &xQuar.data[144];
					fs_write(fd2, str, sprintf(str, ",%02X.%02X\r\n", pTemp[1], pTemp[0]));
				}
			}
		}
	}
	if (fd1 >= 0)
		fs_close(fd1);
	if (fd2 >= 0)
		fs_close(fd2);
	sprintf(str, "%s%s", sAddr, "index.txt");
	fd1 = fs_open(str, O_WRONLY | O_CREAT | O_TRUNC, 0);
	if (fd1 >= 0) {
		fs_write(fd1, str, sprintf(str, "20%02X-%02X-%02X %02X:%02X:00\r\n", aTime[5], aTime[4], aTime[3], aTime[2], aTime[1]));
		fs_write(fd1, str, sprintf(str, "%d Min %d Sec Finshed.\r\n", (rtc_GetTimet() - tNow)/60, (rtc_GetTimet() - tNow)%60));
		fs_close(fd1);
	}
	//遥信
	sprintf(str, "%s%s", sAddr, "pb325_yx.txt");
	fd1 = fs_open(str, O_WRONLY | O_CREAT | O_TRUNC, 0);
	if (fd1 >= 0) {
		data_YXRead(b);
		for (i = 0; i < ECL_DATA_YX_QTY; i++) {
			pTemp = &b->p[i * ECL_DATA_YX_SIZE];
			if (memtest(pTemp, 0xEE, ECL_DATA_YX_SIZE))
				fs_write(fd1, str, sprintf(str, "%03d=[%d]20%02X-%02X-%02X %02X:%02X:%02X\r\n", i + 1, pTemp[0], pTemp[6], pTemp[5], pTemp[4], pTemp[3], pTemp[2], pTemp[1]));
			else
				fs_write(fd1, str, sprintf(str, "%03d=\r\n", i + 1));
		}		
		fs_close(fd1);
		buf_Release(b);
	}
    BEEP(1);
    os_thd_Sleep(1000);
    BEEP(0);
	LED_UDISK(0);
}







