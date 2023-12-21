/*
 * fat.c
 *
 *  Created on: Sep 10, 2023
 *      Author: paulo
 */
/* Private includes ---------------------------------------------------------- */
#include <string.h>
#include "fat.h"
#include "fat_if.h"

/* Private defines ----------------------------------------------------------- */


/* Private Macros ------------------------------------------------------------ */


/* Private Types ------------------------------------------------------------- */
typedef struct
{
	uint16_t RootDirSectorsNbr;
	uint16_t DataSectorsNbr;
	uint16_t CountofClusters;
	uint32_t FATSz;
	uint32_t TotSec;
	uint16_t FirstRootDirSecNum;
}FAT__t;

typedef struct
{
	uint8_t BS_jmpBoot[3];
	uint8_t BS_OEMName[8];
	uint8_t BPB_BytsPerSec[2];
	uint8_t BPB_SecPerClus;
	uint8_t BPB_RsvdSecCnt[2];
	uint8_t BPB_NumFATs;
	uint8_t BPB_RootEntCnt[2];
	uint8_t BPB_TotSec16[2];
	uint8_t BPB_Media;
	uint8_t BPB_FATSz16[2];
	uint8_t BPB_SecPerTrk[2];
	uint8_t BPB_NumHeads[2];
	uint8_t BPB_HiddSec[4];
	uint8_t BPB_TotSec32[4];
	uint8_t BS_DrvNum;
	uint8_t BS_Reserved1;
	uint8_t BS_BootSig;
	uint8_t BS_VolID[4];
	uint8_t BS_VolLab[11];
	uint8_t BS_FileSysType[8];
	uint8_t Reserved[448];
	uint8_t Signature_word[2];
}FAT_BS_BPB_Block_t;

typedef struct
{
	uint8_t DIR_Name[11];
	FAT_File_Attributes_t DIR_Attr;
	uint8_t DIR_NTRes;
	uint8_t DIR_CrtTimeTenth;
	uint8_t DIR_CrtTime[2];
	uint8_t DIR_CrtDate[2];
	uint8_t DIR_LstAccDate[2];
	uint8_t DIR_FstClusHI[2];
	uint8_t DIR_WrtTime[2];
	uint8_t DIR_WrtDate[2];
	uint8_t DIR_FstClusLO[2];
	uint8_t DIR_FileSize[4];
}FAT_DIR_Struct_t;

typedef struct
{
	uint8_t LDIR_Ord;
	uint8_t LDIR_Nam1[10];
	uint8_t LDIR_Attr;
	uint8_t LDIR_Type;
	uint8_t LDIR_Chksum;
	uint8_t LDIR_Name2[12];
	uint8_t LDIR_FstClusLO[2];
	uint8_t LDIR_Name3[4];
}FAT_LDIR_Struct_t;

extern uint8_t buffer[16535];


/* Private Variables -------------------------------------------------------- */

/* Private Function Prototypes ---------------------------------------------- */
FAT_Status_t __ManageFatEntry(uint8_t *pFATSector, uint16_t ClusterNumber, uint32_t EntryValue, FAT_Type_t FatType, uint16_t BytesPerSec);
FAT_Status_t __ReadFatEntry(uint8_t *pFATSector, uint16_t ClusterNumber, uint32_t *pEntryValue, FAT_Type_t FatType, uint16_t BytesPerSec);
FAT_Status_t __GetSectorFromCluster();

/* Private Function --------------------------------------------------------- */
FAT_Status_t __WriteFatEntry(uint8_t *pFATSector, uint16_t ClusterNumber, uint32_t EntryValue, FAT_Type_t FatType, uint16_t BytesPerSec)
{
	/* Local Variables */
	uint16_t FATOffset, ThisFATEntOffset;

	switch(FatType)
	{
		case FAT12:
			FATOffset = ClusterNumber + (ClusterNumber / 2);
			ThisFATEntOffset = FATOffset % BytesPerSec;
			if(ClusterNumber & 0x001)
			{
				EntryValue = EntryValue << 4;
				*(uint16_t *)&pFATSector[ThisFATEntOffset] = *(uint16_t *)&pFATSector[ThisFATEntOffset] & 0x000F;
			}
			else
			{
				EntryValue = EntryValue & 0x0FFF;
				*(uint16_t *)&pFATSector[ThisFATEntOffset] = *(uint16_t *)&pFATSector[ThisFATEntOffset] & 0xF000;
			}

			*(uint16_t *)&pFATSector[ThisFATEntOffset] = *(uint16_t *)&pFATSector[ThisFATEntOffset] | (uint16_t)EntryValue;

			return FAT_OK;

		default:
			return FAT_IF_STATUS_ERROR;
	}
}

FAT_Status_t __ReadFatEntry(uint8_t *pFATSector, uint16_t ClusterNumber, uint32_t *pEntryValue, FAT_Type_t FatType, uint16_t BytesPerSec)
{
	/* Local Variables */
	uint16_t FATOffset, ThisFATEntOffset;

	switch(FatType)
	{
		case FAT12:
			FATOffset = ClusterNumber + (ClusterNumber / 2);
			ThisFATEntOffset = FATOffset % BytesPerSec;
			*(uint16_t *)pEntryValue = *(uint16_t *)&pFATSector[ThisFATEntOffset];

			if(ClusterNumber & 0x0001)
				*pEntryValue = *pEntryValue >> 4;
			else
				*pEntryValue = *pEntryValue & 0x0FFF;

			return FAT_OK;

		default:
			return FAT_IF_STATUS_ERROR;
	}
}

void Init(void)
{
	FAT__t myFAT;

	uint8_t str[] = "Gan,Fas\r\n10,20\r\n";
	uint8_t str2[] = "Gan,Fas\r\n50,30\r\n";
	uint8_t fileContent[1024];

	for(int i = 0; i < 32; i ++)
	{
		memcpy(&fileContent[i * 16], str, 16);
	}

	for(int i = 32; i < 64; i ++)
	{
		memcpy(&fileContent[i * 16], str2, 16);
	}

	FAT_BS_BPB_Block_t data =
	{
			{0xeb, 0x3C, 0x90},			// BS_jmpBoot
			{"FATDODAN"},				// BS_OEMName
			{0x00, 0x02},				// BPB_BytsPerSec
			0x01,						// BPB_SecPerClus
			{0x01, 0x00},				// BPB_RsvdSecCnt
			0x01,						// BPB_NumFATs
			{0x20, 0x00},				// BPB_RootEntCnt
			{0x08, 0x00},				// BPB_TotSec16
			0xF8,						// BPB_Media
			{0x01, 0x00},				// BPB_FATSz16
			{0x01, 0x00},				// BPB_SecPerTrk
			{0x01, 0x00},				// BPB_NumHeads
			{0x00, 0x00, 0x00, 0x00},	// BPB_HiddSec
			{0x00, 0x00, 0x00, 0x00},	// BPB_TotSec32
			0x80,						// BS_DrvNum
			0x00,						// BS_Reserved1
			0x29,						// BS_BootSig
			{0x00, 0x00, 0x00, 0x00},	// BS_VolID
			{"TCC DO DAN"},				// BS_VolLab
			{"FAT16   "},				// BS_FileSysType
			{0x00},
			{0x55, 0xAA}
	};

	myFAT.RootDirSectorsNbr = (((data.BPB_RootEntCnt[0] + (data.BPB_RootEntCnt[1] << 8)) * 32) + (512 - 1)) / 512;

	myFAT.FATSz = data.BPB_FATSz16[0] + (data.BPB_FATSz16[1] << 8);
	myFAT.TotSec = data.BPB_TotSec16[0] + (data.BPB_TotSec16[1] << 8);

	myFAT.DataSectorsNbr = myFAT.TotSec - (data.BPB_RsvdSecCnt[0] + (data.BPB_RsvdSecCnt[1] << 8) + (data.BPB_NumFATs * myFAT.FATSz) + myFAT.RootDirSectorsNbr);

	myFAT.CountofClusters = myFAT.DataSectorsNbr / data.BPB_SecPerClus;
	myFAT.FirstRootDirSecNum = data.BPB_RsvdSecCnt[0] + (data.BPB_RsvdSecCnt[1] << 8) + (data.BPB_NumFATs * data.BPB_FATSz16[0]);


	FAT_DIR_Struct_t volumeID =
	{
			"PENDRIVEDAN",
			ATTR_VOLUME_ID,
			0x00,
			0x00,
			{0x00, 0x00},
			{0x00, 0x00},
			{0x00, 0x00},
			{0x00, 0x00},
			{0x00, 0x00},
			{0x00, 0x00},
			{0x00, 0x00},
			{0x00, 0x00, 0x00, 0x00}
	};

	FAT_DIR_Struct_t myFile =
	{
			"MYFILE  CSV",
			ATTR_ARCHIVE,
			0x00,
			0x00,
			{0x00, 0x00},
			{0x00, 0x00},
			{0x00, 0x00},
			{0x00, 0x00},
			{0x00, 0x00},
			{0x00, 0x00},
			{0x03, 0x00},
			{0x00, 0x04, 0x00, 0x00}
	};

	//	FAT_DIR_Struct_t myFile2 =
	//	{
	//			"MYFILE2 CSV",
	//			ATTR_ARCHIVE,
	//			0x00,
	//			0x00,
	//			{0x00, 0x00},
	//			{0x00, 0x00},
	//			{0x00, 0x00},
	//			{0x00, 0x00},
	//			{0x00, 0x00},
	//			{0x00, 0x00},
	//			{0x03, 0x00},
	//			{0x00, 0x04, 0x00, 0x00}
	//	};

	uint8_t fat[] =
	{
			0xF8, // Clus0 x00
			0x0F, // Clus1 xx0 Clus0 0xx
			0x00, // Clus1 00x
			0x00, // Clus2 x00
			0x40, // Clus3 xx0 Clus2 0xx
			0x00, // Clus3 00x
			0xFF, // Clus4 x00
			0x0F, // Clus5 xx0 Clus4 0xx
			0x00, // Clus5 00x
			0x00, // Clus6 x00
			0x00, // Clus7 0xx Clus6 0xx
			0x00  // Clus7 x00
	};

	memcpy(buffer, &data, 512);
	memcpy(buffer + 512, fat, sizeof(fat));
	memcpy(buffer + (myFAT.FirstRootDirSecNum * 512), &volumeID, sizeof(FAT_DIR_Struct_t));
	memcpy(buffer + (myFAT.FirstRootDirSecNum * 512) + sizeof(FAT_DIR_Struct_t), &myFile, sizeof(FAT_DIR_Struct_t));
	//	memcpy(buffer + (myFAT.FirstRootDirSecNum * 512) + (2*sizeof(FAT_DIR_Struct_t)), &myFile2, sizeof(FAT_DIR_Struct_t));
	memcpy(buffer + ((myFAT.FirstRootDirSecNum + 3) * 512), fileContent, sizeof(fileContent));
	//	memcpy(buffer + ((myFAT.FirstRootDirSecNum + 4) * 512), fileContent + sizeof(fileContent)/2, sizeof(fileContent)/2);
}

/* Exported Functions -------------------------------------------------------- */
FAT_Status_t FAT_Format(FAT_InitStruct_t *pFatInit)
{
	/* Private Variables */
	uint16_t tmp, CountOfRootEntries, CountOfDataSectors, TotalSectors, CountOfClusters, NbrOfBlocks;

	FAT_BS_BPB_Block_t BootSector = {
			{0xeb, 0x3C, 0x90},			// BS_jmpBoot		OK
			{"MYFAT FS"},				// BS_OEMName		OK
			{0x00, 0x00},				// BPB_BytsPerSec	OK
			0x00,						// BPB_SecPerClus	OK
			{0x00, 0x00},				// BPB_RsvdSecCnt	OK
			0x00,						// BPB_NumFATs		OK
			{0x00, 0x00},				// BPB_RootEntCnt	OK
			{0x00, 0x00},				// BPB_TotSec16		OK
			0xF8,						// BPB_Media		OK
			{0x00, 0x00},				// BPB_FATSz16		OK
			{0x01, 0x00},				// BPB_SecPerTrk	OK
			{0x01, 0x00},				// BPB_NumHeads		OK
			{0x00, 0x00, 0x00, 0x00},	// BPB_HiddSec		OK
			{0x00, 0x00, 0x00, 0x00},	// BPB_TotSec32		OK
			0x80,						// BS_DrvNum		OK
			0x00,						// BS_Reserved1		OK
			0x29,						// BS_BootSig		OK
			{0x00, 0x00, 0x00, 0x00},	// BS_VolID			OK
			{"TCC-IFSP23"},				// BS_VolLab		OK
			{"FAT1    "},				// BS_FileSysType	OK
			{0x00},						// Reserved bytes	OK
			{0x55, 0xAA}				// Signature_word	OK
	};

	/* Compute the Adjustable Parameters */
	if(pFatInit->BytesPerSector > 512)
		return FAT_ERROR; // Not Yet Implemented


	BootSector.BPB_SecPerClus 					= pFatInit->SectorsPerCluster;
	BootSector.BPB_NumFATs 						= pFatInit->NbrOfFATs;
	*(uint16_t *)BootSector.BPB_BytsPerSec 		= pFatInit->BytesPerSector;
	*(uint16_t *)BootSector.BPB_RootEntCnt		= pFatInit->NbrOfRootEntries;
	*(uint16_t *)BootSector.BPB_FATSz16			= pFatInit->SectorsPerFat;


	/* Calculate the number of Root Entries */
	CountOfRootEntries 	= ((pFatInit->NbrOfRootEntries * 32) + (pFatInit->BytesPerSector - 1))/pFatInit->BytesPerSector;

	/* Calculate the number of Reserved Sectors to Align the Data Sectors to the Memory Block */
	if(pFatInit->MemorySize > pFatInit->MemoryBlockSize)
	{
		tmp = CountOfRootEntries + (pFatInit->NbrOfFATs * pFatInit->SectorsPerFat);
		*(uint16_t *)BootSector.BPB_RsvdSecCnt = (pFatInit->MemoryBlockSize)/pFatInit->BytesPerSector - tmp;
	}
	else
	{
		*(uint16_t *)BootSector.BPB_RsvdSecCnt = 1;
	}

	/* Compute the amount of Sectors available */
	TotalSectors = pFatInit->MemorySize / pFatInit->BytesPerSector;

	/* Compute the number of Data Sectors */
	CountOfDataSectors = TotalSectors - *(uint16_t *)BootSector.BPB_RsvdSecCnt - CountOfRootEntries - (BootSector.BPB_NumFATs * *(uint16_t *)BootSector.BPB_FATSz16);

	/* Compute the amount of Clusters */
	CountOfClusters = CountOfDataSectors / BootSector.BPB_SecPerClus;

	/* Check the File System Type */
	if(CountOfClusters < 4085)
	{
		BootSector.BS_FileSysType[4] = '2';
		*(uint16_t *)BootSector.BPB_TotSec16 = TotalSectors;
	}
	else if(CountOfClusters < 65525)
	{
		BootSector.BS_FileSysType[4] = '6';
		*(uint16_t *)BootSector.BPB_TotSec16 = TotalSectors;
	}
	else
	{
		return FAT_ERROR; // FAT32 Not yet implemented
	}

	/* Calculate the number of Blocks Needed for the Boot Sector + FATS + RootDirs */
	tmp = *(uint16_t *)BootSector.BPB_TotSec16 - CountOfDataSectors;
	NbrOfBlocks = tmp / (pFatInit->MemoryBlockSize / *(uint16_t *)BootSector.BPB_BytsPerSec);

	/* Use at least 1 block */
	if(!NbrOfBlocks)
		NbrOfBlocks++;

	/* Check if the needed blocks are erased to be written */
	for(uint8_t i = 0; i < NbrOfBlocks; i++)
	{
		if(FAT_IF_IsEmptyBlock((uint32_t)pFatInit->MemoryStartAddress + (i * pFatInit->MemoryBlockSize)) == FAT_IF_STATUS_NOTEMPTY)
			FAT_IF_EraseBlock((uint32_t)pFatInit->MemoryStartAddress + (i * pFatInit->MemoryBlockSize));
	}

	/* Write the Boot Sector */
	FAT_IF_MemoryWrite((uint8_t *)&BootSector, pFatInit->MemoryStartAddress, sizeof(BootSector));

	/* Prepare the FATs to be written */
	uint8_t FATSector[(*(uint16_t *)BootSector.BPB_FATSz16) * *(uint16_t *)BootSector.BPB_BytsPerSec];
	memset(FATSector, 0x00, sizeof(FATSector)); //Fill with zeros

	/* Prepare the first reserved Entry */
	if(BootSector.BS_FileSysType[4] == '2') //FAT 12
	{
		__WriteFatEntry(FATSector, 0, 0x0FF8, FAT12, *(uint16_t *)BootSector.BPB_BytsPerSec);
		__WriteFatEntry(FATSector, 1, 0x0FFF, FAT12, *(uint16_t *)BootSector.BPB_BytsPerSec);
	}
	else if(BootSector.BS_FileSysType[4] == '6') //FAT 16
		__WriteFatEntry(FATSector, 0, 0xFFF8, FAT16, *(uint16_t *)BootSector.BPB_BytsPerSec);

	/* Write the FAT */
	for(uint8_t i = *(uint16_t *)BootSector.BPB_RsvdSecCnt; i < (BootSector.BPB_NumFATs + *(uint16_t *)BootSector.BPB_RsvdSecCnt); i++)
	{
		FAT_IF_MemoryWrite(FATSector, pFatInit->MemoryStartAddress + (i * *(uint16_t *)BootSector.BPB_BytsPerSec), sizeof(FATSector));
	}

	/* Prepare the Device Volume name */
	FAT_DIR_Struct_t volumeID =	{{0x00}, ATTR_VOLUME_ID};

	/* Copy the name into the Struct */
	memcpy(volumeID.DIR_Name, pFatInit->VolumeID, sizeof(volumeID.DIR_Name));

	/* Get the First Root Dir */
	tmp = *(uint16_t *)BootSector.BPB_RsvdSecCnt + (BootSector.BPB_NumFATs * *(uint16_t *)BootSector.BPB_FATSz16);

	/* Write the Volume ID */
	FAT_IF_MemoryWrite((uint8_t *)&volumeID, pFatInit->MemoryStartAddress + (tmp * *(uint16_t *)BootSector.BPB_BytsPerSec), sizeof(volumeID));

	return FAT_OK;
}

FAT_Status_t FAT_Mount(FAT_HandlerStruct_t *pFatStruct, uint8_t *Address, uint16_t MemBlockSize)
{
	/* Private Variables */
	FAT_BS_BPB_Block_t BootSector;

	/* Read the content available in the Address */
	FAT_IF_MemoryRead(Address, (uint8_t *)&BootSector, sizeof(FAT_BS_BPB_Block_t));

	/* Check if the the FAT Struct wasn't already mounted */
	if(pFatStruct->Status == FAT_MOUNTED)
		return FAT_ERROR;

	/* Check if the memory region is formatted */
	if(__REV16(*(uint16_t *)BootSector.BS_jmpBoot) != 0xEB3C)
		return FAT_NOT_FORMATTED;

	/* Everything is OK, fill the struct */
	pFatStruct->BytesPerSector 			= *(uint16_t *)BootSector.BPB_BytsPerSec;
	pFatStruct->SecPerCluster 			= BootSector.BPB_SecPerClus;
	pFatStruct->NbrOfReservedSec 		= *(uint16_t *)BootSector.BPB_RsvdSecCnt;
	pFatStruct->NbrOfFats				= BootSector.BPB_NumFATs;
	pFatStruct->CntRootEntries			= *(uint16_t *)BootSector.BPB_RootEntCnt;
	pFatStruct->FATSize					= (*(uint16_t *)BootSector.BPB_FATSz16) * pFatStruct->BytesPerSector;
	pFatStruct->TotalSectors			= *(uint16_t *)BootSector.BPB_TotSec16;

	/* Compute the Auxiliary Variables */
	pFatStruct->NbrOfRootDirSectors 	= ((pFatStruct->CntRootEntries * 32) + (pFatStruct->BytesPerSector - 1))/pFatStruct->BytesPerSector;
	pFatStruct->NbrOfDataSectors		= pFatStruct->TotalSectors - pFatStruct->NbrOfReservedSec - (pFatStruct->NbrOfFats * pFatStruct->FATSize)/pFatStruct->BytesPerSector - pFatStruct->NbrOfRootDirSectors;
	pFatStruct->FileSystemSize			= pFatStruct->TotalSectors * pFatStruct->BytesPerSector;
	pFatStruct->CountOfClusters			= pFatStruct->NbrOfDataSectors / pFatStruct->SecPerCluster;
	pFatStruct->FileSystemAddress		= (uint8_t *)Address;
	pFatStruct->FirstRootDirSec			= pFatStruct->NbrOfReservedSec + (pFatStruct->NbrOfFats * *(uint16_t *)BootSector.BPB_FATSz16);
	pFatStruct->FirstDataSector			= pFatStruct->FirstRootDirSec + pFatStruct->NbrOfRootDirSectors;
	pFatStruct->MemoryBlockSize			= MemBlockSize;

	/* Compute the FAT type */
	if(*(uint16_t *)BootSector.BPB_TotSec32 != 0x00)
	{
		pFatStruct->FatType = FAT32;
	}
	else
	{
		if(pFatStruct->CountOfClusters < 4085)
		{
			pFatStruct->FatType = FAT12;
		}
		else if(pFatStruct->CountOfClusters < 65525)
		{
			pFatStruct->FatType = FAT16;
		}
		else
		{
			pFatStruct->FATSize = INVALID;
			return FAT_ERROR;
		}
	}

	/* Change the FAT status */
	pFatStruct->Status					= FAT_MOUNTED;

	return FAT_OK;
}

FAT_Status_t FAT_CreateFile(FAT_HandlerStruct_t *pFatStruct, uint8_t *FileName, uint16_t FileSize)
{
	/* Check if it's a valid FAT */
	if(pFatStruct->Status != FAT_MOUNTED)
		return FAT_NOT_MOUNTED;

	/* Local Variables */
	uint32_t EntryValue;
	uint8_t  NeededClusters = FileSize/(pFatStruct->BytesPerSector * pFatStruct->SecPerCluster),
			MemBlock[pFatStruct->MemoryBlockSize];

	FAT_DIR_Struct_t *file;

	if(NeededClusters == 0)
		NeededClusters++;

	uint16_t FreeClusters[NeededClusters], tmp = 0;

	/* Check if there is another file with same name */
	for(uint16_t i = 0; i < pFatStruct->NbrOfRootDirSectors; i++)
	{
		FAT_IF_MemoryRead(pFatStruct->FileSystemAddress + (i + pFatStruct->FirstRootDirSec) * pFatStruct->BytesPerSector, MemBlock, pFatStruct->BytesPerSector);

		/* Scan the files in the RootDirSector */
		for(uint16_t j = 0; j < pFatStruct->BytesPerSector / sizeof(FAT_DIR_Struct_t); j++)
		{
			file = (FAT_DIR_Struct_t *)&MemBlock[j * sizeof(FAT_DIR_Struct_t)];
			if(!memcmp(file->DIR_Name, FileName, 11))
				return FAT_FILE_ALREADY_EXIST;
		}
	}

	/* Read the FAT Sector */
	FAT_IF_MemoryRead(pFatStruct->FileSystemAddress, MemBlock, sizeof(MemBlock));

	/* Check if there are enough free space */
	for(uint16_t i = 2; i <= pFatStruct->CountOfClusters + 1; i++)
	{
		__ReadFatEntry(MemBlock + (pFatStruct->BytesPerSector * pFatStruct->NbrOfReservedSec), i, &EntryValue, pFatStruct->FatType, pFatStruct->BytesPerSector);
		/* Check if the cluster is empty */
		if(EntryValue == 0x0000)
		{
			/* Save the cluster number */
			FreeClusters[tmp] = i;
			tmp++;

			/* Check if all the needed free clusters was found */
			if(tmp == NeededClusters)
				break;
		}
	}

	/* Check if there are enough space for the file */
	if(tmp < NeededClusters)
		return FAT_INSUFICIENT_MEMORY;

	/* Construct the FAT Sector */
	for(uint16_t i = 0; i < NeededClusters - 1; i++)
	{
		__WriteFatEntry(MemBlock + (pFatStruct->BytesPerSector * pFatStruct->NbrOfReservedSec), FreeClusters[i], FreeClusters[i + 1], pFatStruct->FatType, pFatStruct->BytesPerSector);
	}
	__WriteFatEntry(MemBlock + (pFatStruct->BytesPerSector * pFatStruct->NbrOfReservedSec), FreeClusters[NeededClusters - 1], 0xFFFFFFFF, pFatStruct->FatType, pFatStruct->BytesPerSector);

	/* Search for an empty dir section in Root Dir Sector */
	for(uint16_t i = 0; i < pFatStruct->CntRootEntries; i++)
	{
		if(*(MemBlock + (pFatStruct->FirstRootDirSec * pFatStruct->BytesPerSector) + (32*i)) == 0xFF)
		{
			file = (FAT_DIR_Struct_t *)(MemBlock + ((pFatStruct->FirstRootDirSec * pFatStruct->BytesPerSector) + (32*i)));

			memset(file, 0x00, sizeof(FAT_DIR_Struct_t));
			memcpy(file->DIR_Name, FileName, 11);

			*(uint32_t *)file->DIR_FileSize = FileSize;
			*(uint16_t *)file->DIR_FstClusLO = FreeClusters[0];

			memset(file + 1, 0x00, sizeof(MemBlock) - (((uint32_t)file - (uint32_t)MemBlock) + sizeof(FAT_DIR_Struct_t)));

			break;
		}
	}

	/* Erase the Block before writing on that */
	FAT_IF_EraseBlock((uint32_t)pFatStruct->FileSystemAddress);

	/* Write the file into the Memory */
	FAT_IF_MemoryWrite(MemBlock, pFatStruct->FileSystemAddress, sizeof(MemBlock));

	/* Return the result of the operation */
	return FAT_OK;
}

FAT_Status_t FAT_EditFileContent(FAT_HandlerStruct_t *pFatStruct, uint8_t *FileName, uint8_t *pContent, uint16_t FileSize)
{
	/* Local Variables */
	ITStatus FileFound = RESET;
	FAT_DIR_Struct_t *file;
	uint8_t MemSector[pFatStruct->BytesPerSector];
	uint32_t Entry, tmp;

	/* Search the file */
	for(uint16_t i = 0; i < pFatStruct->NbrOfRootDirSectors; i++)
	{
		/* Get the Root Dir Sectors */
		FAT_IF_MemoryRead(pFatStruct->FileSystemAddress + (i + pFatStruct->FirstRootDirSec) * pFatStruct->BytesPerSector, MemSector, pFatStruct->BytesPerSector);

		/* Check if file exists */
		for(uint16_t j = 0; j < pFatStruct->BytesPerSector / sizeof(FAT_DIR_Struct_t); j++)
		{
			file = (FAT_DIR_Struct_t *)&MemSector[j * sizeof(FAT_DIR_Struct_t)];
			if(!memcmp(file->DIR_Name, FileName, 11))
			{
				FileFound = SET;
				break;
			}
		}

		if(FileFound == SET)
			break;
	}

	/* If file doesn't exists return error */
	if(FileFound == RESET)
		return FAT_FILE_NOT_FOUND;

	/* Compute the number of cluster occupied by the file */
	uint16_t NbrOfClusters = (*(uint32_t *)file->DIR_FileSize) / (pFatStruct->BytesPerSector * pFatStruct->SecPerCluster);

	/* Get the remaining data */
	if((*(uint32_t *)file->DIR_FileSize) % (pFatStruct->BytesPerSector * pFatStruct->SecPerCluster))
		NbrOfClusters++;

	/* Check if size is less then the cluster size */
	if(NbrOfClusters == 0)
		NbrOfClusters++;

	/* Allocate the buffer for the entries */
	uint16_t FileEntries[NbrOfClusters + 1];

	/* Save the First File Entry */
	FileEntries[0] = *(uint16_t *)file->DIR_FstClusLO;

	/* Read the FAT Sector */
	FAT_IF_MemoryRead(pFatStruct->FileSystemAddress + (pFatStruct->NbrOfReservedSec * pFatStruct->BytesPerSector), MemSector, pFatStruct->BytesPerSector);

	/* Search for the entries */
	for(uint16_t i = 0; i < NbrOfClusters + 1; i++)
	{
		__ReadFatEntry(MemSector, FileEntries[i], &Entry, pFatStruct->FatType, pFatStruct->BytesPerSector);
		FileEntries[i + 1] = (uint16_t)Entry;
	}

	/* Check if the last cluster is the end of the file */
	if(pFatStruct->FatType == FAT12)
		if(FileEntries[NbrOfClusters] != 0xfff)
			return FAT_ERROR;

	/* Write the File content */
	for(uint16_t i = 0; i < NbrOfClusters; i++)
	{
		//		if(FAT_IF_IsEmptyBlock(FileEntries[i] - 1))
		if(FileSize > (pFatStruct->BytesPerSector * pFatStruct->SecPerCluster))
			tmp = pFatStruct->BytesPerSector * pFatStruct->SecPerCluster;
		else
			tmp = FileSize;

		FAT_IF_EraseBlock((uint32_t)pFatStruct->FileSystemAddress + (pFatStruct->FirstDataSector * pFatStruct->BytesPerSector) + ((FileEntries[i] - 2) * pFatStruct->BytesPerSector *pFatStruct->SecPerCluster));
		FAT_IF_MemoryWrite(pContent + (i * pFatStruct->BytesPerSector * pFatStruct->SecPerCluster), pFatStruct->FileSystemAddress + (pFatStruct->FirstDataSector * pFatStruct->BytesPerSector) + ((FileEntries[i] - 2) * pFatStruct->BytesPerSector *pFatStruct->SecPerCluster), tmp);

		FileSize -= tmp;

		if(i == NbrOfClusters - 1)
		{
			if(tmp <  pFatStruct->BytesPerSector * pFatStruct->SecPerCluster)
			{
				uint8_t buffer[(pFatStruct->BytesPerSector * pFatStruct->SecPerCluster) - tmp];
				memset(buffer, 0x00, sizeof(buffer));
				FAT_IF_MemoryWrite(buffer, pFatStruct->FileSystemAddress + tmp + (pFatStruct->FirstDataSector * pFatStruct->BytesPerSector) + ((FileEntries[i] - 2) * pFatStruct->BytesPerSector *pFatStruct->SecPerCluster), sizeof(buffer));
			}
		}

	}

	return FAT_OK;
}
