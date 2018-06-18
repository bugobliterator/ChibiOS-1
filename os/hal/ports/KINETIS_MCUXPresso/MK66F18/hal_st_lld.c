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
#define PIT2ST(x) ((((uint64_t)(x)*(uint64_t)CH_CFG_ST_FREQUENCY) + (uint64_t)CLOCK_GetFreq(kCLOCK_BusClk) - 1ULL)/(uint64_t)CLOCK_GetFreq(kCLOCK_BusClk))
#define ST2PIT(x) ((((uint64_t)(x)*(uint64_t)CLOCK_GetFreq(kCLOCK_BusClk)) + (uint64_t)CH_CFG_ST_FREQUENCY - 1ULL)/(uint64_t)CH_CFG_ST_FREQUENCY)
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
  PIT_ClearStatusFlags(PIT, kPIT_Chnl_3, PIT_TFLG_TIF_MASK);
  PIT_ClearStatusFlags(PIT, kPIT_Chnl_2, PIT_TFLG_TIF_MASK);
  PIT_StopTimer(PIT, kPIT_Chnl_2);
  PIT_StopTimer(PIT, kPIT_Chnl_3);
  osalOsTimerHandlerI();
  osalSysUnlockFromISR();

  OSAL_IRQ_EPILOGUE();
}

#endif /* OSAL_ST_MODE == OSAL_ST_MODE_FREERUNNING */

/*===========================================================================*/
/* Driver exported functions.                                                */
/*===========================================================================*/
// #define SYS2ST(x) ((((uint64_t)x*(uint64_t)OSAL_ST_FREQUENCY) + (uint64_t)CLOCK_GetFreq(kCLOCK_CoreSysClk) - 1)/(uint64_t)CLOCK_GetFreq(kCLOCK_CoreSysClk))
// #define SYS2PIT(x) ((((uint64_t)x*(uint64_t)CLOCK_GetFreq(kCLOCK_BusClk)) + (uint64_t)CLOCK_GetFreq(kCLOCK_CoreSysClk) - 1)/(uint64_t)CLOCK_GetFreq(kCLOCK_CoreSysClk))

/**
 * @brief   Low level ST driver initialization.
 *
 * @notapi
 */
void st_lld_init(void) {
#if OSAL_ST_MODE == OSAL_ST_MODE_FREERUNNING
  pit_config_t pit_config;
  PIT_GetDefaultConfig(&pit_config);
  //pit_config.enableRunInDebug = 0;
  PIT_Init(PIT, &pit_config);

  //Setup virtual timer tick
  PIT_ClearStatusFlags(PIT, kPIT_Chnl_0, kPIT_TimerFlag);
  PIT_StopTimer(PIT, kPIT_Chnl_0);
  PIT_ClearStatusFlags(PIT, kPIT_Chnl_1, kPIT_TimerFlag);
  PIT_StopTimer(PIT, kPIT_Chnl_1);
  PIT_SetTimerChainMode(PIT, kPIT_Chnl_1, true);
  PIT_SetTimerPeriod(PIT, kPIT_Chnl_0, (CLOCK_GetFreq(kCLOCK_BusClk) / OSAL_ST_FREQUENCY) - 1);
  PIT_SetTimerPeriod(PIT, kPIT_Chnl_1, 0xFFFFFFFFUL);
  PIT_StartTimer(PIT, kPIT_Chnl_0);
  PIT_StartTimer(PIT, kPIT_Chnl_1);

  //Setup alarm timer
  PIT_ClearStatusFlags(PIT, kPIT_Chnl_2, kPIT_TimerFlag);
  PIT_StopTimer(PIT, kPIT_Chnl_2);
  PIT_ClearStatusFlags(PIT, kPIT_Chnl_3, kPIT_TimerFlag);
  PIT_StopTimer(PIT, kPIT_Chnl_3);
  PIT_SetTimerChainMode(PIT, kPIT_Chnl_3, true);
  PIT_SetTimerPeriod(PIT, kPIT_Chnl_2, (CLOCK_GetFreq(kCLOCK_BusClk) / OSAL_ST_FREQUENCY) - 1);
  nvicEnableVector(ST_VECTOR_NUM, KINETIS_ST_IRQ_PRIORITY);

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
