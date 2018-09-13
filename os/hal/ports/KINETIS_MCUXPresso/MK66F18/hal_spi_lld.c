/*
    ChibiOS - Copyright (C) 2006..2016 Giovanni Di Sirio

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

/**
 * @file    hal_spi_lld.c
 * @brief   PLATFORM SPI subsystem low level driver source.
 *
 * @addtogroup SPI
 * @{
 */
#include "osal.h"
#include "hal.h"

#if (HAL_USE_SPI == TRUE) || defined(__DOXYGEN__)

/*===========================================================================*/
/* Driver local definitions.                                                 */
/*===========================================================================*/

/*===========================================================================*/
/* Driver exported variables.                                                */
/*===========================================================================*/

/**
 * @brief   SPI0 driver identifier.
 */
#if (KINETIS_SPI_USE_SPI0 == TRUE) || defined(__DOXYGEN__)
SPIDriver SPID0;
#endif

/**
 * @brief   SPI1 driver identifier.
 */
#if (KINETIS_SPI_USE_SPI1 == TRUE) || defined(__DOXYGEN__)
SPIDriver SPID1;
#endif


/**
 * @brief   SPI2 driver identifier.
 */
#if (KINETIS_SPI_USE_SPI2 == TRUE) || defined(__DOXYGEN__)
SPIDriver SPID2;
#endif

/*===========================================================================*/
/* Driver local variables and types.                                         */
/*===========================================================================*/
static SPIConfig default_spi_config;
/*===========================================================================*/
/* Driver local functions.                                                   */
/*===========================================================================*/

/*===========================================================================*/
/* Driver interrupt handlers.                                                */
/*===========================================================================*/
void spi_lld_transfer_complete_cb(SPI_Type *base, dspi_master_edma_handle_t *handle, status_t status, void *userData)
{
  (void)base;
  (void)handle;
  SPIDriver* spip = (SPIDriver*)userData;
  if (status == kStatus_Success) {
    _spi_isr_code(spip);
  }
}
/*===========================================================================*/
/* Driver exported functions.                                                */
/*===========================================================================*/

/**
 * @brief   Low level SPI driver initialization.
 *
 * @notapi
 */
void spi_lld_init(void) {

#if KINETIS_SPI_USE_SPI0 == TRUE
  /* Driver initialization.*/
  spiObjectInit(&SPID0);
  SPID0.thread = NULL;
  SPID0.spi = SPI0;
  SPID0.irq_prio = KINETIS_SPI_SPI0_IRQ_PRIORITY;
  SPID0.vector = SPI0_IRQn;
#endif
#if KINETIS_SPI_USE_SPI1 == TRUE
  /* Driver initialization.*/
  spiObjectInit(&SPID1);
  SPID1.thread = NULL;
  SPID1.spi = SPI1;
  SPID1.irq_prio = KINETIS_SPI_SPI1_IRQ_PRIORITY;
  SPID1.vector = SPI1_IRQn;
#endif
#if KINETIS_SPI_USE_SPI2 == TRUE
  /* Driver initialization.*/
  spiObjectInit(&SPID2);
  SPID2.thread = NULL;
  SPID2.spi = SPI2;
  SPID2.irq_prio = KINETIS_SPI_SPI2_IRQ_PRIORITY;
  SPID2.vector = SPI2_IRQn;
#endif
}

/**
 * @brief   Configures and activates the SPI peripheral.
 *
 * @param[in] spip      pointer to the @p SPIDriver object
 *
 * @notapi
 */
void spi_lld_start(SPIDriver *spip) {
  if (spip->config == NULL) {
    DSPI_MasterGetDefaultConfig(&default_spi_config.master);
    spip->config = &default_spi_config;
  }
  if (spip->state == SPI_STOP) {
    /* Enables the peripheral.*/
#if KINETIS_SPI_USE_SPI0 == TRUE
    if (&SPID0 == spip) {
      DSPI_MasterInit(spip->spi, &spip->config->master, CLOCK_GetFreq(DSPI0_CLK_SRC));
      memset(&spip->edmaRxRegToRxDataHandle, 0, sizeof(spip->edmaRxRegToRxDataHandle));
      memset(&spip->edmaIntermediaryToTxRegHandle, 0, sizeof(spip->edmaIntermediaryToTxRegHandle));
      memset(&spip->edmaTxDataToIntermediaryHandle, 0, sizeof(spip->edmaTxDataToIntermediaryHandle));
      dmaChannelAllocate(&spip->edmaRxRegToRxDataHandle, kDmaRequestMux0SPI0Rx, KINETIS_SPI_SPI0_IRQ_PRIORITY, NULL, (void *)spip);
      dmaChannelAllocate(&spip->edmaIntermediaryToTxRegHandle, kDmaRequestMux0SPI0Tx, KINETIS_SPI_SPI0_IRQ_PRIORITY, NULL, (void *)spip);
      dmaChannelAllocate(&spip->edmaTxDataToIntermediaryHandle, kDmaRequestMux0Disable, KINETIS_SPI_SPI0_IRQ_PRIORITY, NULL, (void *)spip);
      DSPI_MasterTransferCreateHandleEDMA(spip->spi, &spip->handle, spi_lld_transfer_complete_cb, spip, &spip->edmaRxRegToRxDataHandle,  &spip->edmaTxDataToIntermediaryHandle, &spip->edmaIntermediaryToTxRegHandle);
    }
#endif
#if KINETIS_SPI_USE_SPI1 == TRUE
    if (&SPID1 == spip) {
      DSPI_MasterInit(spip->spi, &spip->config->master, CLOCK_GetFreq(DSPI1_CLK_SRC));
      memset(&spip->edmaRxRegToRxDataHandle, 0, sizeof(spip->edmaRxRegToRxDataHandle));
      memset(&spip->edmaIntermediaryToTxRegHandle, 0, sizeof(spip->edmaIntermediaryToTxRegHandle));
      memset(&spip->edmaTxDataToIntermediaryHandle, 0, sizeof(spip->edmaTxDataToIntermediaryHandle));
      dmaChannelAllocate(&spip->edmaRxRegToRxDataHandle, kDmaRequestMux0SPI1Rx, KINETIS_SPI_SPI1_IRQ_PRIORITY, NULL, (void *)spip);
      dmaChannelAllocate(&spip->edmaIntermediaryToTxRegHandle, kDmaRequestMux0SPI1Tx, KINETIS_SPI_SPI1_IRQ_PRIORITY, NULL, (void *)spip);
      dmaChannelAllocate(&spip->edmaTxDataToIntermediaryHandle, kDmaRequestMux0Disable, KINETIS_SPI_SPI1_IRQ_PRIORITY, NULL, (void *)spip);
      DSPI_MasterTransferCreateHandleEDMA(spip->spi, &spip->handle, spi_lld_transfer_complete_cb, spip, &spip->edmaRxRegToRxDataHandle, &spip->edmaTxDataToIntermediaryHandle, &spip->edmaIntermediaryToTxRegHandle);
    }
#endif
#if KINETIS_SPI_USE_SPI2 == TRUE
    if (&SPID2 == spip) {
      DSPI_MasterInit(spip->spi, &spip->config->master, CLOCK_GetFreq(DSPI2_CLK_SRC));
      memset(&spip->edmaRxRegToRxDataHandle, 0, sizeof(spip->edmaRxRegToRxDataHandle));
      memset(&spip->edmaIntermediaryToTxRegHandle, 0, sizeof(spip->edmaIntermediaryToTxRegHandle));
      memset(&spip->edmaTxDataToIntermediaryHandle, 0, sizeof(spip->edmaTxDataToIntermediaryHandle));
      dmaChannelAllocate(&spip->edmaRxRegToRxDataHandle, kDmaRequestMux0SPI2Rx, KINETIS_SPI_SPI2_IRQ_PRIORITY, NULL, (void *)spip);
      dmaChannelAllocate(&spip->edmaIntermediaryToTxRegHandle, kDmaRequestMux0SPI2Tx, KINETIS_SPI_SPI2_IRQ_PRIORITY, NULL, (void *)spip);
      dmaChannelAllocate(&spip->edmaTxDataToIntermediaryHandle, kDmaRequestMux0Disable, KINETIS_SPI_SPI2_IRQ_PRIORITY, NULL, (void *)spip);
      DSPI_MasterTransferCreateHandleEDMA(spip->spi, &spip->handle, spi_lld_transfer_complete_cb, spip, &spip->edmaRxRegToRxDataHandle, &spip->edmaTxDataToIntermediaryHandle, &spip->edmaIntermediaryToTxRegHandle);
    }
#endif
  }
  /* Configures the peripheral.*/

}

/**
 * @brief   Deactivates the SPI peripheral.
 *
 * @param[in] spip      pointer to the @p SPIDriver object
 *
 * @notapi
 */
void spi_lld_stop(SPIDriver *spip) {

  if (spip->state == SPI_READY) {
    /* Disables the peripheral.*/
#if KINETIS_SPI_USE_SPI0 == TRUE
    if (&SPID0 == spip) {
      DSPI_MasterTransferAbortEDMA(spip->spi, &spip->handle);
      dmaChannelRelease(&spip->edmaRxRegToRxDataHandle);
      dmaChannelRelease(&spip->edmaIntermediaryToTxRegHandle);
      dmaChannelRelease(&spip->edmaTxDataToIntermediaryHandle);
    }
#endif
#if KINETIS_SPI_USE_SPI1 == TRUE
    if (&SPID1 == spip) {
      DSPI_MasterTransferAbortEDMA(spip->spi, &spip->handle);
      dmaChannelRelease(&spip->edmaRxRegToRxDataHandle);
      dmaChannelRelease(&spip->edmaIntermediaryToTxRegHandle);
      dmaChannelRelease(&spip->edmaTxDataToIntermediaryHandle);
    }
#endif
#if KINETIS_SPI_USE_SPI2 == TRUE
    if (&SPID2 == spip) {
      DSPI_MasterTransferAbortEDMA(spip->spi, &spip->handle);
      dmaChannelRelease(&spip->edmaRxRegToRxDataHandle);
      dmaChannelRelease(&spip->edmaIntermediaryToTxRegHandle);
      dmaChannelRelease(&spip->edmaTxDataToIntermediaryHandle);
    }
#endif
  }
}

/**
 * @brief   Asserts the slave select signal and prepares for transfers.
 *
 * @param[in] spip      pointer to the @p SPIDriver object
 *
 * @notapi
 */
void spi_lld_select(SPIDriver *spip) {
  palClearPad(spip->config->ssport, spip->config->sspad);
}

/**
 * @brief   Deasserts the slave select signal.
 * @details The previously selected peripheral is unselected.
 *
 * @param[in] spip      pointer to the @p SPIDriver object
 *
 * @notapi
 */
void spi_lld_unselect(SPIDriver *spip) {
  palSetPad(spip->config->ssport, spip->config->sspad);
}

/**
 * @brief   Ignores data on the SPI bus.
 * @details This asynchronous function starts the transmission of a series of
 *          idle words on the SPI bus and ignores the received data.
 * @post    At the end of the operation the configured callback is invoked.
 *
 * @param[in] spip      pointer to the @p SPIDriver object
 * @param[in] n         number of words to be ignored
 *
 * @notapi
 */
void spi_lld_ignore(SPIDriver *spip, size_t n) {
  (void)spip;
  (void)n;
}

/**
 * @brief   Exchanges data on the SPI bus.
 * @details This asynchronous function starts a simultaneous transmit/receive
 *          operation.
 * @post    At the end of the operation the configured callback is invoked.
 * @note    The buffers are organized as uint8_t arrays for data sizes below or
 *          equal to 8 bits else it is organized as uint16_t arrays.
 *
 * @param[in] spip      pointer to the @p SPIDriver object
 * @param[in] n         number of words to be exchanged
 * @param[in] txbuf     the pointer to the transmit buffer
 * @param[out] rxbuf    the pointer to the receive buffer
 *
 * @notapi
 */
void spi_lld_exchange(SPIDriver *spip, size_t n,
                      const void *txbuf, void *rxbuf) {

  dspi_transfer_t masterXfer;
  masterXfer.txData = (uint8_t*)txbuf;
  masterXfer.rxData = (uint8_t*)rxbuf;
  masterXfer.dataSize = n;
  masterXfer.configFlags = kDSPI_MasterCtar0 | kDSPI_MasterPcsContinuous;
  DSPI_MasterTransferEDMA(spip->spi, &spip->handle, &masterXfer);
}

/**
 * @brief   Sends data over the SPI bus.
 * @details This asynchronous function starts a transmit operation.
 * @post    At the end of the operation the configured callback is invoked.
 * @note    The buffers are organized as uint8_t arrays for data sizes below or
 *          equal to 8 bits else it is organized as uint16_t arrays.
 *
 * @param[in] spip      pointer to the @p SPIDriver object
 * @param[in] n         number of words to send
 * @param[in] txbuf     the pointer to the transmit buffer
 *
 * @notapi
 */
void spi_lld_send(SPIDriver *spip, size_t n, const void *txbuf) {
  dspi_transfer_t masterXfer;
  masterXfer.txData = (uint8_t*)txbuf;
  masterXfer.rxData = NULL;
  masterXfer.dataSize = n;
  masterXfer.configFlags = kDSPI_MasterCtar0 | kDSPI_MasterPcsContinuous;
  DSPI_MasterTransferEDMA(spip->spi, &spip->handle, &masterXfer);
}

/**
 * @brief   Receives data from the SPI bus.
 * @details This asynchronous function starts a receive operation.
 * @post    At the end of the operation the configured callback is invoked.
 * @note    The buffers are organized as uint8_t arrays for data sizes below or
 *          equal to 8 bits else it is organized as uint16_t arrays.
 *
 * @param[in] spip      pointer to the @p SPIDriver object
 * @param[in] n         number of words to receive
 * @param[out] rxbuf    the pointer to the receive buffer
 *
 * @notapi
 */
void spi_lld_receive(SPIDriver *spip, size_t n, void *rxbuf) {
  dspi_transfer_t masterXfer;
  masterXfer.txData = NULL;
  masterXfer.rxData = rxbuf;
  masterXfer.dataSize = n;
  masterXfer.configFlags = kDSPI_MasterCtar0 | kDSPI_MasterPcsContinuous;
  DSPI_MasterTransferEDMA(spip->spi, &spip->handle, &masterXfer);
}

/**
 * @brief   Exchanges one frame using a polled wait.
 * @details This synchronous function exchanges one frame using a polled
 *          synchronization method. This function is useful when exchanging
 *          small amount of data on high speed channels, usually in this
 *          situation is much more efficient just wait for completion using
 *          polling than suspending the thread waiting for an interrupt.
 *
 * @param[in] spip      pointer to the @p SPIDriver object
 * @param[in] frame     the data frame to send over the SPI bus
 * @return              The received data frame from the SPI bus.
 */
uint16_t spi_lld_polled_exchange(SPIDriver *spip, uint16_t frame) {
  dspi_transfer_t masterXfer;
  uint16_t data;
  masterXfer.txData = (uint8_t*)&frame;
  masterXfer.rxData = (uint8_t*)&data;
  if (spip->config->master.ctarConfig.bitsPerFrame <= 8) {
    masterXfer.dataSize = 1;
  } else {
    masterXfer.dataSize = 2;
  }
  masterXfer.configFlags = kDSPI_MasterCtar0 | kDSPI_MasterPcsContinuous;
  DSPI_MasterTransferBlocking(spip->spi, &masterXfer);
  return data;
}

#endif /* HAL_USE_SPI == TRUE */

/** @} */
