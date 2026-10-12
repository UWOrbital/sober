#pragma once

#include "IThermal.hpp"
#include <cstdint>
#include "thermal_datatypes.hpp"
#include "stm32h7xx_hal.h"

//#include "FLIR_I2C.h"
#include "LEPTON_SDK.h"
#include "LEPTON_VID.h"
#include "LEPTON_SYS.h"
#include "LEPTON_RAD.h"
#include "LEPTON_AGC.h"
#include "LEPTON_OEM.h"
#include "LEPTON_I2C_Reg.h"



class Lepton : public IThermal {
	public:
		Lepton(I2C_HandleTypeDef *i2cHandle,
           SPI_HandleTypeDef *spiHandle);
	
		// Initialization
		LEP_RESULT init() override;
	
		// Data reading, first read returns all 0s, subsequent reads return latest data
        //ThermalImageFrame_t streamFrames() override;
        
        Spotmeter_Values_t get_scene_stats(uint16_t *min, uint16_t *max, uint16_t *avg) override;
	
        uint32_t get_last_frame_timestamp() override;

        Camera_Mode_t get_camera_mode() override;

        Calibration_t get_calibration() override;

		~Lepton() override;

        bool Reset();

        int configureSettings();

        void getSerialNumber();

        void set_calibration(const Calibration_t &cal) override;
	
        
	private:
		HAL_StatusTypeDef readRegister(uint16_t registerAddress, uint16_t *value);
		HAL_StatusTypeDef writeRegister(uint16_t registerAddress, uint16_t value);
		bool isCCIReady();

		SPI_HandleTypeDef *spiHandle;
        I2C_HandleTypeDef *i2cHandle;
};