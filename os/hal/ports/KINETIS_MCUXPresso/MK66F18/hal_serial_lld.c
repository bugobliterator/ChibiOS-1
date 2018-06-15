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
 * @file    hal_serial_lld.c
 * @brief   PLATFORM serial subsystem low level driver source.
 *
 * @addtogroup SERIAL
 * @{
 */

#include "hal.h"

#if (HAL_USE_SERIAL == TRUE) || defined(__DOXYGEN__)

/*===========================================================================*/
/* Driver local definitions.                                                 */
/*===========================================================================*/

/*===========================================================================*/
/* Driver exported variables.                                                */
/*===========================================================================*/

/** @brief USART1 serial driver identifier.*/
#if (KINETIS_SERIAL_USE_LPUART0 == TRUE) || defined(__DOXYGEN__)
SerialDriver LPSD1;
#endif

/*===========================================================================*/
/* Driver local variables and types.                                         */
/*===========================================================================*/


/*===========================================================================*/
/* Driver local functions.                                                   */
/*===========================================================================*/

/**
 * @brief   Error handling routine.
 *
 * @param[in] sdp       pointer to a @p SerialDriver object
 * @param[in] sr        USART SR register value
 */
static void set_error(SerialDriver *sdp, uint16_t flags) {
  eventflags_t sts = 0;
  if (flags & (kLPUART_RxOverrunFlag))
    sts |= SD_OVERRUN_ERROR;
  if (flags & (kLPUART_ParityErrorFlag))
    sts |= SD_PARITY_ERROR;
  if (flags & (kLPUART_FramingErrorFlag))
    sts |= SD_FRAMING_ERROR;
  if (flags & (kLPUART_NoiseErrorFlag))
    sts |= SD_NOISE_ERROR;
  chnAddFlagsI(sdp, sts);
}

/**
 * @brief   Common IRQ handler.
 *
 * @param[in] sdp       communication channel associated to the USART
 */
static void lpuart_serve_interrupt(SerialDriver *sdp) {
  uint32_t flags = 0;
  LPUART_Type* u  = (LPUART_Type*)(sdp->uart);
  flags = LPUART_GetStatusFlags(u);

  /* Special case, LIN break detection.*/
  if ((kLPUART_LinBreakFlag) & flags) {
    osalSysLockFromISR();
    chnAddFlagsI(sdp, SD_BREAK_DETECTED);
    LPUART_ClearStatusFlags(u, kLPUART_LinBreakFlag);
    osalSysUnlockFromISR();
  }

  /* Data available.*/
  osalSysLockFromISR();
  while (flags & ((kLPUART_RxDataRegFullFlag) | (kLPUART_RxOverrunFlag) | (kLPUART_NoiseErrorFlag) | (kLPUART_FramingErrorFlag) |
               (kLPUART_ParityErrorFlag))) {
    uint8_t b;

    /* Error condition detection.*/
    if (flags & ((kLPUART_RxOverrunFlag) | (kLPUART_NoiseErrorFlag) | (kLPUART_FramingErrorFlag) | (kLPUART_ParityErrorFlag)))
      set_error(sdp, flags);
    b = LPUART_ReadByte(u);
    if (flags & (kLPUART_RxDataRegFullFlag))
      sdIncomingDataI(sdp, b);
    flags = LPUART_GetStatusFlags(u);
  }
  osalSysUnlockFromISR();
  uint32_t enabled_interrupts = LPUART_GetEnabledInterrupts(u);
  /* Transmission buffer empty.*/
  if ((flags & (kLPUART_TxDataRegEmptyFlag)) && (enabled_interrupts & (kLPUART_TxDataRegEmptyInterruptEnable))) {
    msg_t b;
    osalSysLockFromISR();
    b = oqGetI(&sdp->oqueue);
    if (b < MSG_OK) {
      chnAddFlagsI(sdp, CHN_OUTPUT_EMPTY);
      LPUART_DisableInterrupts(u, kLPUART_TxDataRegEmptyInterruptEnable);
      LPUART_EnableInterrupts(u, kLPUART_TransmissionCompleteInterruptEnable);
    }
    else
      LPUART_WriteByte(u, b);
    osalSysUnlockFromISR();
  }

  /* Physical transmission end.*/
  if (flags & (kLPUART_TransmissionCompleteFlag)) {
    osalSysLockFromISR();
    if (oqIsEmptyI(&sdp->oqueue))
      chnAddFlagsI(sdp, CHN_TRANSMISSION_END);
    LPUART_DisableInterrupts(u, kLPUART_TransmissionCompleteInterruptEnable);
    osalSysUnlockFromISR();
  }
  _serial_irq_code(sdp);
}

#if KINETIS_SERIAL_USE_LPUART0 || defined(__DOXYGEN__)
static void notify1(io_queue_t *qp) {
  (void)qp;
  /* Enable transmiter interrupt. */
  LPUART_EnableInterrupts(LPUART0, kLPUART_TxDataRegEmptyInterruptEnable);
}
#endif


/*===========================================================================*/
/* Driver interrupt handlers.                                                */
/*===========================================================================*/
#if KINETIS_SERIAL_USE_LPUART0 || defined(__DOXYGEN__)
#if !defined(KINETIS_LPSERIAL0_IRQ_VECTOR)
#error "KINETIS_LPSERIAL0_IRQ_VECTOR not defined"
#endif
/**
 * @brief   LPUART0 interrupt handler.
 *
 * @isr
 */
OSAL_IRQ_HANDLER(KINETIS_LPSERIAL0_IRQ_VECTOR) {

  OSAL_IRQ_PROLOGUE();

  lpuart_serve_interrupt(&LPSD1);

  OSAL_IRQ_EPILOGUE();
}
#endif
/*===========================================================================*/
/* Driver exported functions.                                                */
/*===========================================================================*/

/**
 * @brief   Low level serial driver initialization.
 *
 * @notapi
 */
void sd_lld_init(void) {
#if KINETIS_SERIAL_USE_LPUART0 == TRUE
  sdObjectInit(&LPSD1, NULL, notify1);
  LPSD1.is_lp = true;
  LPSD1.uart = LPUART0;
#endif
}

/**
 * @brief   Low level serial driver configuration and (re)start.
 *
 * @param[in] sdp       pointer to a @p SerialDriver object
 * @param[in] config    the architecture-dependent serial driver configuration.
 *                      If this parameter is set to @p NULL then a default
 *                      configuration is used.
 *
 * @notapi
 */
void sd_lld_start(SerialDriver *sdp, const SerialConfig *config) {
  static SerialConfig default_config;
  if (config == NULL) {
    if (sdp->is_lp) {
      LPUART_GetDefaultConfig(&default_config.lpuart_config);
    } else {
      UART_GetDefaultConfig(&default_config.uart_config);
    }
    config = &default_config;
  }
  sdp->config = config;

  if (sdp->state == SD_STOP) {
#if KINETIS_SERIAL_USE_LPUART0 == TRUE
    if (&LPSD1 == sdp) {
      CLOCK_SetLpuartClock(1);
      LPUART_Init(LPUART0, &config->lpuart_config, CLOCK_GetFreq(kCLOCK_PllFllSelClk));
      nvicEnableVector(LPUART0_IRQn, KINETIS_SERIAL_LPUART0_PRIORITY);
      LPUART_EnableInterrupts(LPUART0, (kLPUART_FramingErrorInterruptEnable) | (kLPUART_ParityErrorInterruptEnable) | 
      (kLPUART_RxOverrunInterruptEnable) | (kLPUART_NoiseErrorInterruptEnable));
    }
#endif
  }
}

/**
 * @brief   Low level serial driver stop.
 * @details De-initializes the USART, stops the associated clock, resets the
 *          interrupt vector.
 *
 * @param[in] sdp       pointer to a @p SerialDriver object
 *
 * @notapi
 */
void sd_lld_stop(SerialDriver *sdp) {

  if (sdp->state == SD_READY) {
#if KINETIS_SERIAL_USE_LPUART0 == TRUE
    if (&LPSD1 == sdp) {
      nvicDisableVector(LPUART0_IRQn);
      LPUART_Deinit(LPUART0);
    }
#endif
  }
}

#endif /* HAL_USE_SERIAL == TRUE */

/** @} */
