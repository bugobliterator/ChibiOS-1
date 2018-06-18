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

#include "fsl_pit.h"
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
/** @} */

/*===========================================================================*/
/* Derived constants and error checks.                                       */
/*===========================================================================*/
#define ST_HANDLER    KINETIS_PIT3_IRQ_VECTOR
#define ST_VECTOR_NUM PIT3_IRQn
/** @} */

/*===========================================================================*/
/* Driver inline functions.                                                  */
/*===========================================================================*/

/**
 * @brief   Returns the time counter value.
 *
 * @return              The counter value.
 *
 * @notapi
 */
inline static systime_t st_lld_get_counter(void) {
  return ~(PIT_GetCurrentTimerCount(PIT, kPIT_Chnl_1));
}

/**
 * @brief   Starts the alarm.
 * @note    Makes sure that no spurious alarms are triggered after
 *          this call.
 *
 * @param[in] time      the time to be set for the first alarm
 *
 * @notapi
 */
inline static  void st_lld_start_alarm(systime_t time) {
    /* We get here absolute interrupt time which takes into account counter overflow.
    * Since we use additional count-down timer to generate interrupt we need to calculate
    * load value based on time-stamp.
    */
  const uint32_t now_ticks = st_lld_get_counter();
  uint32_t delta_ticks =
    time >= now_ticks ? time - now_ticks : (uint32_t)((uint64_t) time + 0xFFFFFFFF - now_ticks);

  if (delta_ticks < CH_CFG_ST_TIMEDELTA) {
    /* The requested delay is less than the minimum resolution of this counter. */
    delta_ticks = CH_CFG_ST_TIMEDELTA;
  }

  PIT_StopTimer(PIT, kPIT_Chnl_3);
  PIT_StopTimer(PIT, kPIT_Chnl_2);
  PIT_SetTimerPeriod(PIT, kPIT_Chnl_3, delta_ticks - 1);
  PIT_EnableInterrupts(PIT, kPIT_Chnl_3, kPIT_TimerInterruptEnable);
  PIT_StartTimer(PIT, kPIT_Chnl_3);
  PIT_StartTimer(PIT, kPIT_Chnl_2);
}

/**
 * @brief   Stops the alarm interrupt.
 *
 * @notapi
 */
inline static  void st_lld_stop_alarm(void) {
  PIT_DisableInterrupts(PIT, kPIT_Chnl_3, kPIT_TimerInterruptEnable);
}

/**
 * @brief   Sets the alarm time.
 *
 * @param[in] time      the time to be set for the next alarm
 *
 * @notapi
 */
inline static  void st_lld_set_alarm(systime_t time) {
  st_lld_start_alarm(time);
}

/**
 * @brief   Returns the current alarm time.
 *
 * @return              The currently set alarm time.
 *
 * @notapi
 */
inline static  systime_t st_lld_get_alarm(void) {
  return PIT->CHANNEL[kPIT_Chnl_3].LDVAL;
}

/**
 * @brief   Determines if the alarm is active.
 *
 * @return              The alarm status.
 * @retval false        if the alarm is not active.
 * @retval true         is the alarm is active
 *
 * @notapi
 */
inline static  bool st_lld_is_alarm_active(void) {
  return PIT_GetEnabledInterrupts(PIT, kPIT_Chnl_3) & kPIT_TimerInterruptEnable;
}

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
  void st_lld_init(void);
#ifdef __cplusplus
}
#endif

#endif /* HAL_ST_LLD_H_ */

/** @} */
