/*
 * fat_if.c
 *
 *  Created on: Sep 14, 2023
 *      Author: daniel, paulo
 */

/* Private includes ---------------------------------------------------------- */
#include "fat_if.h"
#include "fat.h"

#include "W25Qxx.h"

#include <string.h>
#include "cmsis_os2.h"

/* Private Variables --------------------------------------------------------- */
FAT_HandlerStruct_t myFAT;
extern osMutexId_t ExtFlashMtxHandle;

/* Exported Functions -------------------------------------------------------- */
FAT_IF_Status_t FAT_IF_MemoryWrite(uint8_t *pSource, uint8_t *pDestination, uint32_t Size)
{

	osMutexAcquire(ExtFlashMtxHandle, osWaitForever);
	W25qxx_WriteBytes(&hw25q1, pSource, (uint32_t)pDestination, Size, W25QXX_DIRECT);
	osMutexRelease(ExtFlashMtxHandle);

	return FAT_IF_STATUS_OK;
}


FAT_IF_Status_t FAT_IF_MemoryRead(uint8_t *pSource, uint8_t *pDestination, uint32_t Size)
{
	osMutexAcquire(ExtFlashMtxHandle, osWaitForever);
	W25qxx_ReadBytes(&hw25q1, pDestination, (uint32_t)pSource, Size);
	osMutexRelease(ExtFlashMtxHandle);

	return FAT_IF_STATUS_OK;
}


FAT_IF_Status_t FAT_IF_EraseBlock(uint32_t BlockAddress)
{
	uint32_t BlockNumber = (uint32_t)BlockAddress/hw25q1.SectorSize;

	osMutexAcquire(ExtFlashMtxHandle, osWaitForever);
	W25qxx_EraseSector(&hw25q1, BlockNumber);
	osMutexRelease(ExtFlashMtxHandle);

	return FAT_IF_STATUS_EMPTY;
}

FAT_IF_Status_t FAT_IF_MemoryVerify()
{
	return FAT_IF_STATUS_EMPTY;
}

FAT_IF_Status_t FAT_IF_IsEmptyBlock(uint32_t BlockAddress)
{
	uint32_t BlockNumber = (uint32_t)BlockAddress/hw25q1.SectorSize;
	W25QXX_Status_t status;

	osMutexAcquire(ExtFlashMtxHandle, osWaitForever);
	status = W25qxx_IsEmptySector(&hw25q1, BlockNumber, 0, 0);
	osMutexRelease(ExtFlashMtxHandle);

	if(status == W25QXX_EMPTY)
		return FAT_IF_STATUS_EMPTY;
	else
		return FAT_IF_STATUS_NOTEMPTY;
}
