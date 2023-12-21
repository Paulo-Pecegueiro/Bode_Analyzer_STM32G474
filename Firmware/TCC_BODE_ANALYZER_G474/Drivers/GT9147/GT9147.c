/**  GT9147 - Touch controller driver for STM32
 *  GT9147.c
 *
 *  Author: Daniel Botelho
 *  Author: Paulo Pecegueiro
 *  Brazil, Aug 2023
 */
/* Private includes ----------------------------------------------------------- */
#include "GT9147.h"
#include "i2c.h"
#include "cmsis_os2.h"

/* Private defines ------------------------------------------------------------ */
#define GT9147_CMD_WR               0x28        			//0xB8
#define GT9147_CMD_RD               0x29        			//0xB9
#define GT9147_CMD_ADD              GT9147_CMD_WR
#define GT9147_CMD_ADD2				0xBA

#define GT9147_ID                   0x37343139				//"9147"

#define GT9147_CTRL_REG 	        0x8040   				//GT9147 Control register
#define GT9147_CFGS_REG 	        0x80407   				//GT9147 Configure the start address register
#define GT9147_CHECK_REG 	        0x80FF   				//GT9147 Checksum register
#define GT9147_PID_REG 		        0x8140   				//GT9147 Product ID register

#define GT9147_GSTID_REG 	        0x814E   				//GT9147 Currently detected touch conditions
#define GT9147_TP1_REG 		        0x8150  				//Touch point data address 1
#define GT9147_TP2_REG 		        0x8158      			//Touch point data address 2
#define GT9147_TP3_REG 		        0x8160	    			//Touch point data address 3
#define GT9147_TP4_REG 		        0x8168	    			//Touch point data address 4
#define GT9147_TP5_REG 		        0x8170	    			//Touch point data address 5

#define GT9147_RST_PORT             TRST_GPIO_Port
#define GT9147_RST_PIN				TRST_Pin
#define GT9147_INT_PORT				TOUCH_PEN_GPIO_Port
#define GT9147_INT_PIN				TOUCH_PEN_Pin
#define GT9147_I2C					hi2c4

#define GT9147_TIMEOUT              0xFF

/* Private macros ------------------------------------------------------------- */
#define GT_RST_LOW()                GT9147_RST_PORT->BSRR |= (GT9147_RST_PIN << 16)
#define GT_RST_HIGH()               GT9147_RST_PORT->BSRR |= GT9147_RST_PIN

#define GT_INT_PIN()                ((GT9147_INT_PORT->IDR) & GT9147_INT_PIN)

/* Private variables ---------------------------------------------------------- */
LcdTouchInfo sLcdTouchInfos[GT9147_MAX_TOUCH_NUM];
FlagStatus ICAddress = RESET;

const uint16_t GT9147_TPX_TBL[GT9147_MAX_TOUCH_NUM] = 
{
		GT9147_TP1_REG,
		GT9147_TP2_REG,
		GT9147_TP3_REG,
		GT9147_TP4_REG,
		GT9147_TP5_REG
};

/* Private Function Prototypes ------------------------------------------------ */
static void GT_msDelay(uint16_t time);
static HAL_StatusTypeDef GT_I2C_WR_Reg(uint8_t *buf, uint8_t len);
static HAL_StatusTypeDef GT_I2C_RD_Reg(uint8_t *buf, uint8_t len);

/* Private Functions ---------------------------------------------------------- */
static void GT_msDelay(uint16_t time)
{
	/* Custom delay function */
	HAL_Delay(time);
}

static HAL_StatusTypeDef GT_I2C_WR_Reg(uint8_t *buf, uint8_t len)
{
	/* Write register through I2C */
	if(ICAddress == SET)
		return HAL_I2C_Master_Transmit(&GT9147_I2C, GT9147_CMD_ADD, buf, len, GT9147_TIMEOUT);
	else
		return HAL_I2C_Master_Transmit(&GT9147_I2C, GT9147_CMD_ADD2, buf, len, GT9147_TIMEOUT);
}

static HAL_StatusTypeDef GT_I2C_RD_Reg(uint8_t *buf, uint8_t len)
{
	/* Read register through I2C */
	if(ICAddress == SET)
		return HAL_I2C_Master_Receive(&GT9147_I2C, GT9147_CMD_ADD, buf, len, GT9147_TIMEOUT);
	else
		return HAL_I2C_Master_Receive(&GT9147_I2C, GT9147_CMD_ADD2, buf, len, GT9147_TIMEOUT);
}

/* Exported Functions --------------------------------------------------------- */
void GT9147_Reset(void)
{
	/* Hardware Reset */
	GT_RST_LOW();
	GT_msDelay(200);
	GT_RST_HIGH();
}

uint8_t GT9147_Init(void)
{   
	/* Local variables*/
	uint8_t temp[5] = {0, 0, 0, 0, 0};
	uint8_t reg[2] = {GT9147_PID_REG >> 8, GT9147_PID_REG & 0xFF};
	//	HAL_StatusTypeDef status;

	/* Perform a HW reset */
	GT9147_Reset();
	GT_msDelay(100);

	/* Read the Product ID Reg */
	GT_I2C_WR_Reg((uint8_t*)&reg, 2);
	GT_I2C_RD_Reg(temp, 4);

	/* Check the device ID */
	if(*(uint32_t*)temp != GT9147_ID)
	{
		return GT9147_ERROR;
	}

	return GT9147_OK;
}

uint8_t GT9147_Scan(void)
{
	/* Local Variables */
	uint8_t state,
			touchCount = 0,
			data[4],
			reg[3] = {GT9147_GSTID_REG >> 8, GT9147_GSTID_REG & 0xFF, 0};

	/* Read the Touch status */
	GT_I2C_WR_Reg((uint8_t *)&reg, 2);
	GT_I2C_RD_Reg(&state, 1);

	/* Check if there is a touch */
	if(state & 0x80)
	{
		GT_I2C_WR_Reg((uint8_t *)&reg, 3); //Clear the touch flag
	}

	/* Clear all the old stored touches */
	for(int i = 0; i < GT9147_MAX_TOUCH_NUM; i++)
	{
		sLcdTouchInfos[i].touch = 0;
		sLcdTouchInfos[i].x = 0;
		sLcdTouchInfos[i].y = 0;
	}

	touchCount = state & 0x0f;

	/* Read the touch values */
	if(touchCount > 0 && touchCount <= GT9147_MAX_TOUCH_NUM)
	{
		for(int i = 0; i < touchCount; i++)
		{
			reg[0] = GT9147_TPX_TBL[i] >> 8;
			reg[1] = GT9147_TPX_TBL[i] & 0xFF;
			GT_I2C_WR_Reg((uint8_t *)&reg, 2);
			GT_I2C_RD_Reg(data, 4);
			sLcdTouchInfos[i].touch = 1;
			sLcdTouchInfos[i].x = ((uint16_t)data[1] << 8) + (uint16_t)data[0];
			sLcdTouchInfos[i].y = ((uint16_t)data[3] << 8) + (uint16_t)data[2];
		}
	}

	return GT9147_OK;
}
