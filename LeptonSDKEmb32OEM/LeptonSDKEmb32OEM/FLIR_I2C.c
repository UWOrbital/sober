/*******************************************************************************
**
**    File NAME: FLIR_I2C.c
**
**      AUTHOR:  Hart Thomson
**
**      CREATED: 7/10/2015
**  
**      DESCRIPTION: Lepton Device-Specific Driver for various Master I2C Devices
**
**      HISTORY:  7/19/2015 HT - Initial Draft 
**
** Copyright 2010, 2011, 2012, 2013, 2014, 2015 FLIR Systems - Commercial Vision Systems
**
**  All rights reserved.
**
**  Redistribution and use in source and binary forms, with or without
**  modification, are permitted provided that the following conditions are met:
**
**  Redistributions of source code must retain the above copyright notice, this
**  list of conditions and the following disclaimer.
**
**  Redistributions in binary form must reproduce the above copyright notice,
**  this list of conditions and the following disclaimer in the documentation
**  and/or other materials provided with the distribution.
**
**  Neither the name of the Indigo Systems Corporation nor the names of its
**  contributors may be used to endorse or promote products derived from this
**  software without specific prior written permission.
**
**  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
**  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
**  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
**  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
**  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
**  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
**  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
**  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
**  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
**  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
**  THE POSSIBILITY OF SUCH DAMAGE.
**
*******************************************************************************/
/******************************************************************************/
/** INCLUDE FILES                                                            **/
/******************************************************************************/


#include "LEPTON_Types.h"
#include "LEPTON_ErrorCodes.h"
#include "LEPTON_Macros.h"
#include "FLIR_I2C.h"
#include "LEPTON_I2C_Reg.h"
#include <stdio.h>
#include <stdlib.h>


/******************************************************************************/
/** LOCAL DEFINES                                                            **/
/******************************************************************************/

#define ADDRESS_SIZE_BYTES  2
#define VALUE_SIZE_BYTES    2
#define I2C_BUFFER_SIZE (ADDRESS_SIZE_BYTES + LEP_I2C_DATA_BUFFER_0_LENGTH)
#define I2C_DEVICE_ADDRESS 0x2A
#define PORT_ID 0x00
#define STATUS_REGISTER_ADDRESS 0x0002
#define STATUS_READY_MASK 0x05 
#define STATUS_READY 0x04 


static LEP_UINT16 txWordBuf[I2C_BUFFER_SIZE / 2];
static LEP_UINT16 rxWordBuf[I2C_BUFFER_SIZE / 2];


/******************************************************************************/
/** LOCAL TYPE DEFINITIONS                                                   **/
/******************************************************************************/

/******************************************************************************/
/** PRIVATE DATA DECLARATIONS                                                **/
/******************************************************************************/
//TEMPORARY? 
extern I2C_HandleTypeDef hi2c1; // Define the static member variable

/******************************************************************************/
/** PRIVATE FUNCTION DECLARATIONS                                            **/
/******************************************************************************/

/******************************************************************************/
/** EXPORTED PUBLIC DATA                                                     **/
/******************************************************************************/

/******************************************************************************/
/** EXPORTED PUBLIC FUNCTIONS                                                **/
/******************************************************************************/


LEP_RESULT DEV_I2C_MasterSelectDevice(LEP_PROTOCOL_DEVICE_E device)
{
    LEP_RESULT result = LEP_OK;

    return(result);
}


/******************************************************************************/
/**
 *  main.c calls mx_i2c1_init().
 *  mx_i2c1_init() defines i2c1 handle in i2c.c and calls HAL_I2C_Init(&hi2c1) to initialize the i2c1 handle.
 *  HAL_I2C_Init calls HAL_I2C_MspInit(&hi2c1) to initialize the i2c1 peripheral and gpio pins for scl and sda.
 *  so we can just use the hi2c1 handle that is already initialized in main
 * Performs I2C Master Initialization
 * 
 * @param portID     LEP_UINT16  User specified port ID tag.  Can be used to
 *                   select between multiple cameras
 * 
 * @param BaudRate   Clock speed in kHz. Typically this is 400.
 *                   The Device Specific Driver will try to match the desired
 *                   speed.  This parameter is updated to the actual speed the
 *                   driver can use.
 * 
 * @return LEP_RESULT  0 if all goes well, errno otherwise
 */
LEP_RESULT DEV_I2C_MasterInit(LEP_UINT16 portID, 
                              LEP_UINT16 *BaudRate)
{
    
    LEP_RESULT result = LEP_OK;	
	uint32_t start = HAL_GetTick();
    
    //ASSUME DRIVE RESET HIGH BY DEFUALT, IE POWER ON.
    // if otherwise, start needs to be collected from the time reset is high from a GPIO
    while ( HAL_GetTick() - start < 950){ // wait for boot
    }
    
    result =  DEV_I2C_MasterStatus();

    // already configured through stm32 hal, return what was set.
    *BaudRate = 100; // Timing 0x10707DBC at the 64 MHz APB1 clock (HSI is your SYSCLK) works out to about 100 kHz. 
                     // Set it to 100. If you later switch SYSCLK to the PLL, regenerate the I2C timing.

   return(result);
}

/**
 * Closes the I2C driver connection.
 * 
 * @return LEP_RESULT  0 if all goes well, errno otherwise.
 */
LEP_RESULT DEV_I2C_MasterClose()
{   
   return LEP_OK; // shouldn' de-init through LEP, this is owned by STM32 HAL.
}

/**
 * Resets the I2C driver back to the READY state.
 * 
 * @return LEP_RESULT  0 if all goes well, errno otherwise.
 */
LEP_RESULT DEV_I2C_MasterReset(void )
{
    LEP_RESULT result = LEP_OK;

    HAL_I2C_DeInit(&hi2c1); // deinit the i2c handle
    MX_I2C1_Init();

    result =  DEV_I2C_MasterStatus();

   return(result);
}

LEP_RESULT DEV_I2C_MasterReadData(LEP_UINT16  portID,               // User-defined port ID
                                  LEP_UINT8   deviceAddress,        // Lepton Camera I2C Device Address
                                  LEP_UINT16  regAddress,           // Lepton Register Address
                                  LEP_UINT16 *readDataPtr,          // Read DATA buffer pointer
                                  LEP_UINT16  wordsToRead,          // Number of 16-bit words to Read
                                  LEP_UINT16 *numWordsRead,         // Number of 16-bit words actually Read
                                  LEP_UINT16 *status                // Transaction Status
                                 )
{ 
   if (wordsToRead > (I2C_BUFFER_SIZE / 2)) {
        return LEP_DATA_SIZE_ERROR; // Return an error if the requested number of words exceeds the buffer size
    }
    LEP_RESULT result = LEP_OK;
   LEP_UINT16 bytesToRead = (wordsToRead << 1);
   int timeout = 1000; // 1 second timeout for I2C read operation

    // FUNCTION SIG: HAL_I2C_Mem_Read(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t MemAddress,
    //                               uint16_t MemAddSize, uint8_t *pData, uint16_t Size, uint32_t Timeout);
    // uses i2c handle, cci device address, register address provided, rxdata payload, x bytes read, timeout of 1000 ms
    if (HAL_I2C_Mem_Read(&hi2c1, (uint16_t)(deviceAddress << 1), regAddress, I2C_MEMADD_SIZE_16BIT, (uint8_t *)rxWordBuf, bytesToRead, timeout) != HAL_OK)
    {
        *numWordsRead = 0;
        *status = HAL_I2C_GetError(&hi2c1); // get the error code from the HAL I2C handle
        return LEP_ERROR_I2C_FAIL;
    }
        
       for (LEP_UINT16 i = 0; i < wordsToRead; i++) {
        readDataPtr[i] = REVERSE_ENDIENESS_UINT16(rxWordBuf[i]);
    }
   
    *numWordsRead = wordsToRead;

   return(result);
}

LEP_RESULT DEV_I2C_MasterWriteData(LEP_UINT16  portID,              // User-defined port ID
                                   LEP_UINT8   deviceAddress,       // Lepton Camera I2C Device Address
                                   LEP_UINT16  regAddress,          // Lepton Register Address
                                   LEP_UINT16 *writeDataPtr,        // Write DATA buffer pointer
                                   LEP_UINT16  wordsToWrite,        // Number of 16-bit words to Write
                                   LEP_UINT16 *numWordsWritten,     // Number of 16-bit words actually written
                                   LEP_UINT16 *status)              // Transaction Status
{
   if (wordsToWrite > (I2C_BUFFER_SIZE / 2)) {
        return LEP_DATA_SIZE_ERROR; // Return an error if the requested number of words exceeds the buffer size
    }

   LEP_RESULT result = LEP_OK;
   LEP_UINT16 bytesToWrite = (wordsToWrite << 1);
    
    for (LEP_UINT16 i = 0; i < wordsToWrite; i++) {
        txWordBuf[i] = REVERSE_ENDIENESS_UINT16(writeDataPtr[i]);
    }

    //HAL_StatusTypeDef HAL_I2C_Mem_Write(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t MemAddress,
                                        //uint16_t MemAddSize, uint8_t *pData, uint16_t Size, uint32_t Timeout);
    if (HAL_I2C_Mem_Write(&hi2c1, (uint16_t)(deviceAddress << 1), regAddress,
                                    I2C_MEMADD_SIZE_16BIT, (uint8_t *)txWordBuf, bytesToWrite, 1000) != HAL_OK)
    {
        *numWordsWritten = 0;
        *status = HAL_I2C_GetError(&hi2c1); // get the error code from the HAL I2C handle
        return LEP_ERROR_I2C_FAIL;
        // read from status register whats wrong in hal. are there any error flags? if its not hal ok?
    }

    *numWordsWritten = wordsToWrite;

   return(result);  
}

LEP_RESULT DEV_I2C_MasterReadRegister( LEP_UINT16 portID,
                                       LEP_UINT8  deviceAddress, 
                                       LEP_UINT16 regAddress,
                                       LEP_UINT16 *regValue,    //buffer to hold the value read from the register
                                       LEP_UINT16 *status
                                     )
{
    LEP_RESULT result = LEP_OK;

   LEP_UINT16 wordsActuallyRead;
    /* Place Device-Specific Interface here
    */ 
    result = DEV_I2C_MasterReadData(portID, deviceAddress, regAddress, regValue, 1 /*1 word*/, &wordsActuallyRead, status);

   return(result);
}

LEP_RESULT DEV_I2C_MasterWriteRegister( LEP_UINT16 portID,
                                        LEP_UINT8  deviceAddress, 
                                        LEP_UINT16 regAddress,
                                        LEP_UINT16 regValue,     // data to write to register
                                        LEP_UINT16 *status      // transaction status
                                      )
{
   LEP_RESULT result = LEP_OK; 
   LEP_UINT16 wordsActuallyWritten; 
    /* Place Device-Specific Interface here
    */ 
   result = DEV_I2C_MasterWriteData(portID, deviceAddress, regAddress, &regValue, 1, &wordsActuallyWritten, status);

   return(result);
}


// Reads the status register of the Lepton camera to determine if the camera is booted and ready for communication. 
// This function is called by the DEV_I2C_MasterInit() function as part of start-up sequence.
// wait for bit 2 to become 1
// poll for bit 0 to become 0
// bit 1 should be 1 to indicate successful rom boot. if 0, return error. 
LEP_RESULT DEV_I2C_MasterStatus(void )
{
    LEP_UINT8 lep_status_errorCode; // error code from lepton status register
    LEP_UINT16 returnedStatus[1]; // buffer to hold the status register value (2 bytes, 16 bits total)
    LEP_UINT16 HAL_errorStatus[2]; // buffer to hold the error status from the I2C read operation (is it 2 bytes or 1 byte? check the HAL function)
    LEP_RESULT result;

    // need port ID here. inject it as a parameter or use a global variable. for now, just use PORT_ID as a global variable.


    uint32_t start = HAL_GetTick();

   do {
       LEP_UINT16 st = 0, herr = 0;
       if (DEV_I2C_MasterReadRegister(PORT_ID, I2C_DEVICE_ADDRESS,
               STATUS_REGISTER_ADDRESS, &st, &herr) == LEP_OK && (st & STATUS_READY_MASK) == STATUS_READY)
           return LEP_OK;
       HAL_Delay(10);
   } while ((HAL_GetTick() - start) < 2000);
   return LEP_TIMEOUT_ERROR;

   uint32_t start = HAL_GetTick();
   do {
       int32_t raw = -1;   // 32-bit read, safe regardless of enum size
       if (LEP_GetSysFFCStatus(portDescPtr, (LEP_SYS_STATUS_E_PTR)&raw) == LEP_OK
           && raw == LEP_SYS_STATUS_READY) break;
       HAL_Delay(10);
   } while (HAL_GetTick() - start < 3000);
}

//if (DEV_I2C_MasterReadRegister(...) == LEP_OK && ((st & 0x07) == 0x06)) return LEP_OK;


/******************************************************************************/
/** PRIVATE MODULE FUNCTIONS                                                 **/
/******************************************************************************/


