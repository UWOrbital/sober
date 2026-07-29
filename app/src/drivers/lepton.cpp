// Lepton.cpp
#pragma once

#include <string.h>
#include "lepton.hpp"

I2C_HandleTypeDef hi2c1; // Define the static member variable

uint16_t portID = 0;
LEP_CAMERA_PORT_E portType = LEP_CCI_TWI;
uint16_t portBaudRate = 100; // Adjust as needed
LEP_CAMERA_PORT_DESC_T_PTR portDescPtr;

Lepton::Lepton(I2C_HandleTypeDef *i2cHandle,
                SPI_HandleTypeDef *spiHandle, GPIO_TypeDef *csPort, uint16_t csPin)
    : i2cHandle(i2cHandle), spiHandle(spiHandle), csPort(csPort), csPin(csPin) {
}

int Lepton::init() {

    uint16_t baudRate;
    // initialize the i2c handle
    // initialize portDescPtr and allocate memory for it
    if (LEP_OpenPort(portID,
                    portType,
                    portBaudRate,
                    portDescPtr) != LEP_OK) {
        return -1; // Return -1 on failure
    }
    
    return 0; // Return 0 on success
}

bool Lepton::Reset(){
    LEP_RESULT status = LEP_ResetPort(portDescPtr);
    if (status != LEP_OK) {
        // Log error
        return false; // Return false on failure
    }
    return true; // Return true on success
}

bool Lepton::isCCIReady() {
    if (DEV_I2C_MasterStatus() != LEP_OK) {
        return false; // CCI is not ready
    }

    // checking register 0x2 as well, but just the 2nd bit. not the other one. so not sure what to do with this.
    // LEP_SDK_BOOT_STATUS_E bootStatus;
    // if (LEP_GetCameraBootStatus(portDescPtr, &bootStatus) != LEP_OK) {
    //     // log error, boot status
    //     return false; // Failed to get boot status
    // }
    // else if (!bootStatus) { // bootStatus must be enum 1
    //     return false; // Camera is not booted
    // }

    return true;
}
    // poll sys ffc command status (4.5.18 see this part) (system ready return)

void Lepton::configureSettings() {
    // disable/enable agc
    LEP_SetAgcEnableState( portDescPtr, 0); // disable for now, enable later if necessary
    LEP_SetAgcPolicy( portDescPtr, LEP_AGC_LINEAR); // set to linear for now, can change to HEQ later if necessary
    LEP_SetRadEnableState( portDescPtr, LEP_RAD_DISABLE); // enable radiometry for now, can change to disable later if necessary);
    LEP_SetOemVideoOutputFormat( portDescPtr, LEP_VID_VIDEO_OUTPUT_FORMAT_RAW14); // set to raw14 for now, can change to other formats later if necessary
}

void Lepton::getSerialNumber() {
    
    LEP_SYS_FLIR_SERIAL_NUMBER_T_PTR sysSerialNumberBufPtr = new LEP_SYS_FLIR_SERIAL_NUMBER_T; // Assuming you have a valid buffer for the serial number
    
    LEP_RESULT status = LEP_GetSysFlirSerialNumber(portDescPtr, sysSerialNumberBufPtr);
    if (status != LEP_OK) {
        //log status
    } else {
        //log serial number from sysSerialNumberBufPtr
    }
    delete sysSerialNumberBufPtr; // Clean up the allocated memory
}

ThermalImageFrame_t Lepton::streamFrames() {
    ThermalImageFrame_t frame;
    // Read a frame from the Lepton camera and fill the frame structure
    // This may involve SPI communication and data processing
    return vospi_.getLatestFrame();
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
result = LEP_ClosePort(portDescPtr);
    if (result != LEP_OK) {
     // Return false on failure
    }
    // This may involve closing the SPI interface and cleaning up any allocated memory
    }

