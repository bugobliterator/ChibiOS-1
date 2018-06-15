/*
    ChibiOS - Copyright (C) 2014-2015 Fabio Utzig

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
 * @file    KINETIS/LLD/pal_lld.c
 * @brief   PAL subsystem low level driver.
 *
 * @addtogroup PAL
 * @{
 */

#include "osal.h"
#include "hal.h"

#if HAL_USE_PAL || defined(__DOXYGEN__)

/*===========================================================================*/
/* Driver local definitions.                                                 */
/*===========================================================================*/

/*===========================================================================*/
/* Driver exported variables.                                                */
/*===========================================================================*/

/*===========================================================================*/
/* Driver local variables and types.                                         */
/*===========================================================================*/

/*===========================================================================*/
/* Driver local functions.                                                   */
/*===========================================================================*/

/**
 * @brief Reads a logical state from an I/O pad.
 * @note The @ref PAL provides a default software implementation of this
 * functionality, implement this function if can optimize it by using
 * special hardware functionalities or special coding.
 *
 * @param[in] port port identifier
 * @param[in] pad pad number within the port
 * @return The logical state.
 * @retval PAL_LOW low logical state.
 * @retval PAL_HIGH high logical state.
 *
 * @notapi
 */
uint8_t _pal_lld_readpad(ioportid_t port,
                         uint8_t pad) {

  return (uint8_t)GPIO_PinRead(port, pad);
}

/**
 * @brief Writes a logical state on an output pad.
 * @note This function is not meant to be invoked directly by the
 * application code.
 * @note The @ref PAL provides a default software implementation of this
 * functionality, implement this function if can optimize it by using
 * special hardware functionalities or special coding.
 *
 * @param[in] port port identifier
 * @param[in] pad pad number within the port
 * @param[in] bit logical value, the value must be @p PAL_LOW or
 * @p PAL_HIGH
 *
 * @notapi
 */
void _pal_lld_writepad(ioportid_t port,
                       uint8_t pad,
                       uint8_t bit) {
  GPIO_PinWrite(port, pad, bit);
}

/**
 * @brief   Pad mode setup.
 * @details This function programs a pad with the specified mode.
 * @note    The @ref PAL provides a default software implementation of this
 *          functionality, implement this function if can optimize it by using
 *          special hardware functionalities or special coding.
 * @note    Programming an unknown or unsupported mode is silently ignored.
 *
 * @param[in] port      port identifier
 * @param[in] pad       pad number within the port
 * @param[in] mode      pad mode
 *
 * @notapi
 */
void _pal_lld_setpadmode(ioportid_t port,
                         uint8_t pad,
                         iomode_t mode) {

  PORT_Type *portcfg = NULL;
  port_pin_config_t port_pin_config;
  gpio_pin_config_t gpio_pin_config;
  osalDbgAssert(pad < PAL_IOPORTS_WIDTH, "pal_lld_setpadmode() #1, invalid pad");

  if (mode == PAL_MODE_OUTPUT_PUSHPULL) {
    gpio_pin_config.pinDirection = kGPIO_DigitalOutput;
    GPIO_PinInit(port, pad, (const gpio_pin_config_t*)&gpio_pin_config);
  } else { 
    gpio_pin_config.pinDirection = kGPIO_DigitalInput;
    GPIO_PinInit(port, pad, (const gpio_pin_config_t*)&gpio_pin_config);
  }
  if (port == IOPORT1)
    portcfg = PORTA;
  else if (port == IOPORT2)
    portcfg = PORTB;
  else if (port == IOPORT3)
    portcfg = PORTC;
  else if (port == IOPORT4)
    portcfg = PORTD;
  else if (port == IOPORT5)
    portcfg = PORTE;

  osalDbgAssert(portcfg != NULL, "pal_lld_setpadmode() #2, invalid port");
  port_pin_config.mux = 1;
  switch (mode) {
#if defined(FSL_FEATURE_PORT_HAS_OPEN_DRAIN) && FSL_FEATURE_PORT_HAS_OPEN_DRAIN
  case PAL_MODE_OUTPUT_OPENDRAIN:
    port_pin_config.openDrainEnable = kPORT_OpenDrainEnable;
    break;
#endif
  case PAL_MODE_INPUT_PULLUP:
    port_pin_config.pullSelect = kPORT_PullUp;
    break;
  case PAL_MODE_INPUT_PULLDOWN:
    port_pin_config.pullSelect = kPORT_PullDown;
    break;
  case PAL_MODE_UNCONNECTED:
  case PAL_MODE_INPUT_ANALOG:
    port_pin_config.mux = 0;
    break;
  case PAL_MODE_ALTERNATIVE_2:
    port_pin_config.mux = 2;
    break;
  case PAL_MODE_ALTERNATIVE_3:
    port_pin_config.mux = 3;
    break;
  case PAL_MODE_ALTERNATIVE_4:
    port_pin_config.mux = 4;
    break;
  case PAL_MODE_ALTERNATIVE_5:
    port_pin_config.mux = 5;
    break;
  case PAL_MODE_ALTERNATIVE_6:
    port_pin_config.mux = 6;
    break;
  case PAL_MODE_ALTERNATIVE_7:
    port_pin_config.mux = 7;
    break;
  }
  PORT_SetPinConfig(portcfg, pad, (const port_pin_config_t*)&port_pin_config);
}

/*===========================================================================*/
/* Driver interrupt handlers.                                                */
/*===========================================================================*/

/*===========================================================================*/
/* Driver exported functions.                                                */
/*===========================================================================*/

/**
 * @brief   Kinetis I/O ports configuration.
 * @details Ports A-E clocks enabled.
 *
 * @param[in] config    the Kinetis ports configuration
 *
 * @notapi
 */
void _pal_lld_init(const PALConfig *config) {

  int i, j;

  /* Enable clocking on all Ports */
  CLOCK_EnableClock(kCLOCK_PortA);
  CLOCK_EnableClock(kCLOCK_PortB);
  CLOCK_EnableClock(kCLOCK_PortC);
  CLOCK_EnableClock(kCLOCK_PortD);
  CLOCK_EnableClock(kCLOCK_PortE);

  /* Initial PORT and GPIO setup */
  for (i = 0; i < FSL_FEATURE_SOC_PORT_COUNT; i++) {
    for (j = 0; j < PAL_IOPORTS_WIDTH; j++) {
      pal_lld_setpadmode(config->ports[i].port,
                         j,
                         config->ports[i].pads[j]);
    }
  }
}

/**
 * @brief   Pads mode setup.
 * @details This function programs a pads group belonging to the same port
 *          with the specified mode.
 *
 * @param[in] port      the port identifier
 * @param[in] mask      the group mask
 * @param[in] mode      the mode
 *
 * @notapi
 */
void _pal_lld_setgroupmode(ioportid_t port,
                           ioportmask_t mask,
                           iomode_t mode) {

  int i;

  (void)mask;

  for (i = 0; i < PAL_IOPORTS_WIDTH; i++) {
    pal_lld_setpadmode(port, i, mode);
  }
}

#endif /* HAL_USE_PAL */

/** @} */
