/*
 * fat_if.h
 *
 *  Created on: Sep 14, 2023
 *      Author: daniel, paulo
 */

#ifndef FAT_CUSTOM_INC_FAT_IF_H_
#define FAT_CUSTOM_INC_FAT_IF_H_
#ifdef __cplusplus
extern "C" {
#endif
/* Exported includes --------------------------------------------------------- */
#include "main.h"

/* Exported defines ---------------------------------------------------------- */
#define FAT_IF_MEM_BLOCK_SIZE				0x1000

/* Exported Types ------------------------------------------------------------- */
typedef enum
{
	FAT_IF_STATUS_OK = 0,
	FAT_IF_STATUS_ERROR,
	FAT_IF_STATUS_EMPTY,
	FAT_IF_STATUS_NOTEMPTY
}FAT_IF_Status_t;


/* Exported Function Prototypes ---------------------------------------------- */
FAT_IF_Status_t FAT_IF_MemoryWrite(uint8_t *pSource, uint8_t *pDestination, uint32_t Size);
FAT_IF_Status_t FAT_IF_MemoryRead(uint8_t *pSource, uint8_t *pDestination, uint32_t Size);
FAT_IF_Status_t FAT_IF_IsEmptyBlock(uint32_t BlockAddress);
FAT_IF_Status_t FAT_IF_MemoryVerify();
FAT_IF_Status_t FAT_IF_EraseBlock(uint32_t BlockAddress);

#ifdef __cplusplus
}
#endif	//__cplusplus
#endif /* FAT_CUSTOM_INC_FAT_IF_H_ */
