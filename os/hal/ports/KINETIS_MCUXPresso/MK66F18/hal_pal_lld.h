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
 * @file    KINETIS/LLD/pal_lld.h
 * @brief   PAL subsystem low level driver header.
 *
 * @addtogroup PAL
 * @{
 */

#ifndef HAL_PAL_LLD_H_
#define HAL_PAL_LLD_H_
#include "fsl_gpio.h"
#include "fsl_port.h"

#if HAL_USE_PAL || defined(__DOXYGEN__)


/*===========================================================================*/
/* Driver constants.                                                         */
/*===========================================================================*/

/**
 * @name    Pads mode constants
 * @{
 */
/**
 * @brief   After reset state.
 * @details The state itself is not specified and is architecture dependent,
 *          it is guaranteed to be equal to the after-reset state. It is
 *          usually an input state.
 */
#define PAL_MODE_RESET_MASK                  0U

/**
 * @brief   Safe state for <b>unconnected</b> pads.
 * @details The state itself is not specified and is architecture dependent,
 *          it may be mapped on @p PAL_MODE_INPUT_PULLUP,
 *          @p PAL_MODE_INPUT_PULLDOWN or @p PAL_MODE_OUTPUT_PUSHPULL for
 *          example.
 */
#define PAL_MODE_UNCONNECTED_MASK            (1U<<PAL_MODE_UNCONNECTED)

/**
 * @brief   Regular input high-Z pad.
 */
#define PAL_MODE_INPUT_MASK                  (1U<<PAL_MODE_INPUT)

/**
 * @brief   Input pad with weak pull up resistor.
 */
#define PAL_MODE_INPUT_PULLUP_MASK           (1U<<PAL_MODE_INPUT_PULLUP)

/**
 * @brief   Input pad with weak pull down resistor.
 */
#define PAL_MODE_INPUT_PULLDOWN_MASK         (1U<<PAL_MODE_INPUT_PULLDOWN)

/**
 * @brief   Analog input mode.
 */
#define PAL_MODE_INPUT_ANALOG_MASK           (1U<<PAL_MODE_INPUT_ANALOG)

/**
 * @brief   Output pad.
 */
#define PAL_MODE_OUTPUT_MASK                 (1U<<PAL_MODE_OUTPUT_PUSHPULL)

/**
 * @brief   Open-drain output pad.
 */
#define PAL_MODE_OUTPUT_OPENDRAIN_MASK       (1U<<PAL_MODE_OUTPUT_OPENDRAIN)
/** @} */
#define PAL_MODE_ALTERNATIVE_MASK            (0x7F00U | PAL_MODE_INPUT_ANALOG_MASK | PAL_MODE_UNCONNECTED_MASK)
#define PAL_MODE_ALTERNATIVE_1_MASK          (1U<<8U)
#define PAL_MODE_ALTERNATIVE_2_MASK          (1U<<9U)
#define PAL_MODE_ALTERNATIVE_3_MASK          (1U<<10U)
#define PAL_MODE_ALTERNATIVE_4_MASK          (1U<<11U)
#define PAL_MODE_ALTERNATIVE_5_MASK          (1U<<12U)
#define PAL_MODE_ALTERNATIVE_6_MASK          (1U<<13U)
#define PAL_MODE_ALTERNATIVE_7_MASK          (1U<<14U)

/**
 * @brief   Width, in bits, of an I/O port.
 */
#define PAL_IOPORTS_WIDTH 32

/**
 * @brief   Digital I/O port sized unsigned type.
 */
typedef uint32_t ioportmask_t;

/**
 * @brief   Digital I/O modes.
 */
typedef uint32_t iomode_t;

/**
 * @brief   Type of an I/O line.
 */
typedef uint32_t ioline_t;

/**
 * @brief   Port Identifier.
 * @details This type can be a scalar or some kind of pointer, do not make
 *          any assumption about it, use the provided macros when populating
 *          variables of this type.
 */
typedef GPIO_Type *ioportid_t;

/**
 * @brief   Port Configuration.
 * @details This structure stores the configuration parameters of all pads
 *          belonging to a port.
 */
typedef struct {
    ioportid_t      port;
    iomode_t        pads[32];
} PortConfig;

/**
 * @brief   Generic I/O ports static initializer.
 * @details An instance of this structure must be passed to @p palInit() at
 *          system startup time in order to initialized the digital I/O
 *          subsystem. This represents only the initial setup, specific pads
 *          or whole ports can be reprogrammed at later time.
 * @note    Implementations may extend this structure to contain more,
 *          architecture dependent, fields.
 */
typedef struct {
    PortConfig      ports[FSL_FEATURE_SOC_PORT_COUNT];
} PALConfig;

/*===========================================================================*/
/* I/O Ports Identifiers.                                                    */
/*===========================================================================*/

/**
 * @brief   GPIO port A identifier.
 */
#define IOPORT1          GPIOA

/**
 * @brief   GPIO port B identifier.
 */
#define IOPORT2          GPIOB

/**
 * @brief   GPIO port C identifier.
 */
#define IOPORT3          GPIOC

/**
 * @brief   GPIO port D identifier.
 */
#define IOPORT4          GPIOD

/**
 * @brief   GPIO port E identifier.
 */
#define IOPORT5          GPIOE

/**
 * @name    Line handling macros
 * @{
 */
/**
 * @brief   Forms a line identifier.
 * @details A port/pad pair are encoded into an @p ioline_t type. The encoding
 *          of this type is platform-dependent.
 * @note    In this driver the pad number is encoded in the byte of the GPIO
 *          address that's zero on all Kinetis devices.
 */
#define PAL_LINE(port, pad)                                                 \
  ((ioline_t)((uint32_t)(port) | ((uint32_t)(pad)<<20)))

/**
 * @brief   Decodes a port identifier from a line identifier.
 */
#define PAL_PORT(line)                                                      \
  ((GPIO_Type *)(((uint32_t)(line)) & 0xF00FFFFFU))

/**
 * @brief   Decodes a pad identifier from a line identifier.
 */
#define PAL_PAD(line)                                                       \
  ((uint32_t)((uint32_t)(line) & 0x0FF00000U)>>20)

/**
 * @brief   Value identifying an invalid line.
 */
#define PAL_NOLINE                      0U
/** @} */

/*===========================================================================*/
/* Implementation, some of the following macros could be implemented as      */
/* functions, if so please put them in pal_lld.c.                            */
/*===========================================================================*/

/**
 * @brief   Low level PAL subsystem initialization.
 *
 * @param[in] config    architecture-dependent ports configuration
 *
 * @notapi
 */
#define pal_lld_init(config) _pal_lld_init(config)

/**
 * @brief   Reads the physical I/O port states.
 *
 * @param[in] port      port identifier
 * @return              The port bits.
 *
 * @notapi
 */
#define pal_lld_readport(port) \
          (port)->PDIR

/**
 * @brief   Reads the output latch.
 * @details The purpose of this function is to read back the latched output
 *          value.
 *
 * @param[in] port      port identifier
 * @return              The latched logical states.
 *
 * @notapi
 */
#define pal_lld_readlatch(port) \
          (port)->PDOR

/**
 * @brief   Writes a bits mask on a I/O port.
 *
 * @param[in] port      port identifier
 * @param[in] bits      bits to be written on the specified port
 *
 * @notapi
 */
#define pal_lld_writeport(port, bits) \
          (port)->PDOR = (bits)

/**
 * @brief   Sets a bits mask on a I/O port.
 * @note    The @ref PAL provides a default software implementation of this
 *          functionality, implement this function if can optimize it by using
 *          special hardware functionalities or special coding.
 *
 * @param[in] port      port identifier
 * @param[in] bits      bits to be ORed on the specified port
 *
 * @notapi
 */
#define pal_lld_setport(port, bits) \
          GPIO_PortSet(port, bits)

/**
 * @brief   Clears a bits mask on a I/O port.
 * @note    The @ref PAL provides a default software implementation of this
 *          functionality, implement this function if can optimize it by using
 *          special hardware functionalities or special coding.
 *
 * @param[in] port      port identifier
 * @param[in] bits      bits to be cleared on the specified port
 *
 * @notapi
 */
#define pal_lld_clearport(port, bits) \
          GPIO_PortClear(port, bits)

/**
 * @brief   Toggles a bits mask on a I/O port.
 * @note    The @ref PAL provides a default software implementation of this
 *          functionality, implement this function if can optimize it by using
 *          special hardware functionalities or special coding.
 *
 * @param[in] port      port identifier
 * @param[in] bits      bits to be toggled on the specified port
 *
 * @notapi
 */
#define pal_lld_toggleport(port, bits) \
          GPIO_PortToggle(port, bits)

/**
 * @brief   Reads a group of bits.
 * @note    The @ref PAL provides a default software implementation of this
 *          functionality, implement this function if can optimize it by using
 *          special hardware functionalities or special coding.
 *
 * @param[in] port      port identifier
 * @param[in] mask      group mask
 * @param[in] offset    group bit offset within the port
 * @return              The group logical states.
 *
 * @notapi
 */
//#define pal_lld_readgroup(port, mask, offset) 0

/**
 * @brief   Writes a group of bits.
 * @note    The @ref PAL provides a default software implementation of this
 *          functionality, implement this function if can optimize it by using
 *          special hardware functionalities or special coding.
 *
 * @param[in] port      port identifier
 * @param[in] mask      group mask
 * @param[in] offset    group bit offset within the port
 * @param[in] bits      bits to be written. Values exceeding the group width
 *                      are masked.
 *
 * @notapi
 */
//#define pal_lld_writegroup(port, mask, offset, bits) (void)bits

/**
 * @brief   Pads group mode setup.
 * @details This function programs a pads group belonging to the same port
 *          with the specified mode.
 * @note    Programming an unknown or unsupported mode is silently ignored.
 *
 * @param[in] port      port identifier
 * @param[in] mask      group mask
 * @param[in] offset    group bit offset within the port
 * @param[in] mode      group mode
 *
 * @notapi
 */
#define pal_lld_setgroupmode(port, mask, offset, mode)                      \
  _pal_lld_setgroupmode(port, mask << offset, mode)

/**
 * @brief   Reads a logical state from an I/O pad.
 * @note    The @ref PAL provides a default software implementation of this
 *          functionality, implement this function if can optimize it by using
 *          special hardware functionalities or special coding.
 *
 * @param[in] port      port identifier
 * @param[in] pad       pad number within the port
 * @return              The logical state.
 * @retval PAL_LOW      low logical state.
 * @retval PAL_HIGH     high logical state.
 *
 * @notapi
 */
#define pal_lld_readpad(port, pad) _pal_lld_readpad(port, pad)

/**
 * @brief   Writes a logical state on an output pad.
 * @note    This function is not meant to be invoked directly by the
 *          application  code.
 * @note    The @ref PAL provides a default software implementation of this
 *          functionality, implement this function if can optimize it by using
 *          special hardware functionalities or special coding.
 *
 * @param[in] port      port identifier
 * @param[in] pad       pad number within the port
 * @param[in] bit       logical value, the value must be @p PAL_LOW or
 *                      @p PAL_HIGH
 *
 * @notapi
 */
#define pal_lld_writepad(port, pad, bit) _pal_lld_writepad(port, pad, bit)

/**
 * @brief   Sets a pad logical state to @p PAL_HIGH.
 * @note    The @ref PAL provides a default software implementation of this
 *          functionality, implement this function if can optimize it by using
 *          special hardware functionalities or special coding.
 *
 * @param[in] port      port identifier
 * @param[in] pad       pad number within the port
 *
 * @notapi
 */
#define pal_lld_setpad(port, pad) GPIO_PortSet(port, ((uint32_t) 1 << (pad)))

/**
 * @brief   Clears a pad logical state to @p PAL_LOW.
 * @note    The @ref PAL provides a default software implementation of this
 *          functionality, implement this function if can optimize it by using
 *          special hardware functionalities or special coding.
 *
 * @param[in] port      port identifier
 * @param[in] pad       pad number within the port
 *
 * @notapi
 */
#define pal_lld_clearpad(port, pad) GPIO_PortClear(port, ((uint32_t) 1 << (pad)))

/**
 * @brief   Toggles a pad logical state.
 * @note    The @ref PAL provides a default software implementation of this
 *          functionality, implement this function if can optimize it by using
 *          special hardware functionalities or special coding.
 *
 * @param[in] port      port identifier
 * @param[in] pad       pad number within the port
 *
 * @notapi
 */
#define pal_lld_togglepad(port, pad) GPIO_PortToggle(port, ((uint32_t) 1 << (pad)))

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
#define pal_lld_setpadmode(port, pad, mode)                                 \
    _pal_lld_setpadmode(port, pad, mode)

#if !defined(__DOXYGEN__)
extern const PALConfig pal_default_config;
#endif

#ifdef __cplusplus
extern "C" {
#endif
  void _pal_lld_init(const PALConfig *config);
  void _pal_lld_setgroupmode(ioportid_t port,
                             ioportmask_t mask,
                             iomode_t mode);
  void _pal_lld_setpadmode(ioportid_t port,
                           uint8_t pad,
                           iomode_t mode);
  uint8_t _pal_lld_readpad(ioportid_t port,
                           uint8_t pad);
  void _pal_lld_writepad(ioportid_t port,
                         uint8_t pad,
                         uint8_t bit);
#ifdef __cplusplus
}
#endif

#endif /* HAL_USE_PAL */

#endif /* HAL_PAL_LLD_H_ */

/** @} */
