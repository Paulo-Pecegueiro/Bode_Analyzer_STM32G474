/*
 * app_filesystem.h
 *
 *  Created on: Dec 3, 2023
 *      Author: daniel, paulo
 */

#ifndef INC_APP_FILESYSTEM_H_
#define INC_APP_FILESYSTEM_H_

/* Exported includes */
#include "main.h"
#include "fat.h"

/* Exported Function Prototypes */
void APP_FileSystemInit(void);
void APP_FormatFS(void);
void APP_ReadFileContent(void);
void APP_WriteFileContent(void);

/* Exported variables */
extern const uint8_t FileHeader[];

#endif /* INC_APP_FILESYSTEM_H_ */
