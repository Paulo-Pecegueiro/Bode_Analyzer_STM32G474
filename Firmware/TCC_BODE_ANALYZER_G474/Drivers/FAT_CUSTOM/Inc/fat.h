/*
 * fat.h
 *
 *  Created on: Sep 10, 2023
 *      Author: daniel, paulo
 */

#ifndef FAT_CUSTOM_FAT_H_
#define FAT_CUSTOM_FAT_H_

/* Exported includes --------------------------------------------------------- */
#include "main.h"

/* Exported defines ----------------------------------------------------------- */
#define MAX_NBR_OF_FILES		0x03


/* Exported Enums ------------------------------------------------------------- */
typedef enum
{
	FAT12 = 0,
	FAT16,
	FAT32,
	INVALID
}FAT_Type_t;


typedef enum
{
	ATTR_READ_ONLY	= 0x01,
	ATTR_HIDDEN = 0x02,
	ATTR_VOLUME_ID	= 0x08,
	ATTR_DIRECTORY	= 0x10,
	ATTR_ARCHIVE = 0x20
}FAT_File_Attributes_t;

typedef enum
{
	FAT_OK = 0,
	FAT_ERROR,
	FAT_INSUFICIENT_MEMORY,
	FAT_MOUNTED,
	FAT_NOT_MOUNTED,
	FAT_NOT_FORMATTED,
	FAT_FILE_ALREADY_EXIST,
	FAT_FILE_NOT_FOUND
}FAT_Status_t;

/* Exported Types ------------------------------------------------------------- */
typedef struct
{
	/* Boot Sector values */
	uint16_t 	BytesPerSector;
	uint8_t  	SecPerCluster;
	uint16_t 	NbrOfReservedSec;
	uint8_t  	NbrOfFats;
	uint16_t 	CntRootEntries;
	uint16_t	FATSize;
	uint16_t	TotalSectors;

	/* File System Auxiliary Variables */
	uint16_t	MemoryBlockSize;
	FAT_Type_t  FatType;
	uint16_t 	NbrOfRootDirSectors;
	uint16_t	NbrOfDataSectors;
	uint16_t	FileSystemSize;
	uint16_t	FirstRootDirSec;
	uint16_t 	FirstDataSector;
	uint16_t	CountOfClusters;
	uint8_t 	*FileSystemAddress;

	/* Control Variables */
	FAT_Status_t Status;
}FAT_HandlerStruct_t;

typedef struct
{
	uint16_t BytesPerSector;
	uint8_t  SectorsPerCluster;
	uint8_t  NbrOfFATs;
	uint8_t  SectorsPerFat;
	uint16_t NbrOfRootEntries;
	uint16_t MemorySize;
	uint16_t MemoryBlockSize;
	uint8_t  VolumeID[11];
	uint8_t *MemoryStartAddress;
}FAT_InitStruct_t;

/* Exported Variables -------------------------------------------------------- */
extern FAT_HandlerStruct_t myFAT;

/* Exported Function Prototypes ---------------------------------------------- */
FAT_Status_t FAT_Format(FAT_InitStruct_t *pFatInit);
FAT_Status_t FAT_Mount(FAT_HandlerStruct_t *pFatStruct, uint8_t *Address, uint16_t MemBlockSize);
FAT_Status_t FAT_CreateFile(FAT_HandlerStruct_t *pFatStruct, uint8_t *FileName, uint16_t FileSize);
FAT_Status_t FAT_EditFileContent(FAT_HandlerStruct_t *pFatStruct, uint8_t *FileName, uint8_t *pContent, uint16_t FileSize);
#endif /* FAT_CUSTOM_FAT_H_ */
