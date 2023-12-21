/*
 * app_filesystem.c
 *
 *  Created on: Dec 3, 2023
 *      Author: daniel, paulo
 */

/* Private includes */
#include "W25Qxx.h"
#include "fat.h"
#include "app_main.h"
#include "app_filesystem.h"
#include "usbd_core.h"

/* Private global variables */
const uint8_t FileHeader[] = "Frequency,Magnitude,Phase\r\n";
extern USBD_HandleTypeDef hUsbDeviceFS;
const int frequencies[38] =
{
		1, 2, 3, 4, 5, 6, 7, 8, 9,
		10, 20, 30, 40, 50, 60, 70, 80, 90,
		100, 200, 300, 400, 500, 600, 700, 800, 900,
		1000, 2000, 3000, 4000, 5000, 6000, 7000, 8000, 9000,
		10000, 20000
};

/* Exported Functions */
void APP_FileSystemInit(void)
{
	/* Local variables */
	const FAT_InitStruct_t FatInit =
	{
			0x0200,					// Bytes Per Sector
			0x08,					// Sectors Per Cluster
			0x01,					// Number of FATs
			0x01,					// FAT size in Sectors
			0x0020,					// Number of Root Entries
			0x2000,					// Memory Size
			0x1000,					// Memory Block Size
			"BDANALYZER",			// Volume ID
			(uint8_t*)0x00FFE000	// Memory Start Address
	};

	/* Try to mount the file system */
	if(FAT_Mount(&myFAT, FatInit.MemoryStartAddress, 0x1000) != FAT_OK)
	{
		/* In case of error try to format it */
		if(FAT_Format((FAT_InitStruct_t *)&FatInit) == FAT_OK)
		{
			/* Try to mount the unity again */
			if(FAT_Mount(&myFAT, FatInit.MemoryStartAddress, 0x1000) != FAT_OK)
				/* Call the error handler in case of error */
				Error_Handler();
			else
			{
				/* Try to create the CSV */
				if(FAT_CreateFile(&myFAT, (uint8_t *)&"MYFILE  CSV", 0x1000) == FAT_OK)
					FAT_EditFileContent(&myFAT, (uint8_t *)&"MYFILE  CSV", (uint8_t *)FileHeader, sizeof(FileHeader));
			}
		}
		else
			/* Call the error handler in case of error */
			Error_Handler();
	}
}

void APP_FormatFS(void)
{
	/* Local Variables */
	const FAT_InitStruct_t FatInit =
	{
			0x0200,					// Bytes Per Sector
			0x08,					// Sectors Per Cluster
			0x01,					// Number of FATs
			0x01,					// FAT size in Sectors
			0x0020,					// Number of Root Entries
			0x2000,					// Memory Size
			0x1000,					// Memory Block Size
			"BDANALYZER",			// Volume ID
			(uint8_t*)0x00FFE000	// Memory Start Address
	};

	/* Stop the USB Peripheral */
	USBD_Stop(&hUsbDeviceFS);

	/* Try to format the File System */
	if(FAT_Format((FAT_InitStruct_t *)&FatInit) == FAT_OK)
	{
		/* Update the FS status */
		myFAT.Status = FAT_NOT_MOUNTED;

		/* Mount the unity again */
		if(FAT_Mount(&myFAT, FatInit.MemoryStartAddress, 0x1000) == FAT_OK)
		{
			/* Try to create the CSV */
			if(FAT_CreateFile(&myFAT, (uint8_t *)&"MYFILE  CSV", 0x1000) == FAT_OK)
				FAT_EditFileContent(&myFAT, (uint8_t *)&"MYFILE  CSV", (uint8_t *)FileHeader, sizeof(FileHeader));
		}
	}

	/* Start the USB Peripheral */
	USBD_Start(&hUsbDeviceFS);
}

void APP_ReadFileContent(void)
{

}

void APP_WriteFileContent(void)
{
	/* Private variables */
	uint8_t content[0x1000];
	uint8_t *pPointer;

	/* Copy the header into the buffer */
	memcpy(content, FileHeader, sizeof(FileHeader));

	/* Increase the pointer */
	pPointer = content;
	pPointer += sizeof(FileHeader);

	/* Stop the USB Peripheral */
	USBD_Stop(&hUsbDeviceFS);

	/* Construct the package to be programmed */
	for(uint8_t i = 0; i < FREQ_COUNT; i++)
	{
		sprintf((char *)content + sizeof(FileHeader) + (i * 24), "%5d,%8.4f,%8.4f\n\r", frequencies[i], Acq.gain[i], Acq.phase[i]);
		pPointer += 24;
	}

	/* Fill the rest of memory with 0s */
	memset(pPointer, 0x00, 0x1000 - (uint32_t)(pPointer - content));


	/* Edit the file */
	FAT_EditFileContent(&myFAT, (uint8_t *)&"MYFILE  CSV", content, 0x1000);

	/* Start the USB Peripheral */
	USBD_Start(&hUsbDeviceFS);
}
