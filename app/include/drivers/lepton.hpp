#pragma once

#include "IThermal.hpp"
#include "stm32h7xx_hal.h"
#include "FLIR_I2C.h"
#include <cstdint>
#include "thermal_datatypes.hpp"

class Lepton : public IThermal {
	public:
		Lepton(SPI_HandleTypeDef *spiHandle, GPIO_TypeDef *csPort, uint16_t csPin);
	
		// Initialization
		int init() override;
	
		// Data reading, first read returns all 0s, subsequent reads return latest data
        ThermalImageFrame_t streamFrames() override;
        
        Spotmeter_Values_t get_scene_stats(uint16_t *min, uint16_t *max, uint16_t *avg) override;
	
        uint32_t get_last_frame_timestamp() override;

        Camera_Mode_t get_camera_mode() override;

        Calibration_t get_calibration() override;

        void set_calibration(const Calibration_t &cal) override;
	
		SPI_HandleTypeDef *getSPI();

	private:
		HAL_StatusTypeDef readRegister(uint16_t registerAddress, uint16_t *value);
		HAL_StatusTypeDef writeRegister(uint16_t registerAddress, uint16_t value);
		bool isCCIReady();

		SPI_HandleTypeDef *spiHandle;
};