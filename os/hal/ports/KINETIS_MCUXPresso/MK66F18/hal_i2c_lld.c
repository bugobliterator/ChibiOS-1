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
 * @file    hal_i2c_lld.c
 * @brief   PLATFORM I2C subsystem low level driver source.
 *
 * @addtogroup I2C
 * @{
 */

#include "hal.h"

#if (HAL_USE_I2C == TRUE) || defined(__DOXYGEN__)

/*===========================================================================*/
/* Driver local definitions.                                                 */
/*===========================================================================*/

/*===========================================================================*/
/* Driver exported variables.                                                */
/*===========================================================================*/

/**
 * @brief   I2C1 driver identifier.
 */
#if (KINETIS_I2C_USE_I2C0 == TRUE) || defined(__DOXYGEN__)
I2CDriver I2CD0;
#endif

I2CConfig default_i2c_config;
/*===========================================================================*/
/* Driver local variables and types.                                         */
/*===========================================================================*/

/*===========================================================================*/
/* Driver local functions.                                                   */
/*===========================================================================*/

/*===========================================================================*/
/* Driver interrupt handlers.                                                */
/*===========================================================================*/

/*===========================================================================*/
/* Driver exported functions.                                                */
/*===========================================================================*/

/**
 * @brief   Low level I2C driver initialization.
 *
 * @notapi
 */
void i2c_lld_init(void) {
#if KINETIS_I2C_USE_I2C0 == TRUE
  i2cObjectInit(&I2CD0);
  I2CD0.thread = NULL;
  I2CD0.i2c = I2C0;
  I2CD0.edma = KINETIS_DMA_CHANNEL(KINETIS_I2C_I2C0_DMA_CHAN);
  memset(&I2CD0.transfer, 0, sizeof(I2CD0.transfer));
#endif
}

/**
 * @brief   Configures and activates the I2C peripheral.
 *
 * @param[in] i2cp      pointer to the @p I2CDriver object
 *
 * @notapi
 */

void i2c_lld_start(I2CDriver *i2cp) {
  if (i2cp->config == NULL) {
    I2C_MasterGetDefaultConfig(&default_i2c_config);
    i2cp->config = &default_i2c_config;
  }
  if (i2cp->state == I2C_STOP) {
    /* Enables the peripheral.*/
#if KINETIS_I2C_USE_I2C0 == TRUE
    if (&I2CD0 == i2cp) {
      I2C_MasterInit(i2cp->i2c, i2cp->config, CLOCK_GetFreq(BUS_CLK));
      DMAMUX_SetSource(DMAMUX0, KINETIS_I2C_I2C0_DMA_CHAN, kDmaRequestMux0I2C0);
      EDMA_CreateHandle(&i2cp->edma_handle, DMA0, KINETIS_I2C_I2C0_DMA_CHAN);
      dmaChannelAllocate(i2cp->edma, KINETIS_I2C_I2C0_DMA_PRIORITY, NULL, (void *)i2cp);
      DMAMUX_EnableChannel(DMAMUX0, KINETIS_I2C_I2C0_DMA_CHAN);
    }
#endif
  }

}

/**
 * @brief   Deactivates the I2C peripheral.
 *
 * @param[in] i2cp      pointer to the @p I2CDriver object
 *
 * @notapi
 */
void i2c_lld_stop(I2CDriver *i2cp) {

  if (i2cp->state != I2C_STOP) {

    /* Disables the peripheral.*/
#if KINETIS_I2C_USE_I2C0 == TRUE
    if (&I2CD1 == i2cp) {

    }
#endif
  }
}

/**
 * @brief   Receives data via the I2C bus as master.
 *
 * @param[in] i2cp      pointer to the @p I2CDriver object
 * @param[in] addr      slave device address
 * @param[out] rxbuf    pointer to the receive buffer
 * @param[in] rxbytes   number of bytes to be received
 * @param[in] timeout   the number of ticks before the operation timeouts,
 *                      the following special values are allowed:
 *                      - @a TIME_INFINITE no timeout.
 *                      .
 * @return              The operation status.
 * @retval MSG_OK       if the function succeeded.
 * @retval MSG_RESET    if one or more I2C errors occurred, the errors can
 *                      be retrieved using @p i2cGetErrors().
 * @retval MSG_TIMEOUT  if a timeout occurred before operation end. <b>After a
 *                      timeout the driver must be stopped and restarted
 *                      because the bus is in an uncertain state</b>.
 *
 * @notapi
 */
msg_t i2c_lld_master_receive_timeout(I2CDriver *i2cp, i2caddr_t addr,
                                     uint8_t *rxbuf, size_t rxbytes,
                                     systime_t timeout) {

  (void)i2cp;
  (void)addr;
  (void)rxbuf;
  (void)rxbytes;
  (void)timeout;

  return MSG_OK;
}

/**
 * @brief   Transmits data via the I2C bus as master.
 *
 * @param[in] i2cp      pointer to the @p I2CDriver object
 * @param[in] addr      slave device address
 * @param[in] txbuf     pointer to the transmit buffer
 * @param[in] txbytes   number of bytes to be transmitted
 * @param[out] rxbuf    pointer to the receive buffer
 * @param[in] rxbytes   number of bytes to be received
 * @param[in] timeout   the number of ticks before the operation timeouts,
 *                      the following special values are allowed:
 *                      - @a TIME_INFINITE no timeout.
 *                      .
 * @return              The operation status.
 * @retval MSG_OK       if the function succeeded.
 * @retval MSG_RESET    if one or more I2C errors occurred, the errors can
 *                      be retrieved using @p i2cGetErrors().
 * @retval MSG_TIMEOUT  if a timeout occurred before operation end. <b>After a
 *                      timeout the driver must be stopped and restarted
 *                      because the bus is in an uncertain state</b>.
 *
 * @notapi
 */
msg_t i2c_lld_master_transmit_timeout(I2CDriver *i2cp, i2caddr_t addr,
                                      const uint8_t *txbuf, size_t txbytes,
                                      uint8_t *rxbuf, size_t rxbytes,
                                      systime_t timeout) {

  (void)i2cp;
  (void)addr;
  (void)txbuf;
  (void)txbytes;
  (void)rxbuf;
  (void)rxbytes;
  (void)timeout;

  return MSG_OK;
}

#endif /* HAL_USE_I2C == TRUE */

/** @} */
