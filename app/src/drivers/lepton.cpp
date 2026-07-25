// Lepton.cpp
#pragma once

#include <string.h>
#include "lepton.hpp"

I2C_HandleTypeDef hi2c1; // Define the static member variable

Lepton::Lepton(SPI_HandleTypeDef *spiHandle, GPIO_TypeDef *csPort, uint16_t csPin)
    : spiHandle(spiHandle), csPort(csPort), csPin(csPin) {
}


int Lepton::init() {

    if (DEV_I2C_MasterInit(LEP_UINT16 portID, 
                              LEP_UINT16 *BaudRate) != LEP_OK) {
        return -1; // Return -1 on failure
    }
    
    return 0; // Return 0 on success
}

bool Lepton::isCCIReady() {
    if (DEV_I2C_MasterStatus() != LEP_OK) {
        return false; // CCI is not ready
    }

    return true;
}
    // poll sys ffc command status (4.5.18 see this part) (system ready return)

HAL_StatusTypeDef Lepton::readRegister(uint16_t* register_address, uint16_t *rxData) {}
    

bool Lepton::writeRegister(uint16_t register_address, uint16_t value) {}

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

