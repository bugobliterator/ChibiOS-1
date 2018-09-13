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

#ifndef _MCUCONF_H_
#define _MCUCONF_H_

#define CPU_MK66FN2M0VLQ18
#define KINETIS_MCUCONF
/*
 * HAL driver system settings.
 */
#define KINETIS_SERIAL_USE_LPUART0 TRUE

#define KINETIS_SPI_USE_SPI1 TRUE
#define KINETIS_SPI_SPI1_IRQ_PRIORITY 10
#define KINETIS_SPI_USE_SPI2 TRUE
#define KINETIS_SPI_SPI2_IRQ_PRIORITY 10
#endif /* _MCUCONF_H_ */
