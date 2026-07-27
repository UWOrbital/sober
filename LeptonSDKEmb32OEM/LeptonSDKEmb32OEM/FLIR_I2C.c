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
#include "LEPTON_MACROS.h"
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

LEP_UINT8 tx[I2C_BUFFER_SIZE];
LEP_UINT8 rx[I2C_BUFFER_SIZE];

/******************************************************************************/
/** LOCAL TYPE DEFINITIONS                                                   **/
/******************************************************************************/

/******************************************************************************/
/** PRIVATE DATA DECLARATIONS                                                **/
/******************************************************************************/

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
	
    if (HAL_I2C_IsDeviceReady(&hi2c1, I2C_DEVICE_ADDRESS, 3, 1000) != HAL_OK)
    {
        // log hi2c1.State;
        // log i2c device not ready
        return LEP_ERROR;
    }

    result =  DEV_I2C_MasterStatus();

    // can have something configure baud if we want, but for now just return the baud rate we were given
    *BaudRate = hi2c1.Init.ClockSpeed / 1000; // return the actual baud rate in kHz

   return(result);
}

/**
 * Closes the I2C driver connection.
 * 
 * @return LEP_RESULT  0 if all goes well, errno otherwise.
 */
LEP_RESULT DEV_I2C_MasterClose()
{   
    LEP_RESULT result = LEP_OK;
   // close socket
    if (HAL_I2C_DeInit(&hi2c1) != HAL_OK)
    {
        // log error
        result = LEP_ERROR;
    }

   return result;
}

/**
 * Resets the I2C driver back to the READY state.
 * 
 * @return LEP_RESULT  0 if all goes well, errno otherwise.
 */
LEP_RESULT DEV_I2C_MasterReset(void )
{
    LEP_RESULT result = LEP_OK;

    if (HAL_I2C_DeInit(&hi2c1) != HAL_OK)
    {
        // log error
        result = LEP_ERROR;
    }

    HAL_Delay(500); // wait for 500 ms before re-initializing

    if (HAL_I2C_Init(&hi2c1) != HAL_OK)
    {
        // log error
        result = LEP_ERROR;
    }

    // clear any I2C error flags (__HAL_I2C_CLEAR_FLAG)

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
   LEP_RESULT result = LEP_OK;
   LEP_UINT16 bytesToRead = (wordsToRead << 1);

    // HAL_I2C_Mem_Read(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t MemAddress,
    //                               uint16_t MemAddSize, uint8_t *pData, uint16_t Size, uint32_t Timeout);
    // uses i2c handle, cci device address, register address provided, rxdata payload, x bytes read, timeout of 1000 ms
    if (HAL_I2C_Mem_Read(&hi2c1, I2C_DEVICE_ADDRESS, regAddress, I2C_MEMADD_SIZE_16BIT, (uint8_t *)rx, bytesToRead, 1000) != HAL_OK)
    {
        *numWordsRead = 0;
        return LEP_ERROR_I2C_FAIL;
        // read from status register whats wrong in hal. are there any error flags? if its not hal ok?
    }

    LEP_UINT16 *src = (LEP_UINT16*)rx;
        
       for (LEP_UINT16 i = 0; i < wordsToRead; i++) {
        readDataPtr[i] = REVERSE_ENDIENESS_UINT16(src[i]);
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
   
   LEP_RESULT result = LEP_OK;
   LEP_UINT16 bytesToWrite = (wordsToWrite << 1);
    //HAL_StatusTypeDef HAL_I2C_Mem_Write(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t MemAddress,
                                        //uint16_t MemAddSize, uint8_t *pData, uint16_t Size, uint32_t Timeout);
    for (LEP_UINT16 i = 0; i < wordsToWrite; i++) {
        txWordBuf[i] = REVERSE_ENDIENESS_UINT16(writeDataPtr[i]);
    }

    if (HAL_I2C_Mem_Write(&hi2c1, deviceAddress, regAddress,
                                    I2C_MEMADD_SIZE_16BIT, (uint8_t *)txWordBuf, bytesToWrite, 1000) != HAL_OK)
    {
        *numWordsWritten = 0;
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
                                        LEP_UINT16 regValue,     // Number of 16-bit words actually written
                                        LEP_UINT16 *status
                                      )
{
   LEP_RESULT result = LEP_OK;
   LEP_UINT16 wordsActuallyWritten;
    /* Place Device-Specific Interface here
    */ 
   result = DEV_I2C_MasterWriteData(portID, deviceAddress, regAddress, &regValue, 1, &wordsActuallyWritten, status);

   return(result);
}

LEP_RESULT DEV_I2C_MasterStatus(void )
{
    LEP_RESULT result = LEP_OK;

    int8_t errorCode;
    LEP_UINT16 statusReg = 0;
    LEP_UINT16 returnStatus;
    if (DEV_I2C_MasterReadRegister(0x00,
                                    I2C_DEVICE_ADDRESS, 
                                    0x0002,
                                    &statusReg,    
                                    &returnStatus
                                     ) != LEP_OK) { // error, could not read status register
        (LEP_UINT8*) returnedData = (LEP_UINT16*) &statusReg; // cast to 2 byte array
        errorCode = returnedData[0]; // log this later
        return LEP_ERROR_I2C_FAIL;
    }
    // else if (!(status[1] && 0x02 >> 1)){ // if bit 1 is 0, ROM is cooked could not boot
    //     return false;
    // }
    
    LEP_UINT32 startTick = HAL_GetTick();

    while(status[1] & 0x06 != 0x06){ // must be 0000 0110 when ready . BIT MASK FOR IT
        errorCode = status[0]; // log this later
        HAL_Delay(100);
        DEV_I2C_MasterReadRegister(0x00, I2C_DEVICE_ADDRESS, 0x0002, &statusReg, &returnStatus);
        if ((HAL_GetTick() - startTick) >= 2000) // timeout at 2 seconds
            return LEP_ERROR_I2C_FAIL;
    }

    return(result);
}


/******************************************************************************/
/** PRIVATE MODULE FUNCTIONS                                                 **/
/******************************************************************************/


