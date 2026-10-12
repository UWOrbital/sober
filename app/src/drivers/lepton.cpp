// Lepton.cpp
#include <string.h>
#include "lepton.hpp"

constexpr uint16_t PORT_ID = 0x00; 

static constexpr LEP_CAMERA_PORT_E portType = LEP_CCI_TWI; //assign port to TWI for now
static LEP_CAMERA_PORT_DESC_T portDesc; 
static LEP_CAMERA_PORT_DESC_T_PTR portDescPtr = &portDesc;

// many error codes exist in the lepton sdk, but we will just return -1 for now. can expand later if necessary.

// the handles for the i2c and spi interfaces are actually defined in spi.c and i2c.c
// so we pass in a pointer to those handles. in the main.cpp code that calls this class
// to be expanded upon in lepton.hpp (add more members to constructor); may need other pins? or do we need pins at all?
Lepton::Lepton(I2C_HandleTypeDef *i2cHandle,
                SPI_HandleTypeDef *spiHandle)
    : i2cHandle(i2cHandle), spiHandle(spiHandle) {
}

LEP_RESULT Lepton::init() {
     /* FROM SDK: Using the Lepton SDK to communicate with the Lepton camera requires opening a supported
communication port before issuing any other calls. The port open operation specifies the desired baud
rate for the port and returns a port descriptor for use with all other SDK APIs. */
    uint16_t baudRate = 100; 
    // initialize the i2c handle
    // initialize portDescPtr and allocate memory for it
  
    // in main: apply power, clock, and reset_L to high. (IDD 2.1.1)
    // This is typically done in the main application code before initializing the Lepton

  LEP_RESULT result = LEP_OpenPort(PORT_ID,
                    portType,
                    baudRate,
                    portDescPtr)
    
    return result; // Return the result of the operation
}

bool Lepton::Reset(){
    LEP_RESULT status = LEP_ResetPort(portDescPtr);
    if (status != LEP_OK) {
        // Log error
        return false; // Return false on failure
    }
    return true; // Return true on success
}


// call this over and over until it returns true for every command to send
bool Lepton::isCCIReady() {
    

    LEP_UINT16 status = 0;
    if (LEP_DirectReadRegister(portDescPtr, LEP_I2C_STATUS_REG, &status) != LEP_OK) {
        return false;
    }
    return (status & 0x05) == 0x04;   // booted and not busy
    }
    // poll sys ffc command status (4.5.18 see this part) (system ready return)

int Lepton::configureSettings() {
    // disable/enable agc
    LEP_RESULT r;
    if ((r = LEP_SetAgcEnableState(portDescPtr, LEP_AGC_DISABLE)) != LEP_OK) return r;
    if ((r = LEP_SetAgcPolicy(portDescPtr, LEP_AGC_LINEAR)) != LEP_OK) return r;
    if ((r = LEP_SetRadEnableState(portDescPtr, LEP_RAD_DISABLE)) != LEP_OK) return r;
    // skip to keep default for now
    //if ((r = LEP_SetOemVideoOutputFormat( portDescPtr, LEP_VIDEO_OUTPUT_FORMAT_RAW14)) != LEP_OK) return r;

    return r;

    //then when init calls this, 
    // int rc = configureSettings(); if (rc != 0) return rc;
}

void Lepton::getSerialNumber() {
    LEP_SYS_FLIR_SERIAL_NUMBER_T sysSerialNumberBuf;
    LEP_SYS_FLIR_SERIAL_NUMBER_T_PTR sysSerialNumberBufPtr = &sysSerialNumberBuf; // Assuming you have a valid buffer for the serial number
    
    LEP_RESULT status = LEP_GetSysFlirSerialNumber(portDescPtr, sysSerialNumberBufPtr);
    if (status != LEP_OK) {
        //log status
    } else {
        //log serial number from sysSerialNumberBufPtr
    }
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
    LEP_RESULT result = LEP_ClosePort(portDescPtr);
    
    // This may involve closing the SPI interface and cleaning up any allocated memory
}