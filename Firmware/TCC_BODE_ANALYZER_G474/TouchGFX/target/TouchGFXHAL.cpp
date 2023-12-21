/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * File Name          : TouchGFXHAL.cpp
 ******************************************************************************
 * This file was created by TouchGFX Generator 4.22.1. This file is only
 * generated once! Delete this file from your project and re-generate code
 * using STM32CubeMX or change this file manually to update it.
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
/* USER CODE END Header */

#include <TouchGFXHAL.hpp>

/* USER CODE BEGIN TouchGFXHAL.cpp */
extern "C"
{
/* Private Includes */
#include "tim.h"
#include "usb_device.h"

#include "NT35510.h"
#include "W25Qxx.h"

#include "app_filesystem.h"
#include "cmsis_os2.h"

/* Private Function Prototypes */
int touchgfxDisplayDriverTransmitActive();
int touchgfxDisplayDriverShouldTransferBlock(uint16_t bottom);
void touchgfxDisplayDriverTransmitBlock(const uint8_t* pixels, uint16_t x, uint16_t y, uint16_t w, uint16_t h);
}

using namespace touchgfx;

void TouchGFXHAL::initialize()
{
	// Calling parent implementation of initialize().
	//
	// To overwrite the generated implementation, omit call to parent function
	// and implemented needed functionality here.
	// Please note, HAL::initialize() must be called to initialize the framework.

	TouchGFXGeneratedHAL::initialize();
	/* Fill the Flash SPI Handler Parameters */
	hw25q1.spi 				= &hspi2;
	hw25q1.GPIO_CS_Pin 		= FLASH_CS_Pin;
	hw25q1.GPIO_CS_Port 	= FLASH_CS_GPIO_Port;

	/* Initialize the External Memory */
	if(W25qxx_Init(&hw25q1) != W25QXX_OK)
		Error_Handler();

	/* Initialize the display */
	NT35510_Init();

	/* Enable Update Interrupt for the V-Sync timer */
	LL_TIM_EnableIT_UPDATE(TIM7);

	/* Enable the V-Sync timer */
	LL_TIM_EnableCounter(TIM7);
}

/**
 * Gets the frame buffer address used by the TFT controller.
 *
 * @return The address of the frame buffer currently being displayed on the TFT.
 */
uint16_t* TouchGFXHAL::getTFTFrameBuffer() const
{
	// Calling parent implementation of getTFTFrameBuffer().
	//
	// To overwrite the generated implementation, omit call to parent function
	// and implemented needed functionality here.

	return TouchGFXGeneratedHAL::getTFTFrameBuffer();
}

/**
 * Sets the frame buffer address used by the TFT controller.
 *
 * @param [in] address New frame buffer address.
 */
void TouchGFXHAL::setTFTFrameBuffer(uint16_t* address)
{
	// Calling parent implementation of setTFTFrameBuffer(uint16_t* address).
	//
	// To overwrite the generated implementation, omit call to parent function
	// and implemented needed functionality here.

	TouchGFXGeneratedHAL::setTFTFrameBuffer(address);
}

/**
 * This function is called whenever the framework has performed a partial draw.
 *
 * @param rect The area of the screen that has been drawn, expressed in absolute coordinates.
 *
 * @see flushFrameBuffer().
 */
void TouchGFXHAL::flushFrameBuffer(const touchgfx::Rect& rect)
{
	// Calling parent implementation of flushFrameBuffer(const touchgfx::Rect& rect).
	//
	// To overwrite the generated implementation, omit call to parent function
	// and implemented needed functionality here.
	// Please note, HAL::flushFrameBuffer(const touchgfx::Rect& rect) must
	// be called to notify the touchgfx framework that flush has been performed.
	// To calculate he start adress of rect,
	// use advanceFrameBufferToRect(uint8_t* fbPtr, const touchgfx::Rect& rect)
	// defined in TouchGFXGeneratedHAL.cpp

	TouchGFXGeneratedHAL::flushFrameBuffer(rect);
}

bool TouchGFXHAL::blockCopy(void* RESTRICT dest, const void* RESTRICT src, uint32_t numBytes)
{
	return TouchGFXGeneratedHAL::blockCopy(dest, src, numBytes);
}

/**
 * Configures the interrupts relevant for TouchGFX. This primarily entails setting
 * the interrupt priorities for the DMA and LCD interrupts.
 */
void TouchGFXHAL::configureInterrupts()
{
	// Calling parent implementation of configureInterrupts().
	//
	// To overwrite the generated implementation, omit call to parent function
	// and implemented needed functionality here.

	TouchGFXGeneratedHAL::configureInterrupts();
}

/**
 * Used for enabling interrupts set in configureInterrupts()
 */
void TouchGFXHAL::enableInterrupts()
{
	// Calling parent implementation of enableInterrupts().
	//
	// To overwrite the generated implementation, omit call to parent function
	// and implemented needed functionality here.

	TouchGFXGeneratedHAL::enableInterrupts();
}

/**
 * Used for disabling interrupts set in configureInterrupts()
 */
void TouchGFXHAL::disableInterrupts()
{
	// Calling parent implementation of disableInterrupts().
	//
	// To overwrite the generated implementation, omit call to parent function
	// and implemented needed functionality here.

	TouchGFXGeneratedHAL::disableInterrupts();
}

/**
 * Configure the LCD controller to fire interrupts at VSYNC. Called automatically
 * once TouchGFX initialization has completed.
 */
void TouchGFXHAL::enableLCDControllerInterrupt()
{
	// Calling parent implementation of enableLCDControllerInterrupt().
	//
	// To overwrite the generated implementation, omit call to parent function
	// and implemented needed functionality here.

	TouchGFXGeneratedHAL::enableLCDControllerInterrupt();
}

bool TouchGFXHAL::beginFrame()
{
	return TouchGFXGeneratedHAL::beginFrame();
}

void TouchGFXHAL::endFrame()
{
	TouchGFXGeneratedHAL::endFrame();
}

int touchgfxDisplayDriverTransmitActive()
{
	/* Return the status variable */
	return __isTranmittingBlock;
}

void touchgfxDisplayDriverTransmitBlock(const uint8_t* pixels, uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{
	/* Set the bitmap window */
	NT35510_SetWindow(x, y, w, h);

	/* Transmit data in DMA  mode */
	NT35510_DMAWrite((uint16_t *)pixels, x, y, w, h);
}
/* USER CODE END TouchGFXHAL.cpp */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
