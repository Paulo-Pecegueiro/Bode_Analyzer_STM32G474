/*
 * Serial Flash - W25Qxx STM32
 *
 *  Created on: Sep 7, 2023
 *  Author: Daniel Botelho
 *  Author: Paulo Pecegueiro
 */

/* Private includes ---------------------------------------------------------- */
#include "W25Qxx.h"
#include "cmsis_os2.h"

/* Private defines ----------------------------------------------------------- */
#define _W25QXX_COMMAND_WRITE_ENABLE                      	0x06        /**< write enable */
#define _W25QXX_COMMAND_VOLATILE_SR_WRITE_ENABLE          	0x50        /**< sr write enable */
#define _W25QXX_COMMAND_WRITE_DISABLE                     	0x04        /**< write disable */
#define _W25QXX_COMMAND_READ_STATUS_REG1                  	0x05        /**< read status register-1 */
#define _W25QXX_COMMAND_READ_STATUS_REG2                  	0x35        /**< read status register-2 */
#define _W25QXX_COMMAND_READ_STATUS_REG3                  	0x15        /**< read status register-3 */
#define _W25QXX_COMMAND_WRITE_STATUS_REG1                 	0x01        /**< write status register-1 */
#define _W25QXX_COMMAND_WRITE_STATUS_REG2                 	0x31        /**< write status register-2 */
#define _W25QXX_COMMAND_WRITE_STATUS_REG3                 	0x11        /**< write status register-3 */
#define _W25QXX_COMMAND_CHIP_ERASE                        	0xC7        /**< chip erase */
#define _W25QXX_COMMAND_READ_MANUFACTURER                 	0x90        /**< manufacturer */
#define _W25QXX_COMMAND_READ_ID                           	0x9F        /**< read id */
#define _W25QXX_COMMAND_ENABLE_RESET                      	0x66        /**< enable reset */
#define _W25QXX_COMMAND_RESET_DEVICE                      	0x99        /**< reset device */
#define _W25QXX_COMMAND_READ_UNIQUE_ID                    	0x4B        /**< read unique id */
#define _W25QXX_COMMAND_PAGE_PROGRAM                      	0x02        /**< page program */
#define _W25QXX_COMMAND_SECTOR_ERASE_4K                   	0x20        /**< sector erase */
#define _W25QXX_COMMAND_BLOCK_ERASE_32K                   	0x52        /**< block erase */
#define _W25QXX_COMMAND_BLOCK_ERASE_64K                   	0xD8        /**< block erase */
#define _W25QXX_COMMAND_READ_DATA                         	0x03        /**< read data */
#define _W25QXX_COMMAND_FAST_READ                         	0x0B        /**< fast read */
#define _W25QXX_COMMAND_READ_SFDP_REGISTER                	0x5A        /**< read SFDP register */
#define _W25QXX_COMMAND_ERASE_SECURITY_REGISTER           	0x44        /**< erase security register */
#define _W25QXX_COMMAND_PROGRAM_SECURITY_REGISTER         	0x42        /**< program security register */
#define _W25QXX_COMMAND_READ_SECURITY_REGISTER            	0x48        /**< read security register */

#define _W25QXX_DUMMY									 	0x55
#define _W25QXX_MAX_INSTANCES								0x02 * 0x02

/* Private Macros ------------------------------------------------------------ */
#define __W25_CS_LOW(__PORT__, __PIN__)			(__PORT__)->BSRR  |= ((__PIN__) << 16)
#define __W25_CS_HIGH(__PORT__, __PIN__)		(__PORT__)->BSRR  |= (__PIN__)


/* Private Types ------------------------------------------------------------- */
#ifdef W25Q_USE_DMA
typedef struct
{
	uint8_t DMAInstanceNbr;
	W25QXX_Handler_t *W25QXX_Instance[_W25QXX_MAX_INSTANCES];
}W25QXX_DMA_Controller_t;
#endif

/* Private Variables --------------------------------------------------------- */
uint8_t dummy = _W25QXX_DUMMY;
W25QXX_Handler_t hw25q1;
extern osSemaphoreId_t FlashDMASemHandle;

#ifdef W25Q_USE_DMA
W25QXX_DMA_Controller_t DMA_Controller;
#endif

/* Private Function Prototypes ----------------------------------------------- */
void 				_w25qxx_delay(uint16_t msDelay);
void				_w25qxx_send_address(W25QXX_Handler_t *hw25q, uint32_t Address);
void 				_w25qxx_wait_write_end(W25QXX_Handler_t *hw25q);
void 				_w25qxx_write_enable(W25QXX_Handler_t *hw25q);
uint8_t 			_w25qxx_spi(W25QXX_Handler_t *hw25q, uint8_t command);
uint32_t 			_w25qxx_read_id(W25QXX_Handler_t *hw25q);
W25QXX_Status_t		_w25qxx_read_uniq_id(W25QXX_Handler_t *hw25q);
W25QXX_Status_t		_w25qxx_read_status_reg(W25QXX_Handler_t *hw25q, uint8_t RegNbr);
W25QXX_Status_t 	_w25qxx_write_data(W25QXX_Handler_t *hw25q, uint8_t *pData, uint32_t Size, uint32_t WriteAddress, W25QXX_ByteOrder_t ByteOrder);

#ifdef W25Q_USE_DMA
void 				_w25qxx_dmarx_callback(DMA_HandleTypeDef *hdma);
void 				_w25qxx_dmatx_callback(DMA_HandleTypeDef *hdma);
void 				_w25qxx_dmaerror_callback(DMA_HandleTypeDef *hdma);
#endif

/* Private Functions --------------------------------------------------------- */
void _w25qxx_delay(uint16_t msDelay)
{
	/* Call the delay function */
	HAL_Delay(msDelay);
}

void _w25qxx_send_address(W25QXX_Handler_t *hw25q, uint32_t Address)
{
	_w25qxx_spi(hw25q, (Address & 0xFF0000) >> 16);
	_w25qxx_spi(hw25q, (Address & 0x00FF00) >> 8);
	_w25qxx_spi(hw25q, (Address & 0x0000FF));
}

void _w25qxx_wait_write_end(W25QXX_Handler_t *hw25q)
{
	/* Wait a ms */
//	_w25qxx_delay(1);

	/* Drive the CS Pin LOW */
	__W25_CS_LOW(hw25q->GPIO_CS_Port, hw25q->GPIO_CS_Pin);

	/* Send the read status reg 1 */
	_w25qxx_spi(hw25q, _W25QXX_COMMAND_READ_STATUS_REG1);

	/* Read the Status Reg until the flag is reset */
	do
	{
		hw25q->StatusRegister1 = _w25qxx_spi(hw25q, _W25QXX_DUMMY);
//		_w25qxx_delay(1);
	} while (hw25q->StatusRegister1 & 0x01);

	/* Drive the CS Pin High */
	__W25_CS_HIGH(hw25q->GPIO_CS_Port, hw25q->GPIO_CS_Pin);
}

void _w25qxx_write_enable(W25QXX_Handler_t *hw25q)
{
	/* Drive the CS Pin LOW */
	__W25_CS_LOW(hw25q->GPIO_CS_Port, hw25q->GPIO_CS_Pin);

	/* Send the Write Enable Command */
	_w25qxx_spi(hw25q, _W25QXX_COMMAND_WRITE_ENABLE);

	/* Drive the CS Pin High */
	__W25_CS_HIGH(hw25q->GPIO_CS_Port, hw25q->GPIO_CS_Pin);
}

uint8_t _w25qxx_spi(W25 QXX_Handler_t *hw25q, uint8_t command)
{
	/* Local Variable */
	uint8_t temp;

	/* Put data to be transmitted */
	*(__IO uint8_t *)&hw25q->spi->Instance->DR = command;

	/* Wait until data is being transmitted */
	while(__HAL_SPI_GET_FLAG(hw25q->spi, SPI_FLAG_BSY));

	/* Read the data */
	temp = *(__IO uint8_t *)&hw25q->spi->Instance->DR;

	/* Return the read data */
	return temp;
}

uint32_t _w25qxx_read_id(W25QXX_Handler_t *hw25q)
{
	/* Local Variables */
	uint8_t Temp[3];
	uint32_t id;

	/* Drive the CS Pin LOW */
	__W25_CS_LOW(hw25q->GPIO_CS_Port, hw25q->GPIO_CS_Pin);

	/* Transmit the command */
	_w25qxx_spi(hw25q, _W25QXX_COMMAND_READ_ID);

	/* Receive the Data */
	Temp[0] = _w25qxx_spi(hw25q, _W25QXX_DUMMY);
	Temp[1] = _w25qxx_spi(hw25q, _W25QXX_DUMMY);
	Temp[2] = _w25qxx_spi(hw25q, _W25QXX_DUMMY);

	/* Drive the CS Pin High */
	__W25_CS_HIGH(hw25q->GPIO_CS_Port, hw25q->GPIO_CS_Pin);

	/* Compute the ID */
	id = (Temp[0] << 16) | (Temp[1] << 8) | Temp[2];

	/* Return the ID */
	return id;
}

W25QXX_Status_t _w25qxx_read_uniq_id(W25QXX_Handler_t *hw25q)
{
	/* Drive the CS Pin LOW */
	__W25_CS_LOW(hw25q->GPIO_CS_Port, hw25q->GPIO_CS_Pin);

	/* Transmit the command */
	_w25qxx_spi(hw25q, _W25QXX_COMMAND_READ_UNIQUE_ID);

	/* Send 4 Dummy data */
	for(uint8_t i = 0; i < 4; i++)
		_w25qxx_spi(hw25q, _W25QXX_DUMMY);

	/* Read the Unique ID */
	for(uint8_t i = 0; i < 8; i++)
		hw25q->UniqID[i] = _w25qxx_spi(hw25q, _W25QXX_DUMMY);

	/* Drive the CS Pin High */
	__W25_CS_HIGH(hw25q->GPIO_CS_Port, hw25q->GPIO_CS_Pin);

	return W25QXX_OK;
}

uint8_t _w25qxx_read_status_reg(W25QXX_Handler_t *hw25q, uint8_t RegNbr)
{
	/* Drive the CS Pin LOW */
	__W25_CS_LOW(hw25q->GPIO_CS_Port, hw25q->GPIO_CS_Pin);

	/* Select the Status Register */
	switch (RegNbr)
	{
		case 0x01:
			_w25qxx_spi(hw25q, _W25QXX_COMMAND_READ_STATUS_REG1);
			hw25q->StatusRegister1 = _w25qxx_spi(hw25q, _W25QXX_DUMMY);
			break;

		case 0x02:
			_w25qxx_spi(hw25q, _W25QXX_COMMAND_READ_STATUS_REG2);
			hw25q->StatusRegister2 = _w25qxx_spi(hw25q, _W25QXX_DUMMY);
			break;

		case 0x03:
			_w25qxx_spi(hw25q, _W25QXX_COMMAND_READ_STATUS_REG3);
			hw25q->StatusRegister3 = _w25qxx_spi(hw25q, _W25QXX_DUMMY);
			break;

		default:
			return W25QXX_ERROR;
			break;
	}

	/* Drive the CS Pin High */
	__W25_CS_HIGH(hw25q->GPIO_CS_Port, hw25q->GPIO_CS_Pin);

	return W25QXX_OK;
}

W25QXX_Status_t _w25qxx_verify_empty(W25QXX_Handler_t *hw25q, uint32_t Size, uint32_t VerifyAddress)
{
	/* Drive the CS Pin LOW */
	__W25_CS_LOW(hw25q->GPIO_CS_Port, hw25q->GPIO_CS_Pin);

	/* Send the fast Read Command */
	_w25qxx_spi(hw25q, _W25QXX_COMMAND_FAST_READ);

	/* Send the address */
	_w25qxx_send_address(hw25q, VerifyAddress);

	/* Send a Dummy data */
	_w25qxx_spi(hw25q, _W25QXX_DUMMY);

	/* Read the data positions */
	for(uint32_t i = 0; i < Size; i++)
	{
		/* Check if the position is empty */
		if(_w25qxx_spi(hw25q, _W25QXX_DUMMY) != 0xFF)
		{
			/* Drive the CS Pin High */
			__W25_CS_HIGH(hw25q->GPIO_CS_Port, hw25q->GPIO_CS_Pin);

			/* Return not empty */
			return W25QXX_NOTEMPTY;
		}
	}

	/* Drive the CS Pin High */
	__W25_CS_HIGH(hw25q->GPIO_CS_Port, hw25q->GPIO_CS_Pin);

	return W25QXX_EMPTY;
}

W25QXX_Status_t _w25qxx_write_data(W25QXX_Handler_t *hw25q, uint8_t *pData, uint32_t Size, uint32_t WriteAddress, W25QXX_ByteOrder_t ByteOrder)
{
	/* Check if the size is correct */
	if(Size > hw25q->PageSize)
	{
		return W25QXX_ERROR;
	}

	/* Enable the memory write */
	_w25qxx_write_enable(hw25q);

	/* Drive the CS Pin LOW */
	__W25_CS_LOW(hw25q->GPIO_CS_Port, hw25q->GPIO_CS_Pin);

	/* Send the Program Command */
	_w25qxx_spi(hw25q, _W25QXX_COMMAND_PAGE_PROGRAM);

	/* Send the address */
	_w25qxx_send_address(hw25q, WriteAddress);

	/* Check the Byte order */
	if(ByteOrder == W25QXX_DIRECT)
	{
		/* Write data into memory*/
		for(uint32_t i = 0; i < Size; i++)
		{
			_w25qxx_spi(hw25q, *pData++);
		}
	}
	else if(ByteOrder == W25QXX_REVERSE)
	{
		/* Local Variables */
		uint8_t temp = 3;

		/* Write data into memory */
		for(uint32_t i = 0; i < Size; i++)
		{
			_w25qxx_spi(hw25q, *(pData + temp));

			if(temp == 0)
			{
				pData +=4;
				temp = 3;
			}
			else
				temp--;
		}
	}

	/* Drive the CS Pin High */
	__W25_CS_HIGH(hw25q->GPIO_CS_Port, hw25q->GPIO_CS_Pin);

	/* Wait for the Write operation to be completed */
	_w25qxx_wait_write_end(hw25q);

	/* Return the operation result */
	return W25QXX_OK;
}

#ifdef W25Q_USE_DMA
void _w25qxx_dmarx_callback(DMA_HandleTypeDef *hdma)
{
	/* Local Variables */
	W25QXX_Handler_t *hw25q;

	/* Search for the instance which generated the Interrupt */
	for(uint8_t i = 0; i < _W25QXX_MAX_INSTANCES; i++)
	{
		if(hdma == DMA_Controller.W25QXX_Instance[i]->spi->hdmarx)
		{
			/* Register the instance */
			hw25q = DMA_Controller.W25QXX_Instance[i];

			/* Drive the CS Pin HIGH */
			__W25_CS_HIGH(hw25q->GPIO_CS_Port, hw25q->GPIO_CS_Pin);

			/* Disable the DMA Transfer */
			hw25q->spi->hdmarx->Instance->CCR &= ~DMA_CCR_EN;

			/* Disable the RX DMA buffer */
			hw25q->spi->Instance->CR2 &= ~SPI_CR2_RXDMAEN;

			/* Unlock the instance */
			hw25q->Lock = 0;

			/* Release the Semaphore */
			osSemaphoreRelease(FlashDMASemHandle);
			break;
		}
	}
}

void _w25qxx_dmatx_callback(DMA_HandleTypeDef *hdma)
{
	/* Local Variables */
	W25QXX_Handler_t *hw25q;

	/* Search for the instance which generated the Interrupt */
	for(uint8_t i = 0; i < _W25QXX_MAX_INSTANCES; i++)
	{
		if(hdma == DMA_Controller.W25QXX_Instance[i]->spi->hdmatx)
		{
			/* Register the instance */
			hw25q = DMA_Controller.W25QXX_Instance[i];

			/* Drive the CS Pin HIGH */
			__W25_CS_HIGH(hw25q->GPIO_CS_Port, hw25q->GPIO_CS_Pin);

			/* Disable the DMA Transfer */
			hw25q->spi->hdmatx->Instance->CCR &= ~DMA_CCR_EN;

			/* Disable the TX DMA buffer */
			hw25q->spi->Instance->CR2 &= ~SPI_CR2_TXDMAEN;

			/* Clear the RX FIFO */
			while(hw25q->spi->Instance->SR & SPI_SR_RXNE)
				hw25q->spi->Instance->DR;

			/* Unlock the instance */
			hw25q->Lock = 0;
			break;
		}
	}
}

void _w25qxx_dmaerror_callback(DMA_HandleTypeDef *hdma)
{
	while(1);
}
#endif

/* Exported Functions -------------------------------------------------------- */
W25QXX_Status_t W25qxx_Init(W25QXX_Handler_t *hw25q)
{
	/* Private Variables */
	uint32_t id;
#ifdef W25Q_USE_DMA
	FlagStatus UseDMA = RESET;
#endif

	/* Check if the instance is being used */
	if(hw25q->Lock == 1)
		return W25QXX_BUSY;

	/* Lock the instance */
	hw25q->Lock = 1;

	/* Check if the SPI is already enabled */
	if ((hw25q->spi->Instance->CR1 & SPI_CR1_SPE) != SPI_CR1_SPE)
	{
		/* Enable SPI peripheral */
		__HAL_SPI_ENABLE(hw25q->spi);
	}

	/* Drive the CS Pin High */
	__W25_CS_HIGH(hw25q->GPIO_CS_Port, hw25q->GPIO_CS_Pin);

	/* Read the memory ID */
	id = _w25qxx_read_id(hw25q);

	/* Identify the memory */
	switch (id & 0x000000FF)
	{
		case 0x20: //W25Q512
			hw25q->ID = W25Q512;
			hw25q->BlockCount = 1024;
			break;

		case 0x19: //W25Q256
			hw25q->ID = W25Q256;
			hw25q->BlockCount = 512;
			break;

		case 0x18: //W25Q128
			hw25q->ID = W25Q128;
			hw25q->BlockCount = 256;
			break;

		case 0x17: //W25Q64
			hw25q->ID = W25Q64;
			hw25q->BlockCount = 128;
			break;

		case 0x16: //W25Q32
			hw25q->ID = W25Q32;
			hw25q->BlockCount = 64;
			break;

		case 0x15: //W25Q16
			hw25q->ID = W25Q16;
			hw25q->BlockCount = 32;
			break;

		case 0x14: //W25Q80
			hw25q->ID = W25Q80;
			hw25q->BlockCount = 16;
			break;

		case 0x13: //W25Q40
			hw25q->ID = W25Q40;
			hw25q->BlockCount = 8;
			break;

		case 0x12: //W25Q20
			hw25q->ID = W25Q20;
			hw25q->BlockCount = 4;
			break;

		case 0x11: //W25Q10
			hw25q->ID = W25Q10;
			hw25q->BlockCount = 2;
			break;

		default: //Unknown ID
			return W25QXX_ERROR;
	}

	hw25q->PageSize 			= 256;															// Compute the Page Size
	hw25q->SectorSize 			= 0x1000;														// Compute the Sector Size
	hw25q->SectorCount 			= hw25q->BlockCount * 16;										// Compute the Sector Count
	hw25q->PageCount			= (hw25q->SectorCount * hw25q->SectorSize)/hw25q->PageSize;		// Compute the Page Count
	hw25q->BlockSize			= hw25q->SectorSize * 16;										// Compute the Block Size
	hw25q->CapacityInKiloByte	= (hw25q->SectorCount * hw25q->SectorSize)/1024;				// Compute the Capacity

	_w25qxx_read_uniq_id(hw25q);																	// Compute the Unique ID
	_w25qxx_read_status_reg(hw25q, 1);														// Compute the Status Register 1
	_w25qxx_read_status_reg(hw25q, 2);														// Compute the Status Register 2
	_w25qxx_read_status_reg(hw25q, 3);														// Compute the Status Register 3

#ifdef W25Q_USE_DMA
	/* Program the RX DMA options */
	if(hw25q->spi->hdmarx != 0)
	{
		/* Configure the Peripheral Address */
		hw25q->spi->hdmarx->Instance->CPAR = (uint32_t)&hw25q->spi->Instance->DR;

		/* Register the DMA callback */
		hw25q->spi->hdmarx->XferCpltCallback = _w25qxx_dmarx_callback;
		hw25q->spi->hdmarx->XferErrorCallback = _w25qxx_dmaerror_callback;

		/* Set the DMA Flag */
		UseDMA = SET;
	}

	/* Program the TX DMA options */
	if(hw25q->spi->hdmatx != 0)
	{
		/* Configure the Peripheral Address */
		hw25q->spi->hdmatx->Instance->CPAR = (uint32_t)&hw25q->spi->Instance->DR;

		/* Register the DMA callbacks */
		hw25q->spi->hdmatx->XferCpltCallback = _w25qxx_dmatx_callback;
		hw25q->spi->hdmatx->XferErrorCallback = _w25qxx_dmaerror_callback;

		/* Set the DMA Flag */
		UseDMA = SET;
	}

	/* Register the DMA Instance */
	if(UseDMA == SET)
	{
		DMA_Controller.W25QXX_Instance[DMA_Controller.DMAInstanceNbr] = hw25q;
		DMA_Controller.DMAInstanceNbr++;
	}
#endif

	/* Unlock the Instance */
	hw25q->Lock = 0;

	/* Return OK */
	return W25QXX_OK;
}

W25QXX_Status_t W25qxx_EraseChip(W25QXX_Handler_t *hw25q)
{
	/* Local Variables */
	uint8_t status = W25QXX_OK;

	/* Check if the instance is being used */
	if(hw25q->Lock == 1)
		return W25QXX_BUSY;

	/* Lock the instance */
	hw25q->Lock = 1;

	/* Wait for the Write operation be ended */
	_w25qxx_wait_write_end(hw25q);

	/* Enable the memory write */
	_w25qxx_write_enable(hw25q);

	/* Drive the CS Pin LOW */
	__W25_CS_LOW(hw25q->GPIO_CS_Port, hw25q->GPIO_CS_Pin);

	/* Send the Chip Erase Command */
	_w25qxx_spi(hw25q, _W25QXX_COMMAND_CHIP_ERASE);

	/* Drive the CS Pin High */
	__W25_CS_HIGH(hw25q->GPIO_CS_Port, hw25q->GPIO_CS_Pin);

	/* Wait for the Write operation be ended */
	_w25qxx_wait_write_end(hw25q);

	/* Unlock the Instance */
	hw25q->Lock = 0;

	/* Return the Operation Status */
	return status;
}

W25QXX_Status_t W25qxx_EraseSector(W25QXX_Handler_t *hw25q, uint32_t SectorAddr)
{
	/* Local Variables */
	uint8_t status = W25QXX_OK;

	/* Determine the Sector Address */
	SectorAddr = SectorAddr * hw25q->SectorSize;

	/* Check if the instance is being used */
	if(hw25q->Lock == 1)
		return W25QXX_BUSY;

	/* Lock the instance */
	hw25q->Lock = 1;

	/* Enable the memory write */
	_w25qxx_write_enable(hw25q);

	/* Drive the CS Pin LOW */
	__W25_CS_LOW(hw25q->GPIO_CS_Port, hw25q->GPIO_CS_Pin);

	/* Check if the memory is greater then 256MBit */
	if(hw25q->ID >= W25Q256)
	{
		/* Not yet supported */
		status = W25QXX_ERROR;
		return status;
	}
	else
	{
		/* Send the Sector Erase Command */
		_w25qxx_spi(hw25q, _W25QXX_COMMAND_SECTOR_ERASE_4K);
	}

	/* Send the Address to be erased */
	_w25qxx_send_address(hw25q, SectorAddr);

	/* Drive the CS Pin High */
	__W25_CS_HIGH(hw25q->GPIO_CS_Port, hw25q->GPIO_CS_Pin);

	/* Wait for the Write operation be ended */
	_w25qxx_wait_write_end(hw25q);

	/* Unlock the Instance */
	hw25q->Lock = 0;

	/* Return the Operation Status */
	return status;
}

uint32_t W25qxx_PageToSector(W25QXX_Handler_t *hw25q, uint32_t PageAddress)
{
	return ((PageAddress * hw25q->PageSize) / hw25q->SectorSize);
}

uint32_t W25qxx_SectorToPage(W25QXX_Handler_t *hw25q, uint32_t SectorAddress)
{
	return ((SectorAddress * hw25q->SectorSize) / hw25q->PageSize);
}

W25QXX_Status_t W25qxx_IsEmptyPage(W25QXX_Handler_t *hw25q, uint32_t Page_Address, uint32_t OffsetInByte, uint32_t NumByteToCheck_up_to_PageSize)
{
	/* Local Variables */
	uint32_t WorkAddress;
	W25QXX_Status_t status;

	/* Check if the instance is being used */
	if(hw25q->Lock == 1)
		return W25QXX_BUSY;

	/* Lock the instance */
	hw25q->Lock = 1;

	/* Compute the Number of Bytes to read */
	if(((NumByteToCheck_up_to_PageSize + OffsetInByte) > hw25q->PageSize) || (NumByteToCheck_up_to_PageSize == 0))
		NumByteToCheck_up_to_PageSize = hw25q->PageSize - OffsetInByte;

	/* Compute the Address */
	WorkAddress = (OffsetInByte + (Page_Address * hw25q->PageSize));

	/* Verify if page is empty */
	status = _w25qxx_verify_empty(hw25q, NumByteToCheck_up_to_PageSize, WorkAddress);

	/* Unlock the Instance */
	hw25q->Lock = 0;

	/* Return the Operation Status */
	return status;
}

W25QXX_Status_t W25qxx_IsEmptySector(W25QXX_Handler_t *hw25q, uint32_t Sector_Address, uint32_t OffsetInByte, uint32_t NumByteToCheck_up_to_SectorSize)
{
	/* Local Variables */
	uint32_t WorkAddress;
	W25QXX_Status_t status;

	/* Check if the instance is being used */
	if(hw25q->Lock == 1)
		return W25QXX_BUSY;

	/* Compute the Number of Bytes to read */
	if((NumByteToCheck_up_to_SectorSize > hw25q->SectorSize) || (NumByteToCheck_up_to_SectorSize == 0))
		NumByteToCheck_up_to_SectorSize = hw25q->SectorSize;

	/* Lock the instance */
	hw25q->Lock = 1;

	/* Compute the Address */
	WorkAddress = (OffsetInByte + (Sector_Address * hw25q->SectorSize));

	/* Verify if Sector is empty */
	status = _w25qxx_verify_empty(hw25q, NumByteToCheck_up_to_SectorSize, WorkAddress);

	/* Unlock the Instance */
	hw25q->Lock = 0;

	/* Return the Operation Status */
	return status;
}

W25QXX_Status_t W25qxx_WriteBytes(W25QXX_Handler_t *hw25q, uint8_t *pBuffer, uint32_t WriteAddress, uint32_t Size, W25QXX_ByteOrder_t ByteOrder)
{
	/* Local Variables */
	W25QXX_Status_t status = W25QXX_OK;
	uint32_t count = 0, temp = 0;

	/* Check if the instance is being used */
	if(hw25q->Lock == 1)
		return W25QXX_BUSY;

	/* Lock the instance */
	hw25q->Lock = 1;

	/* Wait for the Write operation to be completed */
	_w25qxx_wait_write_end(hw25q);

	/* Check if the Address is page aligned */
	if((WriteAddress % hw25q->PageSize) == 0)	// Page aligned
	{
		/* Program data by 256 bytes */
		while(Size > 0)
		{
			if(Size >= hw25q->PageSize)
				count = hw25q->PageSize;	// Program a page size
			else
				count = Size;	// Program the remaining data

			/* Write the Page */
			_w25qxx_write_data(hw25q, pBuffer, count, WriteAddress, ByteOrder);

			/* Decrease the size */
			Size -= count;

			/* Increase the Address and buffer pointer */
			WriteAddress += count;
			pBuffer+=count;

		}
	}
	else	// Not page aligned
	{
		/* Compute the number of bytes until the end of page */
		temp = hw25q->PageSize - (WriteAddress % hw25q->PageSize);

		/* Check if size is smaller then the remaining page data */
		if(Size < temp)
		{
			/* Write the data until the end of page */
			_w25qxx_write_data(hw25q, pBuffer, Size, WriteAddress, ByteOrder);
		}
		else
		{
			/* Write the data until the end of page */
			_w25qxx_write_data(hw25q, pBuffer, temp, WriteAddress, ByteOrder);

			/* Decrement the memory size */
			Size -= temp;

			/* Increase the Address */
			WriteAddress += temp;

			/* Increase the Buffer */
			pBuffer += temp;

			while(Size > 0)
			{
				if(Size >= hw25q->PageSize)
					count = hw25q->PageSize;	// Program a page size
				else
					count = Size;	// Program the remaining data

				/* Write the Page */
				_w25qxx_write_data(hw25q, pBuffer, count, WriteAddress, ByteOrder);

				/* Decrease the size */
				Size -= count;

				/* Increase the Address */
				WriteAddress += count;

				/* Increase the Buffer */
				pBuffer += count;
			}
		}
	}

	/* Unlock the Instance */
	hw25q->Lock = 0;

	/* Return the Operation Status */
	return status;
}

#ifdef W25Q_USE_DMA
W25QXX_Status_t W25qxx_Write256BytesDMA(W25QXX_Handler_t *hw25q, uint8_t *pBuffer, uint32_t WriteAddress, uint32_t Size)
{
	/* Local Variables */
	W25QXX_Status_t status = W25QXX_OK;

	/* Check if the instance is being used */
	if(hw25q->Lock == 1)
		return W25QXX_BUSY;

	/* Check if DMA is available */
	if(hw25q->spi->hdmatx == 0x00)
		return W25QXX_ERROR;

	/* Check if the limit size is being respected */
	if(Size > 256)
		return W25QXX_ERROR;

	/* Lock the instance */
	hw25q->Lock = 1;

	/* Wait for the Write operation to be completed */
	_w25qxx_wait_write_end(hw25q);

	/* Enable the memory write */
	_w25qxx_write_enable(hw25q);

	/* Drive the CS Pin LOW */
	__W25_CS_LOW(hw25q->GPIO_CS_Port, hw25q->GPIO_CS_Pin);

	/* Send the Program Command */
	_w25qxx_spi(hw25q, _W25QXX_COMMAND_PAGE_PROGRAM);

	/* Send the address */
	_w25qxx_send_address(hw25q, WriteAddress);

	/* Check if the size isn't a multiple of two */
	if(Size % 2)
		hw25q->spi->Instance->CR2 |= SPI_CR2_LDMATX;

	/* Enable the Error and Transfer Complete Interrupts */
	hw25q->spi->hdmatx->Instance->CCR |= (DMA_CCR_TCIE | DMA_CCR_TEIE);

	/* Set the memory address in DMA */
	hw25q->spi->hdmatx->Instance->CMAR = (uint32_t)pBuffer;

	/* Set the Transfer size */
	hw25q->spi->hdmatx->Instance->CNDTR = Size;

	/* Enable the Memory Increment for TX */
	hw25q->spi->hdmatx->Instance->CCR |= DMA_CCR_MINC;

	/* Enable the transfer */
	hw25q->spi->hdmatx->Instance->CCR |= DMA_CCR_EN;
	hw25q->spi->Instance->CR2 |= SPI_CR2_TXDMAEN;

	/* Return the result of the operation */
	return status;
}
#endif

W25QXX_Status_t W25qxx_ReadBytes(W25QXX_Handler_t *hw25q, uint8_t *pBuffer,uint32_t ReadAddr,uint32_t NumByteToRead)
{
	/* Local Variables */
	uint8_t status = W25QXX_OK;

	/* Check if the instance is being used */
	if(hw25q->Lock == 1)
		return W25QXX_BUSY;

	/* Lock the instance */
	hw25q->Lock = 1;

	/* Drive the CS Pin LOW */
	__W25_CS_LOW(hw25q->GPIO_CS_Port, hw25q->GPIO_CS_Pin);

	/* Send the fast Read Command */
	_w25qxx_spi(hw25q, _W25QXX_COMMAND_FAST_READ);

	/* Send the address */
	_w25qxx_send_address(hw25q, ReadAddr);

	/* Send a Dummy data */
	_w25qxx_spi(hw25q, _W25QXX_DUMMY);

	/* Read the data */
	for(uint32_t i = 0; i < NumByteToRead; i ++)
	{
		/* Store the read data */
		*pBuffer++ = _w25qxx_spi(hw25q, _W25QXX_DUMMY);
	}

	/* Drive the CS Pin High */
	__W25_CS_HIGH(hw25q->GPIO_CS_Port, hw25q->GPIO_CS_Pin);

	/* Unlock the Instance */
	hw25q->Lock = 0;

	/* Return the Operation Status */
	return status;
}

#ifdef W25Q_USE_DMA
W25QXX_Status_t W25qxx_ReadBytesDMA(W25QXX_Handler_t *hw25q, uint8_t *pBuffer, uint32_t ReadAddr, uint32_t NumbyteToRead)
{
	/* Local Variables */
	W25QXX_Status_t status = W25QXX_OK;

	/* Check if the instance is being used */
	if(hw25q->Lock == 1)
		return W25QXX_BUSY;

	/* Check if DMA is available */
	if(hw25q->spi->hdmarx == 0x00 || hw25q->spi->hdmatx == 0x00)
		return W25QXX_ERROR;

	/* Check if the transfer length is greater then the maximum DMA Transfer */
	if(NumbyteToRead > 16535)
		return W25QXX_ERROR;

	/* Lock the instance */
	hw25q->Lock = 1;

	/* Drive the CS Pin LOW */
	__W25_CS_LOW(hw25q->GPIO_CS_Port, hw25q->GPIO_CS_Pin);

	/* Send the fast Read Command */
	_w25qxx_spi(hw25q, _W25QXX_COMMAND_FAST_READ);

	/* Send the address */
	_w25qxx_send_address(hw25q, ReadAddr);

	/* Send a Dummy data */
	_w25qxx_spi(hw25q, _W25QXX_DUMMY);

	/* Check if the number of bytes to be read isn't a multiple of two */
	if(NumbyteToRead % 2)
		hw25q->spi->Instance->CR2 |= SPI_CR2_LDMARX | SPI_CR2_LDMATX;

	/* Enable the SPI DMA RX/TX request */
	hw25q->spi->Instance->CR2 |= SPI_CR2_RXDMAEN | SPI_CR2_TXDMAEN;

	/* Enable the Error and Transfer Complete Interrupts */
	hw25q->spi->hdmarx->Instance->CCR |= (DMA_CCR_TCIE | DMA_CCR_TEIE);
	hw25q->spi->hdmatx->Instance->CCR |= (DMA_CCR_TCIE | DMA_CCR_TEIE);

	/* Set the memory address in DMA */
	hw25q->spi->hdmarx->Instance->CMAR = (uint32_t)pBuffer;
	hw25q->spi->hdmatx->Instance->CMAR = (uint32_t)&dummy;

	/* Disable the Memory Increment for TX */
	hw25q->spi->hdmatx->Instance->CCR &= ~DMA_CCR_MINC;

	/* Set the Transfer size */
	hw25q->spi->hdmarx->Instance->CNDTR = NumbyteToRead;
	hw25q->spi->hdmatx->Instance->CNDTR = NumbyteToRead;

	/* Enable the Transfer */
	hw25q->spi->hdmarx->Instance->CCR |= DMA_CCR_EN;
	hw25q->spi->hdmatx->Instance->CCR |= DMA_CCR_EN;

	/* Return the result of the operation */
	return status;
}
#endif

W25QXX_Status_t W25qxx_Verify(W25QXX_Handler_t *hw25q, uint8_t *pBuffer, uint32_t ReadAddr, uint32_t NumByteToRead, W25QXX_ByteOrder_t ByteOrder)
{
	/* Local Variables */
	uint8_t status = W25QXX_OK;
	uint8_t temp, *ptemp = pBuffer, counter = 3;

	/* Check if the instance is being used */
	if(hw25q->Lock == 1)
		return W25QXX_BUSY;

	/* Lock the instance */
	hw25q->Lock = 1;

	/* Drive the CS Pin LOW */
	__W25_CS_LOW(hw25q->GPIO_CS_Port, hw25q->GPIO_CS_Pin);

	/* Send the fast Read Command */
	_w25qxx_spi(hw25q, _W25QXX_COMMAND_FAST_READ);

	/* Send the address */
	_w25qxx_send_address(hw25q, ReadAddr);

	/* Send a Dummy data */
	_w25qxx_spi(hw25q, _W25QXX_DUMMY);

	/* Check the Byte Order */
	if(ByteOrder == W25QXX_DIRECT)
	{
		/* Read the data */
		for(uint32_t i = 0; i < NumByteToRead; i ++)
		{
			temp = _w25qxx_spi(hw25q, _W25QXX_DUMMY);

			/* Verify the read data */
			if((*ptemp) != temp)
			{
				status = W25QXX_ERROR;
				break;
			}

			ptemp++;
		}
	}
	else if(ByteOrder == W25QXX_REVERSE)
	{
		/* Read the data */
		for(uint32_t i = 0; i < NumByteToRead; i ++)
		{
			temp = _w25qxx_spi(hw25q, _W25QXX_DUMMY);

			/* Verify the read data */
			if(*(ptemp + counter) != temp)
			{
				status = W25QXX_ERROR;
				break;
			}
			else if(counter == 0)
			{
				counter = 4;
				ptemp += 4;
			}
			counter--;
		}
	}

	/* Drive the CS Pin High */
	__W25_CS_HIGH(hw25q->GPIO_CS_Port, hw25q->GPIO_CS_Pin);

	/* Unlock the Instance */
	hw25q->Lock = 0;

	/* Return the Operation Status */
	return status;
}

W25QXX_Status_t W25QXX_WaitWriteEnd(W25QXX_Handler_t *hw25q)
{
	/* Check if the instance is being used */
	if(hw25q->Lock == 1)
		return W25QXX_BUSY;

	/* Lock the instance */
	hw25q->Lock = 1;

	/* Wait for the Write operation to be completed */
	_w25qxx_wait_write_end(hw25q);

	/* Unlock the Instance */
	hw25q->Lock = 0;

	/* Return the result of the operation */
	return W25QXX_OK;
}
