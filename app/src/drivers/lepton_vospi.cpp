// use dma to stream frames into RAM buffer
// setup vospi

#include "lepton.hpp"
#include "lepton_vospi.hpp"

#include <string.h>

LeptonVoSpi::LeptonVoSpi(SPI_HandleTypeDef *spiHandle, GPIO_TypeDef *csPort, uint16_t csPin)
    : spiHandle_(spiHandle), csPort_(csPort), csPin_(csPin), 
    frameReady_(false) {
    memset(&frameBuffer_, 0, sizeof(frameBuffer_));
    lastFrameTimestamp_ = 0;
}



