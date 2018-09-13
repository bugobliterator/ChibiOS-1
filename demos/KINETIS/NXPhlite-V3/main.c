/*
    ChibiOS - Copyright (C) 2006..2015 Giovanni Di Sirio

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

#include "ch.h"
#include "hal.h"
#include "ch_test.h"
#include "chprintf.h"
const SerialConfig sercfg = {
    .lpuart_config = {
        .baudRate_Bps = 115200,
        .parityMode = kLPUART_ParityDisabled,
        .dataBitsCount = kLPUART_EightDataBits,
        .isMsb = false,
        .stopBitCount = kLPUART_OneStopBit,
        .enableRxRTS = false,
        .enableTxCTS = false,
        .txCtsSource = 0,
        .txCtsConfig = 0,
        .rxIdleType = kLPUART_IdleTypeStartBit,
        .rxIdleConfig = kLPUART_IdleCharacter1,
        .enableTx = true,
        .enableRx = true,
    },
    .irq_cb = NULL,
    .ctx = NULL,
};
/*
 * LED blinker thread.
 */
static THD_WORKING_AREA(waThread1, 8192);
static THD_FUNCTION(Thread1, arg) {

    (void)arg;
    chRegSetThreadName("LEDBlinker");
    while (true) {
        palToggleLine(NXPHLITE_LED_R_LINE);
        palToggleLine(NXPHLITE_LED_G_LINE);
        palToggleLine(NXPHLITE_LED_B_LINE);
        chThdSleepMilliseconds(500);
    }
}

__attribute__((used))
void _getpid(void)
{
}

/*
 * Application entry point.
 */
int main(void) {

  /*
   * System initializations.
   * - HAL initialization, this also initializes the configured device drivers
   *   and performs the board-specific initializations.
   * - Kernel initialization, the main() function becomes a thread and the
   *   RTOS is active.
   */
  halInit();
  chSysInit();

  /*
   * Creates the blinker thread.
   */
  chThdCreateStatic(waThread1, sizeof(waThread1), NORMALPRIO, Thread1, NULL);
  sdStart(&LPSD1, NULL);
  while (true) {
    //chprintf((BaseSequentialStream*)&LPSD1, "Hello World!\n");
    test_execute((BaseSequentialStream*)&LPSD1);
    chThdSleepMilliseconds(1000);
  }

  return 0;
}
