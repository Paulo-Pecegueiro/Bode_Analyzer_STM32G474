/**  GT9147 - Touch controller driver for STM32
 *  GT9147.h
 *
 *  Author: Daniel Botelho
 *  Author: Paulo Pecegueiro
 *  Brazil, Aug 2023
 */

#ifndef __GT9147_H__
#define __GT9147_H__
/* Exported includes --------------------------------------------------------- */
#include "main.h"

/* Exported Types ------------------------------------------------------------- */
typedef struct 
{
	uint8_t 	touch;
	uint16_t 	x;
	uint16_t 	y;
}LcdTouchInfo;

/* Exported defines ----------------------------------------------------------- */
#define GT9147_MAX_TOUCH_NUM    5

#define GT9147_OK               0x00
#define GT9147_ERROR            0x01

/* Exported variables---------------------------------------------------------- */
extern LcdTouchInfo sLcdTouchInfos[GT9147_MAX_TOUCH_NUM];
extern FlagStatus ICAddress;

/* Exported Function Prototypes ----------------------------------------------- */
void GT9147_Reset(void);
uint8_t GT9147_Init(void);
uint8_t GT9147_Scan(void);

#endif // __GT9147_H__
