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
#include "hal.h"

#if HAL_USE_PAL || defined(__DOXYGEN__)

/**
 * @brief   PAL setup.
 * @details Digital I/O ports static configuration as defined in @p board.h.
 *          This variable is used by the HAL when initializing the PAL driver.
 */
const PALConfig pal_default_config =
    {
        .ports = {
            {
            /*
             * PORTA setup.
             */

                .port = IOPORT1,
                .pads = {
                    PAL_MODE_ALTERNATIVE_7_MASK,        //PTA0  SWD
                    PAL_MODE_UNCONNECTED_MASK,      //PTA1
                    PAL_MODE_UNCONNECTED_MASK,      //PTA2
                    PAL_MODE_ALTERNATIVE_7_MASK,        //PTA3  SWD
                    PAL_MODE_UNCONNECTED_MASK,      //PTA4
                    PAL_MODE_OUTPUT_MASK,       //PTA5
                    PAL_MODE_OUTPUT_MASK,       //PTA6
                    PAL_MODE_UNCONNECTED_MASK,      //PTA7
                    PAL_MODE_UNCONNECTED_MASK,      //PTA8
                    PAL_MODE_UNCONNECTED_MASK,      //PTA9
                    PAL_MODE_UNCONNECTED_MASK,      //PTA10
                    PAL_MODE_UNCONNECTED_MASK,      //PTA11
                    PAL_MODE_OUTPUT_MASK,       //PTA12
                    PAL_MODE_OUTPUT_MASK,       //PTA13
                    PAL_MODE_OUTPUT_MASK,       //PTA14
                    PAL_MODE_OUTPUT_MASK,       //PTA15
                    PAL_MODE_OUTPUT_MASK,       //PTA16
                    PAL_MODE_OUTPUT_MASK,       //PTA17
                    PAL_MODE_INPUT_ANALOG_MASK,     //PTA18 Crystal
                    PAL_MODE_INPUT_ANALOG_MASK,     //PTA19 Crystal
                    PAL_MODE_UNCONNECTED_MASK,      //PTA20
                    PAL_MODE_UNCONNECTED_MASK,      //PTA21
                    PAL_MODE_UNCONNECTED_MASK,      //PTA22
                    PAL_MODE_UNCONNECTED_MASK,      //PTA23
                    PAL_MODE_UNCONNECTED_MASK,      //PTA24
                    PAL_MODE_OUTPUT_MASK,      //PTA25
                    PAL_MODE_OUTPUT_MASK,       //PTA26
                    PAL_MODE_UNCONNECTED_MASK,      //PTA27
                    PAL_MODE_OUTPUT_MASK,       //PTA28
                    PAL_MODE_OUTPUT_MASK,       //PTA29
                    PAL_MODE_UNCONNECTED_MASK,      //PTA30
                    PAL_MODE_UNCONNECTED_MASK,      //PTA31
                },
            },
            {
            /*
             * PORTB setup.
             */
                .port = IOPORT2,
                .pads = {
                    PAL_MODE_OUTPUT_MASK,       //PTB0
                    PAL_MODE_OUTPUT_MASK,       //PTB1
                    PAL_MODE_OUTPUT_MASK,       //PTB2
                    PAL_MODE_OUTPUT_MASK,       //PTB3
                    PAL_MODE_OUTPUT_MASK,       //PTB4
                    PAL_MODE_OUTPUT_MASK,       //PTB5
                    PAL_MODE_UNCONNECTED_MASK,      //PTB6
                    PAL_MODE_UNCONNECTED_MASK,      //PTB7
                    PAL_MODE_OUTPUT_MASK,      //PTB8
                    PAL_MODE_OUTPUT_MASK,      //PTB9
                    PAL_MODE_OUTPUT_MASK,       //PTB10
                    PAL_MODE_ALTERNATIVE_2_MASK,       //PTB11
                    PAL_MODE_UNCONNECTED_MASK,      //PTB12
                    PAL_MODE_UNCONNECTED_MASK,      //PTB13
                    PAL_MODE_UNCONNECTED_MASK,      //PTB14
                    PAL_MODE_UNCONNECTED_MASK,      //PTB15
                    PAL_MODE_ALTERNATIVE_2_MASK | PAL_MODE_OUTPUT_OPENDRAIN_MASK,    //PTB16
                    PAL_MODE_ALTERNATIVE_2_MASK,    //PTB17
                    PAL_MODE_OUTPUT_MASK,       //PTB18
                    PAL_MODE_OUTPUT_MASK,       //PTB19
                    PAL_MODE_OUTPUT_MASK,       //PTB20
                    PAL_MODE_ALTERNATIVE_2_MASK,       //PTB21
                    PAL_MODE_ALTERNATIVE_2_MASK,       //PTB22
                    PAL_MODE_ALTERNATIVE_2_MASK,       //PTB23
                    PAL_MODE_UNCONNECTED_MASK,      //PTB24
                    PAL_MODE_UNCONNECTED_MASK,      //PTB25
                    PAL_MODE_UNCONNECTED_MASK,      //PTB26
                    PAL_MODE_UNCONNECTED_MASK,      //PTB27
                    PAL_MODE_UNCONNECTED_MASK,      //PTB28
                    PAL_MODE_UNCONNECTED_MASK,      //PTB29
                    PAL_MODE_UNCONNECTED_MASK,      //PTB30
                    PAL_MODE_UNCONNECTED_MASK,      //PTB31
                },
            },
            {
            /*
             * PORTC setup.
             */
                .port = IOPORT3,
                .pads = {
                    PAL_MODE_OUTPUT_MASK,       //PTC0
                    PAL_MODE_OUTPUT_MASK,       //PTC1
                    PAL_MODE_OUTPUT_MASK,       //PTC2
                    PAL_MODE_OUTPUT_MASK,       //PTC3
                    PAL_MODE_OUTPUT_MASK,       //PTC4
                    PAL_MODE_OUTPUT_MASK,       //PTC5
                    PAL_MODE_OUTPUT_MASK,       //PTC6
                    PAL_MODE_OUTPUT_MASK,       //PTC7
                    PAL_MODE_OUTPUT_MASK,       //PTC8
                    PAL_MODE_OUTPUT_MASK,       //PTC9
                    PAL_MODE_OUTPUT_OPENDRAIN_MASK | PAL_MODE_ALTERNATIVE_2_MASK,       //PTC10 I2C1_SCL
                    PAL_MODE_OUTPUT_OPENDRAIN_MASK | PAL_MODE_ALTERNATIVE_2_MASK,       //PTC11 I2C1_SDA
                    PAL_MODE_UNCONNECTED_MASK,      //PTC12
                    PAL_MODE_UNCONNECTED_MASK,      //PTC13
                    PAL_MODE_UNCONNECTED_MASK,      //PTC14
                    PAL_MODE_UNCONNECTED_MASK,      //PTC15
                    PAL_MODE_UNCONNECTED_MASK,      //PTC16
                    PAL_MODE_UNCONNECTED_MASK,      //PTC17
                    PAL_MODE_UNCONNECTED_MASK,      //PTC18
                    PAL_MODE_UNCONNECTED_MASK,      //PTC19
                    PAL_MODE_UNCONNECTED_MASK,      //PTC20
                    PAL_MODE_UNCONNECTED_MASK,      //PTC21
                    PAL_MODE_UNCONNECTED_MASK,      //PTC22
                    PAL_MODE_UNCONNECTED_MASK,      //PTC23
                    PAL_MODE_UNCONNECTED_MASK,      //PTC24
                    PAL_MODE_UNCONNECTED_MASK,      //PTC25
                    PAL_MODE_UNCONNECTED_MASK,      //PTC26
                    PAL_MODE_UNCONNECTED_MASK,      //PTC27
                    PAL_MODE_UNCONNECTED_MASK,      //PTC28
                    PAL_MODE_UNCONNECTED_MASK,      //PTC29
                    PAL_MODE_UNCONNECTED_MASK,      //PTC30
                    PAL_MODE_UNCONNECTED_MASK,      //PTC31
                },
            },
            {
            /*
             * PORTD setup.
             */

                .port = IOPORT4,
                .pads = {
                    PAL_MODE_OUTPUT_MASK,           //PTD0
                    PAL_MODE_OUTPUT_MASK,           //PTD1
                    PAL_MODE_OUTPUT_MASK,           //PTD2
                    PAL_MODE_OUTPUT_MASK,           //PTD3
                    PAL_MODE_OUTPUT_MASK,           //PTD4
                    PAL_MODE_OUTPUT_MASK,           //PTD5
                    PAL_MODE_OUTPUT_MASK,           //PTD6
                    PAL_MODE_OUTPUT_MASK,           //PTD7
                    PAL_MODE_ALTERNATIVE_5_MASK,        //PTD8 LPUART0_Rx
                    PAL_MODE_ALTERNATIVE_5_MASK,        //PTD9 LPUART0_Tx
                    PAL_MODE_UNCONNECTED_MASK,          //PTD10
                    PAL_MODE_OUTPUT_MASK,           //PTD11
                    PAL_MODE_OUTPUT_MASK,           //PTD12
                    PAL_MODE_OUTPUT_MASK,           //PTD13
                    PAL_MODE_OUTPUT_MASK,           //PTD14
                    PAL_MODE_OUTPUT_MASK,           //PTD15
                    PAL_MODE_UNCONNECTED_MASK,          //PTD16
                    PAL_MODE_UNCONNECTED_MASK,          //PTD17
                    PAL_MODE_UNCONNECTED_MASK,          //PTD18
                    PAL_MODE_UNCONNECTED_MASK,          //PTD19
                    PAL_MODE_UNCONNECTED_MASK,          //PTD20
                    PAL_MODE_UNCONNECTED_MASK,          //PTD21
                    PAL_MODE_UNCONNECTED_MASK,          //PTD22
                    PAL_MODE_UNCONNECTED_MASK,          //PTD23
                    PAL_MODE_UNCONNECTED_MASK,          //PTD24
                    PAL_MODE_UNCONNECTED_MASK,          //PTD25
                    PAL_MODE_UNCONNECTED_MASK,          //PTD26
                    PAL_MODE_UNCONNECTED_MASK,          //PTD27
                    PAL_MODE_UNCONNECTED_MASK,          //PTD28
                    PAL_MODE_UNCONNECTED_MASK,          //PTD29
                    PAL_MODE_UNCONNECTED_MASK,          //PTD30
                    PAL_MODE_UNCONNECTED_MASK,          //PTD31
                },
            },
            {
                /*
             * PORTE setup.
             *
             * PTE0 - SDHC
             * PTE1 - SDHC
             * PTE2 - SDHC
             * PTE3 - SDHC
             * PTE4 - SDHC
             * PTE5 - SDHC
             * PTE6 - USB OTG power switch
             * PTE10 - PIN56
             * PTE11 - PIN57
             * PTE24 - PIN33
             * PTE25 - PIN34
             * PTE26 - PIN24
             */

                .port = IOPORT5,
                .pads = {
                    PAL_MODE_ALTERNATIVE_4_MASK,            //PTE0  SDHC
                    PAL_MODE_ALTERNATIVE_4_MASK,            //PTE1  SDHC
                    PAL_MODE_ALTERNATIVE_4_MASK,            //PTE2  SDHC
                    PAL_MODE_ALTERNATIVE_4_MASK,            //PTE3  SDHC
                    PAL_MODE_ALTERNATIVE_4_MASK,            //PTE4  SDHC
                    PAL_MODE_ALTERNATIVE_4_MASK,            //PTE5  SDHC
                    PAL_MODE_OUTPUT_MASK,           //PTE6  USB OTG power switch
                    PAL_MODE_UNCONNECTED_MASK,          //PTE7
                    PAL_MODE_UNCONNECTED_MASK,          //PTE8
                    PAL_MODE_UNCONNECTED_MASK,          //PTE9
                    PAL_MODE_OUTPUT_MASK,           //PTE10
                    PAL_MODE_OUTPUT_MASK,           //PTE11
                    PAL_MODE_UNCONNECTED_MASK,          //PTE12
                    PAL_MODE_UNCONNECTED_MASK,          //PTE13
                    PAL_MODE_UNCONNECTED_MASK,          //PTE14
                    PAL_MODE_UNCONNECTED_MASK,          //PTE15
                    PAL_MODE_UNCONNECTED_MASK,          //PTE16
                    PAL_MODE_UNCONNECTED_MASK,          //PTE17
                    PAL_MODE_UNCONNECTED_MASK,          //PTE18
                    PAL_MODE_UNCONNECTED_MASK,          //PTE19
                    PAL_MODE_UNCONNECTED_MASK,          //PTE20
                    PAL_MODE_UNCONNECTED_MASK,          //PTE21
                    PAL_MODE_UNCONNECTED_MASK,          //PTE22
                    PAL_MODE_UNCONNECTED_MASK,          //PTE23
                    PAL_MODE_OUTPUT_MASK,           //PTE24
                    PAL_MODE_OUTPUT_MASK,           //PTE25
                    PAL_MODE_OUTPUT_MASK,           //PTE26
                    PAL_MODE_UNCONNECTED_MASK,          //PTE27
                    PAL_MODE_UNCONNECTED_MASK,          //PTE28
                    PAL_MODE_UNCONNECTED_MASK,          //PTE29
                    PAL_MODE_UNCONNECTED_MASK,          //PTE30
                    PAL_MODE_UNCONNECTED_MASK,          //PTE31
                },
            },
        },
};
#endif

/**
 * @brief   Early initialization code.
 * @details This initialization must be performed just after stack setup
 *          and before any other initialization.
 */
void __early_init(void) {
  SystemInit();
}

/**
 * @brief   Board-specific initialization code.
 * @todo    Add your board-specific code, if any.
 */
void boardInit(void) {
    kinetis_clock_init();
}
