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
 * @file    KINETIS/LLD/st_lld.h
 * @brief   ST Driver subsystem low level driver header.
 * @details This header is designed to be include-able without having to
 *          include other files from the HAL.
 *
 * @addtogroup ST
 * @{
 */

#ifndef HAL_ST_LLD_H_
#define HAL_ST_LLD_H_

#include "fsl_tpm.h"
#include "kinetis_registry.h"

/*===========================================================================*/
/* Driver constants.                                                         */
/*===========================================================================*/
//#define PORT_TIMER_TYPE_SINGLE_SHOT
/*===========================================================================*/
/* Driver pre-compile time settings.                                         */
/*===========================================================================*/

/**
 * @name    Configuration options
 * @{
 */
/**
 * @brief   SysTick timer IRQ priority.
 */
#if !defined(KINETIS_ST_IRQ_PRIORITY) || defined(__DOXYGEN__)
#define KINETIS_ST_IRQ_PRIORITY               8
#endif

/**
 * @brief   TIMx unit (by number) to be used for free running operations.
 * @note    Timers 1 and 2 are supported.
 */
#if !defined(KINETIS_ST_USE_TPM) || defined(__DOXYGEN__)
#define KINETIS_ST_USE_TPM                  1
#endif
/** @} */

/*===========================================================================*/
/* Derived constants and error checks.                                       */
/*===========================================================================*/

#if KINETIS_ST_USE_TPM == 1
#define ST_TPM        TPM1
#define ST_HANDLER    KINETIS_TPM1_IRQ_VECTOR
#define ST_VECTOR_NUM TPM1_IRQn
#elif KINETIS_ST_USE_TPM == 2
#define ST_TPM        TPM2
#define ST_HANDLER    KINETIS_TPM2_IRQ_VECTOR
#define ST_VECTOR_NUM TPM2_IRQn
#else
#error "KINETIS_ST_USE_TPM specifies an unsupported PIT Channel"
#endif

/** @} */

/*===========================================================================*/
/* Driver data structures and types.                                         */
/*===========================================================================*/

/*===========================================================================*/
/* Driver macros.                                                            */
/*===========================================================================*/

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif
  systime_t st_lld_get_counter(void);
  void st_lld_start_alarm(systime_t time);
  void st_lld_stop_alarm(void);
  void st_lld_set_alarm(systime_t time);
  systime_t st_lld_get_alarm(void);
  bool st_lld_is_alarm_active(void);
  void st_lld_init(void);
#ifdef __cplusplus
}
#endif

#endif /* HAL_ST_LLD_H_ */

/** @} */
