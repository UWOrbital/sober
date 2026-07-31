// use dma to stream frames into RAM buffer
// setup vospi

#include "lepton.hpp"
#include "lepton_vospi.hpp"

#include <string.h>

SPI_HandleTypeDef hspi1; 
LeptonVoSpi* LeptonVoSpi::activeInstance_ = nullptr;


LeptonVoSpi::LeptonVoSpi(SPI_HandleTypeDef *hspi, GPIO_TypeDef *csPort, uint16_t csPin)
    : spiHandle_(hspi1), csPort_(csPort), csPin_(csPin), 
    frameReady_(false) {
    memset(&frameBuffer_, 0, sizeof(frameBuffer_));
    lastFrameTimestamp_ = 0;
}



void LeptonVoSpi::init() {
    activeInstance_ = this;
     if (HAL_SPI_RegisterCallback(spiHandle_, HAL_SPI_RX_HALF_COMPLETE_CB_ID,
            &LeptonVoSpi::onRxHalfComplete) != HAL_OK) {
        return -1;
    }
    if (HAL_SPI_RegisterCallback(spiHandle_, HAL_SPI_RX_COMPLETE_CB_ID,
            &LeptonVoSpi::onRxComplete) != HAL_OK) {
        return -1;
    }
    if (HAL_SPI_RegisterCallback(spiHandle_, HAL_SPI_ERROR_CB_ID,
            &LeptonVoSpi::onError) != HAL_OK) {
        return -1;
    }


    // CS low = VoSPI select; check part-specific whether this is held low continuously
    HAL_GPIO_WritePin(csPort_, csPin_, GPIO_PIN_RESET);

    HAL_StatusTypeDef status = HAL_SPI_Receive_DMA(spiHandle_, vospiDmaBuf, 2 * VOSPI_PACKET_SIZE);
    if (status != HAL_OK) {
        // Handle error
    }
}

void LeptonVoSpi::onRxHalfComplete(SPI_HandleTypeDef *hspi) {
    if (activeInstance_ && hspi == activeInstance_->spiHandle_) {
        activeInstance_->handlePacket(&activeInstance_->dmaBuf_[0]);
    }
}

void LeptonVoSpi::onRxComplete(SPI_HandleTypeDef *hspi) {
    if (activeInstance_ && hspi == activeInstance_->spiHandle_) {
        activeInstance_->handlePacket(&activeInstance_->dmaBuf_[VOSPI_PACKET_SIZE]);
    }
}

void LeptonVoSpi::onError(SPI_HandleTypeDef *hspi) {
    if (activeInstance_ && hspi == activeInstance_->spiHandle_) {
        activeInstance_->resync();
    }
}

