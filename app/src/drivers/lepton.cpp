// Lepton.cpp
#pragma once

#include <string.h>
#include "lepton.hpp"

#define I2C_DEVICE_ADDRESS 0x2A

Lepton::Lepton(SPI_HandleTypeDef *spiHandle, GPIO_TypeDef *csPort, uint16_t csPin)
    : spiHandle(spiHandle), csPort(csPort), csPin(csPin) {}


int Lepton::init() {
    // Initialize the Lepton camera here
    // This may involve setting up SPI, GPIO, and sending initialization commands to the camera
    // init i2c (cci)

    // configure i2c handle
    I2C_HandleTypeDef hi2c;
    hi2c.Instance = I2C1; //check wiring
    hi2c.Init.Timing = 0x00C0EAFF; // Timing value for 400kHz, check reference manual
    hi2c.Init.OwnAddress1 = 0;
    hi2c.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT; // check
    hi2c.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    hi2c.Init.OwnAddress2 = 0;
    hi2c.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    hi2c.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;

    hi2c.Devaddress = I2C_DEVICE_ADDRESS; // Lepton camera I2C address

    // call init
    if (HAL_I2C_Init(&hi2c) != HAL_OK) {
        Error_Handler(); // Handle error
    }

    // wait 950 ms (see datasheet 2)
    HAL_Delay(950);

    // configure hal i2c msp init
    if (hi2c->Instance == I2C1)
    {
        // clocks applied
    __HAL_RCC_I2C1_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();

    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = GPIO_PIN_8 | GPIO_PIN_9; // CONFIRM WITH data sheet
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF4_I2C1;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    HAL_NVIC_SetPriority(I2C1_EV_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(I2C1_EV_IRQn);
    HAL_NVIC_SetPriority(I2C1_ER_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(I2C1_ER_IRQn);
    }
    
    // need to do interrupt mode, so enable i2c event/error irqs and implement:
    // hal_i2c_ev_IRQHandler
    // hal_i2c_er_IRQHandler

    //then for read write, use HAL_I2C_Mem_read_IT
    // HAL_I2C_Mem_Write_IT
    // or work this one out a bit more
    // we need callback for interrupt

        
        // isCCIReady() check
        // status register (0x0002) bit 2 read, should be 1. if 0, wait more
        // status register (0x0002) bit 0 read, should be 0. if 1, poll until 0
        // poll sys ffc command status (4.5.18 see this part) (system ready return)

        //

    return 0; // Return 0 on success
}

bool Lepton::isCCIReady() {
        // command STATUS read in 0x0002
        readRegister(0x0002, &status);
        // status register (0x0002) bit 2 read, should be 1. if 0, wait more
        // status register (0x0002) bit 0 read, should be 0. if 1, poll until 0
        // poll sys ffc command status (4.5.18 see this part) (system ready return)

    return true; // Placeholder, implement actual check
}

uint16_t Lepton::readRegister(uint16_t* register_address, uint16_t *value) {
    
    //need i2c to basically read the device address 0x2A
    // then we need i2c to consult the specific register address
    // then we need to read information from there in a buffer of specified size
    // we may get an error code, which we should return as a data type of that sort?
    return status;
}

bool Lepton::writeRegister(uint16_t register_address, uint16_t value) {
    bool status;
    // Write to the specified register of the Lepton camera via SPI
    // This may involve sending a command and data
    return status;
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

