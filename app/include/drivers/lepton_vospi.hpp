#pragma once
#include <cstdint>
#include "stm32h7xx_hal.h"
#include "thermal_datatypes.hpp"

class LeptonVoSpi {
public:
    LeptonVoSpi(SPI_HandleTypeDef *spiHandle, GPIO_TypeDef *csPort, uint16_t csPin);

    int init();                              // configure SPI/DMA, start continuous capture
    bool isFrameReady() const;                // frame assembled and ready to hand off
    ThermalImageFrame_t getLatestFrame();     // pull the completed frame buffer
    uint32_t getLastFrameTimestamp() const;

private:
    void handlePacket(const uint8_t *packet); // called from DMA callback, parses/assembles
    bool isDiscardPacket(const uint8_t *packet) const;

    SPI_HandleTypeDef *spiHandle_;
    GPIO_TypeDef *csPort_;
    uint16_t csPin_;
    ThermalImageFrame_t frameBuffer_;
    uint32_t lastFrameTimestamp_;
    bool frameReady_;
};