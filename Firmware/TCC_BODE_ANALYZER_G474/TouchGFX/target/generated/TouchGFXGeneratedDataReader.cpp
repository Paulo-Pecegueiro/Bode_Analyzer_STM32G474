/**
  ******************************************************************************
  * File Name          : TouchGFXGeneratedDataReader.cpp
  ******************************************************************************
  * This file is generated by TouchGFX Generator 4.22.1. Please, do not edit!
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
#include <TouchGFXGeneratedDataReader.hpp>
#include <stm32g4xx.h>

#define MEM_BASE_ADDRESS              0x00000000
#define MEM_FLASH_SIZE                0x01000000

/*
 * External Interface description for starting transmissions
 */
extern "C" void DataReader_WaitForReceiveDone();
extern "C" void DataReader_ReadData(uint32_t address24, uint8_t* buffer, uint32_t length);
extern "C" void DataReader_StartDMAReadData(uint32_t address24, uint8_t* buffer, uint32_t length);

bool TouchGFXGeneratedDataReader::addressIsAddressable(const void* address)
{
    if (address >= (void*)MEM_BASE_ADDRESS && address <(void*)(MEM_BASE_ADDRESS + MEM_FLASH_SIZE))
    {
        return false;
    }

    return true;
}

void TouchGFXGeneratedDataReader::copyData(const void* src, void* dst, uint32_t bytes)
{
    /* If requested number of bytes are less than 2048
     * CPU and polling will be used to read from flash.
     * If number of bytes requested is larger than 2048
     * a DMA transfer will be started.
     */
    if (bytes < 2048)
    {
        /* Start transfer using the CPU */
        DataReader_ReadData((uint32_t)src, (uint8_t*)dst, bytes);
    }
    else
    {
        /* Start transfer using DMA */
        DataReader_StartDMAReadData((uint32_t)src, (uint8_t*)dst, bytes);

        /* Wait on data transmision complete */
        DataReader_WaitForReceiveDone();
    }
}

void TouchGFXGeneratedDataReader::startFlashLineRead(const void* src, uint32_t bytes)
{
    /* Start transfer using DMA */
    DataReader_StartDMAReadData((uint32_t)src, (readToBuffer1 ? buffer1 : buffer2), bytes);
}

const uint8_t* TouchGFXGeneratedDataReader::waitFlashReadComplete()
{
  /*
   * Switch line buffer and then wait for transmission to complete before returning new buffer.
   */
    uint8_t* readBuffer = readToBuffer1 ? buffer1 : buffer2;
    DataReader_WaitForReceiveDone();
    readToBuffer1 = !readToBuffer1;
    return readBuffer;
}
