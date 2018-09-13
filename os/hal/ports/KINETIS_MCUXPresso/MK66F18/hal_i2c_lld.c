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

#include "osal.h"
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
#if (KINETIS_I2C_USE_I2C1 == TRUE) || defined(__DOXYGEN__)
I2CDriver I2CD1;
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
#if (KINETIS_I2C_USE_I2C0 == TRUE) || defined(__DOXYGEN__)
OSAL_IRQ_HANDLER(KINETIS_I2C0_IRQ_VECTOR) {
  OSAL_IRQ_PROLOGUE();
  I2C_MasterTransferHandleIRQ(I2C0, I2CD0.i2c_handle);
  OSAL_IRQ_EPILOGUE();
}
#endif

#if (KINETIS_I2C_USE_I2C1 == TRUE) || defined(__DOXYGEN__)
OSAL_IRQ_HANDLER(KINETIS_I2C1_IRQ_VECTOR) {
  OSAL_IRQ_PROLOGUE();
  I2C_MasterTransferHandleIRQ(I2C1, I2CD1.i2c_handle);
  OSAL_IRQ_EPILOGUE();
}
#endif

static void i2c_master_edma_callback(I2C_Type *base, i2c_master_edma_handle_t *handle, status_t status, void *userData)
{
  (void)base;
  (void)handle;
  /* Signal transfer success when received success status. */
  I2CDriver* i2cp = (I2CDriver*)userData;
  if (status == kStatus_Success)
  {
    _i2c_wakeup_isr(i2cp);
  } else {
    _i2c_wakeup_error_isr(i2cp);
  }
}

static void i2c_master_irq_callback(I2C_Type *base, i2c_master_handle_t *handle, status_t status, void *userData)
{
  (void)base;
  (void)handle;
  /* Signal transfer success when received success status. */
  I2CDriver* i2cp = (I2CDriver*)userData;
  if (status == kStatus_Success)
  {
    _i2c_wakeup_isr(i2cp);
  } else {
    _i2c_wakeup_error_isr(i2cp);
  }
}

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
  I2CD0.irq_prio = KINETIS_I2C_I2C0_IRQ_PRIORITY;
  I2CD0.vector = I2C0_IRQn;
  memset(&I2CD0.transfer, 0, sizeof(I2CD0.transfer));
#endif
#if KINETIS_I2C_USE_I2C1 == TRUE
  i2cObjectInit(&I2CD1);
  I2CD1.thread = NULL;
  I2CD1.i2c = I2C1;
  I2CD1.irq_prio = KINETIS_I2C_I2C1_IRQ_PRIORITY;
  I2CD1.vector = I2C1_IRQn;
  memset(&I2CD1.transfer, 0, sizeof(I2CD1.transfer));
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
      I2C_MasterInit(i2cp->i2c, i2cp->config, CLOCK_GetFreq(I2C0_CLK_SRC));
      dmaChannelAllocate(&i2cp->edma_handle, kDmaRequestMux0I2C0, KINETIS_I2C_I2C0_IRQ_PRIORITY, NULL, (void *)i2cp);
    }
#endif
#if KINETIS_I2C_USE_I2C1 == TRUE
    if (&I2CD1 == i2cp) {
      I2C_MasterInit(i2cp->i2c, i2cp->config, CLOCK_GetFreq(I2C1_CLK_SRC));
      dmaChannelAllocate(&i2cp->edma_handle, kDmaRequestMux0I2C1, KINETIS_I2C_I2C1_IRQ_PRIORITY, NULL, (void *)i2cp);
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
    if (&I2CD0 == i2cp) {
      I2C_MasterStop(i2cp->i2c);
      dmaChannelRelease(&i2cp->edma_handle);
    }
#endif
#if KINETIS_I2C_USE_I2C1 == TRUE
    if (&I2CD1 == i2cp) {
      I2C_MasterStop(i2cp->i2c);
      dmaChannelRelease(&i2cp->edma_handle);
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
  i2c_master_transfer_t masterXfer;
  i2c_master_edma_handle_t i2c_master_edma_handle;
  i2c_master_handle_t i2c_master_handle;
  memset(&masterXfer, 0, sizeof(masterXfer));
  masterXfer.slaveAddress = addr;
  masterXfer.direction = kI2C_Read;
  masterXfer.data = rxbuf;
  masterXfer.dataSize = rxbytes;
  masterXfer.flags = kI2C_TransferDefaultFlag;

  if (masterXfer.dataSize > 1) {
    I2C_MasterCreateEDMAHandle(i2cp->i2c, &i2c_master_edma_handle, i2c_master_edma_callback, i2cp, &i2cp->edma_handle);
    I2C_MasterTransferEDMA(i2cp->i2c, &i2c_master_edma_handle, &masterXfer);
  } else {
    I2C_MasterTransferCreateHandle(i2cp->i2c, &i2c_master_handle, i2c_master_irq_callback, i2cp);
    i2cp->i2c_handle = &i2c_master_handle;
    nvicEnableVector(i2cp->vector, i2cp->irq_prio);
    I2C_MasterTransferNonBlocking(i2cp->i2c, &i2c_master_handle, &masterXfer);
    /* Waits for the operation completion or a timeout.*/
    msg_t msg = osalThreadSuspendTimeoutS(&i2cp->thread, timeout);
    nvicDisableVector(i2cp->vector);
    return msg;
  }
  /* Waits for the operation completion or a timeout.*/
  return osalThreadSuspendTimeoutS(&i2cp->thread, timeout);
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
  i2c_master_transfer_t masterXfer;
  i2c_master_edma_handle_t i2c_master_edma_handle;
  i2c_master_handle_t i2c_master_handle;
  if (txbytes > 4 || rxbytes == 0) {
    //do pure write
    if (rxbytes == 0) {
      memset(&masterXfer, 0, sizeof(masterXfer));
      masterXfer.slaveAddress = addr;
      masterXfer.direction = kI2C_Write;
      masterXfer.data = (uint8_t*)txbuf;
      masterXfer.dataSize = txbytes;
      masterXfer.flags = kI2C_TransferDefaultFlag;

      if (masterXfer.dataSize > 1) {
        I2C_MasterCreateEDMAHandle(i2cp->i2c, &i2c_master_edma_handle, i2c_master_edma_callback, i2cp, &i2cp->edma_handle);
        I2C_MasterTransferEDMA(i2cp->i2c, &i2c_master_edma_handle, &masterXfer);
        /* Waits for the operation completion or a timeout.*/
        return osalThreadSuspendTimeoutS(&i2cp->thread, timeout);
      } else {
        I2C_MasterTransferCreateHandle(i2cp->i2c, &i2c_master_handle, i2c_master_irq_callback, i2cp);
        i2cp->i2c_handle = &i2c_master_handle;
        nvicEnableVector(i2cp->vector, i2cp->irq_prio);
        I2C_MasterTransferNonBlocking(i2cp->i2c, &i2c_master_handle, &masterXfer);
        /* Waits for the operation completion or a timeout.*/
        msg_t msg = osalThreadSuspendTimeoutS(&i2cp->thread, timeout);
        nvicDisableVector(i2cp->vector);
        return msg;
      }
    }
    //MCUXpresso API doesn't supportt asynch transactions beyond 4bytes
    return MSG_RESET;
  }
  uint32_t reg_addr = 0;
  memcpy(&reg_addr, txbuf, txbytes);
  memset(&masterXfer, 0, sizeof(masterXfer));
  masterXfer.slaveAddress = addr;
  masterXfer.direction = kI2C_Read;
  masterXfer.subaddress = reg_addr;
  masterXfer.subaddressSize = txbytes;
  masterXfer.data = rxbuf;
  masterXfer.dataSize = rxbytes;
  masterXfer.flags = kI2C_TransferDefaultFlag;

  if (masterXfer.dataSize > 1) {
    I2C_MasterCreateEDMAHandle(i2cp->i2c, &i2c_master_edma_handle, i2c_master_edma_callback, i2cp, &i2cp->edma_handle);
    I2C_MasterTransferEDMA(i2cp->i2c, &i2c_master_edma_handle, &masterXfer);
  } else {
    I2C_MasterTransferCreateHandle(i2cp->i2c, &i2c_master_handle, i2c_master_irq_callback, i2cp);
    i2cp->i2c_handle = &i2c_master_handle;
    nvicEnableVector(i2cp->vector, i2cp->irq_prio);
    I2C_MasterTransferNonBlocking(i2cp->i2c, &i2c_master_handle, &masterXfer);
    /* Waits for the operation completion or a timeout.*/
    msg_t msg = osalThreadSuspendTimeoutS(&i2cp->thread, timeout);
    nvicDisableVector(i2cp->vector);
    return msg;
  }
  /* Waits for the operation completion or a timeout.*/
  return osalThreadSuspendTimeoutS(&i2cp->thread, timeout);
}

#endif /* HAL_USE_I2C == TRUE */

/** @} */
