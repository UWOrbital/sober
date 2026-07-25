// Lepton.cpp
#pragma once

#include <string.h>
#include "lepton.hpp"

I2C_HandleTypeDef hi2c1; // Define the static member variable

Lepton::Lepton(SPI_HandleTypeDef *spiHandle, GPIO_TypeDef *csPort, uint16_t csPin)
    : spiHandle(spiHandle), csPort(csPort), csPin(csPin) {
}


int Lepton::init() {

    // wait 950 ms (see datasheet 2)
    HAL_Delay(950);

    HAL_I2C_IsDeviceReady(&hi2c1, I2C_DEVICE_ADDRESS, 3, 1000); // Check if device is ready
        
    int status = isCCIReady();

    return status; // Return 0 on success, -1 otherwise
}

bool Lepton::isCCIReady() {
        // command STATUS read in 0x0002
    std::int8_t errorCode;
    uint8_t status[2] = {0};
    if (readRegister(0x0002, &status) != HAL_OK) { // error, could not read status register
        errorCode = status[0]; // log this later
        return false;
    }
    // else if (!(status[1] && 0x02 >> 1)){ // if bit 1 is 0, ROM is cooked could not boot
    //     return false;
    // }
    
    while(status[1] != 0x06){ // must be 0000 0110 when ready 
        errorCode = status[0]; // log this later
        HAL_Delay(100);
        readRegister(0x0002, &status);
        if (HAL_GetTick() >= 2000) // timeout at 2 seconds
            return false;
    }

    return true;
}
        // poll sys ffc command status (4.5.18 see this part) (system ready return)

HAL_StatusTypeDef Lepton::readRegister(uint16_t* register_address, uint16_t *rxData) {
    // HAL_I2C_Mem_Read(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t MemAddress,
    //                               uint16_t MemAddSize, uint8_t *pData, uint16_t Size, uint32_t Timeout);
    // uses i2c handle, cci device address, register address provided, rxdata payload, size of 2 bytes, timeout of 1000 ms
    return HAL_I2C_Mem_Read(&hi2c1, I2C_DEVICE_ADDRESS, register_address, I2C_MEMADD_SIZE_16BIT, (uint8_t *)rxData, 2, 1000);
}

bool Lepton::writeRegister(uint16_t register_address, uint16_t value) {
    bool status;
    // Write to the specified register of the Lepton camera via SPI
    // This may involve sending a command and data
    return status;
}

int8_t Lepton::sendCommand(uint16_t command) {
    // Implementation for sending command to Lepton camera
    return 0; // Placeholder
}

ThermalImageFrame_t Lepton::streamFrames() {
    ThermalImageFrame_t frame;
    // Read a frame from the Lepton camera and fill the frame structure
    // This may involve SPI communication and data processing
    return frame;
}

Spotmeter_Values_t Lepton::get_scene_stats(uint16_t *min, uint16_t *max, uint16_t *avg) {
    Spotmeter_Values_t stats;
    // Calculate the scene statistics (mean, max, min) from the thermal frame
    return stats;
}

uint32_t Lepton::get_last_frame_timestamp() {
    // Return the timestamp of the last frame received from the sensor
    return 0; // Placeholder
}

Camera_Mode_t Lepton::get_camera_mode() {
    Camera_Mode_t mode;
    // Retrieve the current camera mode (radiometry, tlinear, agc, video format)
    return mode;
}

Calibration_t Lepton::get_calibration() {
    Calibration_t cal;
    // Retrieve the current calibration data (emissivity, window transmission, window temp)
    return cal;
}

void Lepton::set_calibration(const Calibration_t &cal) {
    // Set the calibration data (emissivity, window transmission, window temp)
}

SPI_HandleTypeDef *Lepton::getSPI() {
    return spiHandle;
}

Lepton::~Lepton() {
    // Clean up resources if necessary
}

