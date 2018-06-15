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
/* Get source clock for TPM driver */
#define TPM_SOURCE_CLOCK (CLOCK_GetFreq(kCLOCK_Osc0ErClk))

#ifndef ST_TPM_PRESCALER
#define ST_TPM_PRESCALER 4      //DIV by 16
#endif

/*===========================================================================*/
/* Driver exported variables.                                                */
/*===========================================================================*/

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
  osalOsTimerHandlerI();
  osalSysUnlockFromISR();

  OSAL_IRQ_EPILOGUE();
}
#endif /* OSAL_ST_MODE == OSAL_ST_MODE_FREERUNNING */


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
  return TPM_GetCurrentTimerCount(ST_TPM);
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
  ST_TPM->CONTROLS[0].CnV = time;
  TPM_EnableInterrupts(ST_TPM, kTPM_Chnl0InterruptEnable);
  TPM_StartTimer(ST_TPM, kTPM_SystemClock);
}

/**
 * @brief   Stops the alarm interrupt.
 *
 * @notapi
 */
void st_lld_stop_alarm(void) {
  TPM_DisableInterrupts(ST_TPM, kTPM_Chnl0InterruptEnable);
  TPM_StopTimer(ST_TPM);
}

/**
 * @brief   Sets the alarm time.
 *
 * @param[in] time      the time to be set for the next alarm
 *
 * @notapi
 */
void st_lld_set_alarm(systime_t time) {
  ST_TPM->CONTROLS[0].CnV = time;
}

/**
 * @brief   Returns the current alarm time.
 *
 * @return              The currently set alarm time.
 *
 * @notapi
 */
systime_t st_lld_get_alarm(void) {
  return ST_TPM->CONTROLS[0].CnV;
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
  return TPM_GetEnabledInterrupts(ST_TPM) & kTPM_Chnl0InterruptEnable;
}

/**
 * @brief   Low level ST driver initialization.
 *
 * @notapi
 */
void st_lld_init(void) {
#if OSAL_ST_MODE == OSAL_ST_MODE_FREERUNNING
  /* Periodic systick mode, the Cortex-Mx internal systick timer is used
     for time keeping*/
  /* TPM known issue of KL81, enable clock of TPM0 to use other TPM module */
  CLOCK_EnableClock(kCLOCK_Tpm2);
  /* Select the clock source for the TPM counter as kCLOCK_PllFllSelClk */
  CLOCK_SetTpmClock(2U);
  tpm_config_t tpm_config = {0};

  /* TPM clock divide by TPM_PRESCALER */
  tpm_config.prescale = ST_TPM_PRESCALER;
  tpm_config.enableDebugMode = true;
  /* Initialize TPM module */
  TPM_Init(ST_TPM, &tpm_config);
  TPM_SetTimerPeriod(ST_TPM, 0xFFFF);
  ST_TPM->CONTROLS[0].CnSC = TPM_CnSC_MSA_MASK;
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
