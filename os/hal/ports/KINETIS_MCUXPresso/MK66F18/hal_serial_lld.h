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
 * @file    hal_serial_lld.h
 * @brief   PLATFORM serial subsystem low level driver header.
 *
 * @addtogroup SERIAL
 * @{
 */

#ifndef HAL_SERIAL_LLD_H
#define HAL_SERIAL_LLD_H

#if (HAL_USE_SERIAL == TRUE) || defined(__DOXYGEN__)
#include "fsl_uart.h"
#include "fsl_lpuart.h"
/*===========================================================================*/
/* Driver constants.                                                         */
/*===========================================================================*/

/*===========================================================================*/
/* Driver pre-compile time settings.                                         */
/*===========================================================================*/

/**
 * @name    Kinetis configuration options
 * @{
 */
/**
 * @brief   USART0 driver enable switch.
 * @details If set to @p TRUE the support for USART1 is included.
 * @note    The default is @p FALSE.
 */
#if !defined(KINETIS_SERIAL_USE_LPUART0) || defined(__DOXYGEN__)
#define KINETIS_SERIAL_USE_LPUART0             FALSE
#endif

/**
 * @brief   USART1 interrupt priority level setting.
 */
#if !defined(KINETIS_SERIAL_LPUART0_PRIORITY) || defined(__DOXYGEN__)
#define KINETIS_SERIAL_LPUART0_PRIORITY        12
#endif
/** @} */

/*===========================================================================*/
/* Driver data structures and types.                                         */
/*===========================================================================*/
  
typedef void (*uartirq_t)(void* sd);

#define _serial_irq_code(sd) {                                      \
  if ((sd)->config->irq_cb != NULL)                                 \
    (sd)->config->irq_cb((sd)->config->ctx);                        \
}
/**
 * @brief   Kinetis Serial Driver configuration structure.
 * @details An instance of this structure must be passed to @p sdStart()
 *          in order to configure and start a serial driver operations.
 * @note    This structure content is architecture dependent, each driver
 *          implementation defines its own version and the custom static
 *          initializers.
 */
typedef struct {
  union {
    lpuart_config_t           lpuart_config;
    uart_config_t             uart_config;
  };
  uartirq_t                   irq_cb;
  void*                       ctx;
} SerialConfig;


typedef void Base_Type;

/**
 * @brief   @p SerialDriver specific data.
 */
#define _serial_driver_data                                                 \
  _base_asynchronous_channel_data                                           \
  /* Driver state.*/                                                        \
  sdstate_t                 state;                                          \
  /* Input queue.*/                                                         \
  input_queue_t             iqueue;                                         \
  /* Output queue.*/                                                        \
  output_queue_t            oqueue;                                         \
  /* Input circular buffer.*/                                               \
  uint8_t                   ib[SERIAL_BUFFERS_SIZE];                        \
  /* Output circular buffer.*/                                              \
  uint8_t                   ob[SERIAL_BUFFERS_SIZE];                        \
  /* End of the mandatory fields.*/                                         \
  bool                      is_lp;                                          \
  SerialConfig              *config;                                        \
  Base_Type*                uart;                                           \

/*===========================================================================*/
/* Driver macros.                                                            */
/*===========================================================================*/

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

#if (KINETIS_SERIAL_USE_LPUART0 == TRUE) && !defined(__DOXYGEN__)
extern SerialDriver LPSD1;
#endif

#ifdef __cplusplus
extern "C" {
#endif
  void sd_lld_init(void);
  void sd_lld_start(SerialDriver *sdp, const SerialConfig *config);
  void sd_lld_stop(SerialDriver *sdp);
#ifdef __cplusplus
}
#endif

#endif /* HAL_USE_SERIAL == TRUE */

#endif /* HAL_SERIAL_LLD_H */

/** @} */
