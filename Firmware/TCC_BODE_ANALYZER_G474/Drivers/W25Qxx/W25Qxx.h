/*
 * Serial Flash - W25Qxx STM32
 *
 *  Created on: Sep 7, 2023
 *  Author: Daniel Botelho
 *  Author: Paulo Pecegueiro
 */

#ifndef _W25QXX_H
#define _W25QXX_H

#define W25Q_USE_DMA 0x55

/* Exported includes --------------------------------------------------------- */
#include "main.h"
#include "spi.h"
#ifdef W25Q_USE_DMA
#include "dma.h"
#endif //W25Q_USE_DMA

#ifdef __cplusplus
extern "C" {
#endif

/* Exported Types ------------------------------------------------------------- */
typedef enum
{
	W25QXX_OK = 0,
	W25QXX_BUSY,
	W25QXX_ERROR,
	W25QXX_EMPTY,
	W25QXX_NOTEMPTY
}W25QXX_Status_t;

typedef enum
{
	W25QXX_DIRECT = 0,
	W25QXX_REVERSE
}W25QXX_ByteOrder_t;

typedef enum
{
	W25Q10 = 1,
	W25Q20,
	W25Q40,
	W25Q80,
	W25Q16,
	W25Q32,
	W25Q64,
	W25Q128,
	W25Q256,
	W25Q512,
}W25QXX_Id_t;

typedef struct
{
	W25QXX_Id_t			ID;
	uint8_t				UniqID[8];
	uint16_t    		PageSize;
	uint32_t			PageCount;
	uint32_t			SectorSize;
	uint32_t			SectorCount;
	uint32_t			BlockSize;
	uint32_t			BlockCount;
	uint32_t			CapacityInKiloByte;
	uint8_t				StatusRegister1;
	uint8_t				StatusRegister2;
	uint8_t				StatusRegister3;
	uint8_t				Lock;

	SPI_HandleTypeDef 	*spi;
	GPIO_TypeDef		*GPIO_CS_Port;
	uint16_t			GPIO_CS_Pin;

}W25QXX_Handler_t;

/* Exported Variables -------------------------------------------------------- */
extern W25QXX_Handler_t hw25q1;

/* Exported Function Prototypes ---------------------------------------------- */
W25QXX_Status_t W25qxx_Init(W25QXX_Handler_t *hw25q);

W25QXX_Status_t W25qxx_EraseChip(W25QXX_Handler_t *hw25q);
W25QXX_Status_t W25qxx_EraseSector(W25QXX_Handler_t *hw25q, uint32_t SectorAddr);

uint32_t W25qxx_PageToSector(W25QXX_Handler_t *hw25q, uint32_t PageAddress);
uint32_t W25qxx_SectorToPage(W25QXX_Handler_t *hw25q, uint32_t SectorAddress);

W25QXX_Status_t W25qxx_IsEmptyPage(W25QXX_Handler_t *hw25q, uint32_t Page_Address, uint32_t OffsetInByte, uint32_t NumByteToCheck_up_to_PageSize);
W25QXX_Status_t W25qxx_IsEmptySector(W25QXX_Handler_t *hw25q, uint32_t Sector_Address, uint32_t OffsetInByte, uint32_t NumByteToCheck_up_to_SectorSize);

W25QXX_Status_t W25qxx_WriteBytes(W25QXX_Handler_t *hw25q, uint8_t *pBuffer, uint32_t Size, uint32_t Byte_Address, W25QXX_ByteOrder_t ByteOrder);
#ifdef W25Q_USE_DMA
W25QXX_Status_t W25qxx_Write256BytesDMA(W25QXX_Handler_t *hw25q, uint8_t *pBuffer, uint32_t Size, uint32_t WriteAddress);
#endif
W25QXX_Status_t W25qxx_ReadBytes(W25QXX_Handler_t *hw25q, uint8_t *pBuffer,uint32_t ReadAddr,uint32_t NumByteToRead);
#ifdef W25Q_USE_DMA
W25QXX_Status_t W25qxx_ReadBytesDMA(W25QXX_Handler_t *hw25q, uint8_t *pBuffer, uint32_t ReadAddr, uint32_t NumbyteToRead);
#endif

W25QXX_Status_t W25qxx_Verify(W25QXX_Handler_t *hw25q, uint8_t *pBuffer, uint32_t ReadAddr, uint32_t NumByteToRead, W25QXX_ByteOrder_t ByteOrder);
W25QXX_Status_t W25QXX_WaitWriteEnd(W25QXX_Handler_t *hw25q);

#ifdef __cplusplus
}
#endif	//__cplusplus

#endif	//_W25QXX_H
