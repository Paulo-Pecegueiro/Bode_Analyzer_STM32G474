/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef NT35510_REG_H
#define NT35510_REG_H

#ifdef __cplusplus
extern "C" {
#endif 

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* List of NT35510 used commands                                  */
/* Detailed in NT35510 Data Sheet v0.80                           */
/* Version of 10/28/2011                                          */
/* Command, NumberOfArguments                                     */
#define  NT35510_CMD_NOP                   0x0000  /* NOP */
#define  NT35510_CMD_SWRESET               0x0100  /* SW reset */
#define  NT35510_CMD_RDDID                 0x0400  /* Read display ID */
#define  NT35510_CMD_RDNUMED               0x0500  /* Read number of errors on DSI */
#define  NT35510_CMD_RDDPM                 0x0A00  /* Read display power mode */
#define  NT35510_CMD_RDDMADCTL             0x0B00  /* Read display MADCTL */
#define  NT35510_CMD_RDDCOLMOD             0x0C00  /* Read display pixel format */
#define  NT35510_CMD_RDDIM                 0x0D00  /* Read display image mode */
#define  NT35510_CMD_RDDSM                 0x0E00  /* Read display signal mode */
#define  NT35510_CMD_RDDSDR                0x0F00  /* Read display self-diagnostics result */
#define  NT35510_CMD_SLPIN                 0x1000  /* Sleep in */
#define  NT35510_CMD_SLPOUT                0x1100  /* Sleep out */
#define  NT35510_CMD_PTLON                 0x1200  /* Partial mode on  */
#define  NT35510_CMD_NORON                 0x1300  /* Normal display mode on */
#define  NT35510_CMD_INVOFF                0x2000  /* Display inversion off */
#define  NT35510_CMD_INVON                 0x2100  /* Display inversion on */
#define  NT35510_CMD_ALLPOFF               0x2200  /* All pixel off */
#define  NT35510_CMD_ALLPON                0x2300  /* All pixel on */
#define  NT35510_CMD_GAMSET                0x2600  /* Gamma set */
#define  NT35510_CMD_DISPOFF               0x2800  /* Display off */
#define  NT35510_CMD_DISPON                0x2900 /* Display on */
#define  NT35510_CMD_CASET                 0x2A00  /* Column address set */
#define  NT35510_CMD_RASET                 0x2B00  /* Row address set */
#define  NT35510_CMD_RAMWR                 0x2C00  /* Memory write */
#define  NT35510_CMD_RAMRD                 0x2E00  /* Memory read  */
#define  NT35510_CMD_PLTAR                 0x3000  /* Partial area */
#define  NT35510_CMD_TOPC                  0x3200  /* Turn On Peripheral Command */
#define  NT35510_CMD_TEOFF                 0x3400  /* Tearing effect line off */
#define  NT35510_CMD_TEEON                 0x3500  /* Tearing effect line on */
#define  NT35510_CMD_MADCTL                0x3600  /* Memory data access control */
#define  NT35510_CMD_IDMOFF                0x3800  /* Idle mode off */
#define  NT35510_CMD_IDMON                 0x3900  /* Idle mode on */
#define  NT35510_CMD_COLMOD                0x3A00  /* Interface pixel format */
#define  NT35510_CMD_RAMWRC                0x3C00  /* Memory write continue */
#define  NT35510_CMD_RAMRDC                0x3E00  /* Memory read continue */
#define  NT35510_CMD_STESL                 0x4400  /* Set tearing effect scan line */
#define  NT35510_CMD_GSL                   0x4500  /* Get scan line */

#define  NT35510_CMD_DSTBON                0x4F00  /* Deep standby mode on */
#define  NT35510_CMD_WRPFD                 0x5000  /* Write profile value for display */
#define  NT35510_CMD_WRDISBV               0x5100  /* Write display brightness */
#define  NT35510_CMD_RDDISBV               0x5200  /* Read display brightness */
#define  NT35510_CMD_WRCTRLD               0x5300  /* Write CTRL display */
#define  NT35510_CMD_RDCTRLD               0x5400  /* Read CTRL display value */
#define  NT35510_CMD_WRCABC                0x5500  /* Write content adaptative brightness control */
#define  NT35510_CMD_RDCABC                0x5600  /* Read content adaptive brightness control */
#define  NT35510_CMD_WRHYSTE               0x5700  /* Write hysteresis */
#define  NT35510_CMD_WRGAMMSET             0x5800  /* Write gamma setting */
#define  NT35510_CMD_RDFSVM                0x5A00  /* Read FS value MSBs */
#define  NT35510_CMD_RDFSVL                0x5B00  /* Read FS value LSBs */
#define  NT35510_CMD_RDMFFSVM              0x5C00  /* Read median filter FS value MSBs */
#define  NT35510_CMD_RDMFFSVL              0x5D00  /* Read median filter FS value LSBs */
#define  NT35510_CMD_WRCABCMB              0x5E00  /* Write CABC minimum brightness */
#define  NT35510_CMD_RDCABCMB              0x5F00  /* Read CABC minimum brightness */
#define  NT35510_CMD_WRLSCC                0x6500  /* Write light sensor compensation coefficient value */
#define  NT35510_CMD_RDLSCCM               0x6600  /* Read light sensor compensation coefficient value MSBs */
#define  NT35510_CMD_RDLSCCL               0x6700  /* Read light sensor compensation coefficient value LSBs */
#define  NT35510_CMD_RDBWLB                0x7000  /* Read black/white low bits */
#define  NT35510_CMD_RDBKX                 0x7100  /* Read Bkx */
#define  NT35510_CMD_RDBKY                 0x7200  /* Read Bky */
#define  NT35510_CMD_RDWX                  0x7300  /* Read Wx */
#define  NT35510_CMD_RDWY                  0x7400  /* Read Wy */
#define  NT35510_CMD_RDRGLB                0x7500  /* Read red/green low bits */
#define  NT35510_CMD_RDRX                  0x7600  /* Read Rx */
#define  NT35510_CMD_RDRY                  0x7700  /* Read Ry */
#define  NT35510_CMD_RDGX                  0x7800  /* Read Gx */
#define  NT35510_CMD_RDGY                  0x7900  /* Read Gy */
#define  NT35510_CMD_RDBALB                0x7A00  /* Read blue/acolor low bits */
#define  NT35510_CMD_RDBX                  0x7B00  /* Read Bx */
#define  NT35510_CMD_RDBY                  0x7C00  /* Read By */
#define  NT35510_CMD_RDAX                  0x7D00  /* Read Ax */
#define  NT35510_CMD_RDAY                  0x7E00  /* Read Ay */
#define  NT35510_CMD_RDDDBS                0xA100  /* Read DDB start */
#define  NT35510_CMD_RDDDBC                0xA800  /* Read DDB continue */
#define  NT35510_CMD_RDDCS                 0xAA00  /* Read first checksum */
#define  NT35510_CMD_RDCCS                 0xAF00  /* Read continue checksum */
#define  NT35510_CMD_RDID1                 0xDA00  /* Read ID1 value */
#define  NT35510_CMD_RDID2                 0xDB00  /* Read ID2 value */
#define  NT35510_CMD_RDID3                 0xDC00  /* Read ID3 value */

/* Parameter TELOM : Tearing Effect Line Output Mode : possible values */
#define NT35510_TEEON_TELOM_VBLANKING_INFO_ONLY            0x00
#define NT35510_TEEON_TELOM_VBLANKING_AND_HBLANKING_INFO   0x01

/* Possible used values of MADCTR */
#define NT35510_MADCTR_MODE_PORTRAIT       0x00
#define NT35510_MADCTR_MODE_LANDSCAPE      0x60  /* MY = 0, MX = 1, MV = 1, ML = 0, RGB = 0 */

/* Possible values of COLMOD parameter corresponding to used pixel formats */
#define  NT35510_COLMOD_RGB565             0x55
#define  NT35510_COLMOD_RGB888             0x77


/* Exported Types ------------------------------------------------------------*/
typedef struct
{
	uint16_t LCD_REG;
	uint16_t LCD_RAM;
} LCD_TypeDef;

/* Exported macro ------------------------------------------------------------*/

/* Exported variables --------------------------------------------------------*/
extern FlagStatus __isTranmittingBlock;

/* Exported functions --------------------------------------------------------*/
void 		LCD_WR_REG(uint16_t data);
void 		LCD_WR_DATA(uint16_t data);
uint16_t 	LCD_RD_DATA(void);

void 		NT35510_Init(void);
void 		NT35510_SetWindow(uint16_t x, uint16_t y, uint16_t sizex, uint16_t sizey);
uint16_t	NT35510_GetLine(void);
void 		NT35510_DMAWrite(uint16_t *pixels, uint16_t x, uint16_t y, uint16_t sizex, uint16_t sizey);

void 		NT35510_FadeIn(uint8_t Delay);
void 		NT35510_FadeOut(uint8_t Delay);


#ifdef __cplusplus
}
#endif

#endif /* NT35510_REG_H */
