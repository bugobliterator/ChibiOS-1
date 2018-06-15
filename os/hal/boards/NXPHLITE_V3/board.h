/*
    ChibiOS - Copyright (C) 2015 RedoX https://github.com/RedoXyde

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

#ifndef _BOARD_H_
#define _BOARD_H_
#include "clock_config.h"
/*
 * Setup for the PJRC Teensy 3.6 board.
 */

/*
 * Board identifier.
 */
#define BOARD_NXPHLITE_V3
#define BOARD_NAME                  "NXPhlite V3"

/* Use internal capacitors for the crystal */
#define KINETIS_BOARD_OSCILLATOR_SETTING OSC_CR_SC8P(1)|OSC_CR_SC2P(1)

/*
 * MCU type
 */
#define MK66F18
#define NXPHLITE_LED_R_LINE  PAL_LINE(GPIOD, 1)

#define TEST_PIN              PAL_LINE(GPIOA, 6)

#define NXPHLITE_LED_B_LINE  PAL_LINE(GPIOC, 9)

#define NXPHLITE_LED_G_LINE  PAL_LINE(GPIOC, 8)

#if !defined(_FROM_ASM_)
#ifdef __cplusplus
extern "C" {
#endif
  void boardInit(void);
#ifdef __cplusplus
}
#endif
#endif /* _FROM_ASM_ */

#endif /* _BOARD_H_ */
