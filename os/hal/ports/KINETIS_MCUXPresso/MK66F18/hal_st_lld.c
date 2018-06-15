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
 * @file    KINETIS/LLD/st_lld.c
 * @brief   ST Driver subsystem low level driver code.
 *
 * @addtogroup ST
 * @{
 */

#include "hal.h"

#if (OSAL_ST_MODE != OSAL_ST_MODE_NONE) || defined(__DOXYGEN__)

/*===========================================================================*/
/* Driver local definitions.                                                 */
/*===========================================================================*/

/*===========================================================================*/
/* Derived constants and error checks.                                       */
/*===========================================================================*/
#define PIT2ST(x) ((((x)*OSAL_ST_FREQUENCY) + (uint64_t)CLOCK_GetFreq(kCLOCK_BusClk) - 1)/(uint64_t)CLOCK_GetFreq(kCLOCK_BusClk))
#define ST2PIT(x) ((((x)*(uint64_t)CLOCK_GetFreq(kCLOCK_BusClk)) + OSAL_ST_FREQUENCY - 1)/(uint64_t)OSAL_ST_FREQUENCY)
/*===========================================================================*/
/* Driver exported variables.                                                */
/*===========================================================================*/
uint64_t systick_time_offset = 0;
uint64_t last_time = 0;
/*===========================================================================*/
/* Driver local types.                                                       */
/*===========================================================================*/

/*===========================================================================*/
/* Driver local variables and types.                                         */
/*===========================================================================*/

/*===========================================================================*/
/* Driver local functions.                                                   */
/*===========================================================================*/

/*===========================================================================*/
/* Driver interrupt handlers.                                                */
/*===========================================================================*/

#if (OSAL_ST_MODE == OSAL_ST_MODE_PERIODIC) || defined(__DOXYGEN__)
/**
 * @brief   System Timer vector.
 * @details This interrupt is used for system tick in periodic mode.
 *
 * @isr
 */
OSAL_IRQ_HANDLER(SysTick_Handler) {

  OSAL_IRQ_PROLOGUE();

  osalSysLockFromISR();
  osalOsTimerHandlerI();
  osalSysUnlockFromISR();

  OSAL_IRQ_EPILOGUE();
}
#endif /* OSAL_ST_MODE == OSAL_ST_MODE_PERIODIC */

#if (OSAL_ST_MODE == OSAL_ST_MODE_FREERUNNING) || defined(__DOXYGEN__)
/**
 * @brief   PIT interrupt handler.
 * @details This interrupt is used for system tick in free running mode.
 *
 * @isr
 */
OSAL_IRQ_HANDLER(ST_HANDLER)
{
  OSAL_IRQ_PROLOGUE();

  osalSysLockFromISR();
  PIT_StopTimer(PIT, KINETIS_ST_USE_PIT_CH);
  osalOsTimerHandlerI();
  osalSysUnlockFromISR();

  OSAL_IRQ_EPILOGUE();
}

/**
 * @brief   PIT interrupt handler.
 * @details This interrupt is used for system tick in free running mode.
 *
 * @isr
 */
OSAL_IRQ_HANDLER(KINETIS_PIT2_IRQ_VECTOR)
{
  OSAL_IRQ_PROLOGUE();

  osalSysLockFromISR();
  systick_time_offset += (systime_t)PIT2ST(1ULL << 32);
  PIT_ClearStatusFlags(PIT, kPIT_Chnl_2, kPIT_TimerFlag);
  osalSysUnlockFromISR();

  OSAL_IRQ_EPILOGUE();
}
pit_config_t pit_config;
#endif /* OSAL_ST_MODE == OSAL_ST_MODE_FREERUNNING */

/*===========================================================================*/
/* Driver exported functions.                                                */
/*===========================================================================*/
// #define SYS2ST(x) ((((uint64_t)x*(uint64_t)OSAL_ST_FREQUENCY) + (uint64_t)CLOCK_GetFreq(kCLOCK_CoreSysClk) - 1)/(uint64_t)CLOCK_GetFreq(kCLOCK_CoreSysClk))
// #define SYS2PIT(x) ((((uint64_t)x*(uint64_t)CLOCK_GetFreq(kCLOCK_BusClk)) + (uint64_t)CLOCK_GetFreq(kCLOCK_CoreSysClk) - 1)/(uint64_t)CLOCK_GetFreq(kCLOCK_CoreSysClk))

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
systime_t st_lld_get_counter(void) {
  return systick_time_offset + (systime_t)PIT2ST(0xFFFFFFFFULL - PIT_GetCurrentTimerCount(PIT, kPIT_Chnl_2));
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
void st_lld_start_alarm(systime_t time) {
  PIT_ClearStatusFlags(PIT, KINETIS_ST_USE_PIT_CH, kPIT_TimerFlag);
  PIT_StopTimer(PIT, KINETIS_ST_USE_PIT_CH);
  PIT_SetTimerPeriod(PIT, KINETIS_ST_USE_PIT_CH, ST2PIT(time));
  PIT_EnableInterrupts(PIT, KINETIS_ST_USE_PIT_CH, kPIT_TimerInterruptEnable);
  PIT_StartTimer(PIT, KINETIS_ST_USE_PIT_CH);
  PIT_StartTimer(PIT, kPIT_Chnl_2);
  PIT_SetTimerPeriod(PIT, kPIT_Chnl_2, 0xFFFFFFFF);
  PIT_EnableInterrupts(PIT, kPIT_Chnl_2, kPIT_TimerInterruptEnable);
}

/**
 * @brief   Stops the alarm interrupt.
 *
 * @notapi
 */
void st_lld_stop_alarm(void) {
  PIT_ClearStatusFlags(PIT, KINETIS_ST_USE_PIT_CH, kPIT_TimerFlag);
  PIT_DisableInterrupts(PIT, KINETIS_ST_USE_PIT_CH, kPIT_TimerInterruptEnable);
  PIT_StopTimer(PIT, KINETIS_ST_USE_PIT_CH);
}

/**
 * @brief   Sets the alarm time.
 *
 * @param[in] time      the time to be set for the next alarm
 *
 * @notapi
 */
void st_lld_set_alarm(systime_t time) {
  PIT_ClearStatusFlags(PIT, KINETIS_ST_USE_PIT_CH, kPIT_TimerFlag);
  PIT_StopTimer(PIT, KINETIS_ST_USE_PIT_CH);
  PIT_SetTimerPeriod(PIT, KINETIS_ST_USE_PIT_CH, ST2PIT(time));
  PIT_StartTimer(PIT, KINETIS_ST_USE_PIT_CH);
}

/**
 * @brief   Returns the current alarm time.
 *
 * @return              The currently set alarm time.
 *
 * @notapi
 */
systime_t st_lld_get_alarm(void) {
  return PIT2ST(PIT->CHANNEL[KINETIS_ST_USE_PIT_CH].LDVAL);
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
bool st_lld_is_alarm_active(void) {
  return PIT_GetEnabledInterrupts(PIT, KINETIS_ST_USE_PIT_CH) & kPIT_TimerInterruptEnable;
}

/**
 * @brief   Low level ST driver initialization.
 *
 * @notapi
 */
void st_lld_init(void) {
#if OSAL_ST_MODE == OSAL_ST_MODE_FREERUNNING
  pit_config.enableRunInDebug = 0;
  PIT_Init(PIT, &pit_config);
  nvicEnableVector(ST_VECTOR_NUM, KINETIS_ST_IRQ_PRIORITY);
  nvicEnableVector(PIT2_IRQn, KINETIS_ST_IRQ_PRIORITY);
#endif
#if OSAL_ST_MODE == OSAL_ST_MODE_PERIODIC
  /* Periodic systick mode, the Cortex-Mx internal systick timer is used
     in this mode.*/
  SysTick->LOAD = (SystemCoreClock / OSAL_ST_FREQUENCY) - 1;
  SysTick->VAL = 0;
  SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk |
                  SysTick_CTRL_ENABLE_Msk |
                  SysTick_CTRL_TICKINT_Msk;

  /* IRQ enabled.*/
  nvicSetSystemHandlerPriority(HANDLER_SYSTICK, KINETIS_ST_IRQ_PRIORITY);
#endif /* OSAL_ST_MODE == OSAL_ST_MODE_PERIODIC */
}

#endif /* OSAL_ST_MODE != OSAL_ST_MODE_NONE */

/** @} */
