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

#define FXAS21002C_WHO_AM_I             0x0c
#define WHO_AM_I                        0xd7
#define DIR_READ(a)                     ((a) | (1 << 7))
#define DIR_WRITE(a)                    ((a) & 0x7f)
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

const SPIConfig spicfg = {
    .master = {
        .whichCtar = kDSPI_Ctar0,      /*!< The desired CTAR to use. */
        .ctarConfig = {
            .baudRate = 1*1000*1000,
            .bitsPerFrame = 8,
            .cpol = kDSPI_ClockPolarityActiveHigh,
            .cpha = kDSPI_ClockPhaseFirstEdge,
            .direction = kDSPI_MsbFirst,
            .pcsToSckDelayInNanoSec = 1000000000U / (2*1000*1000),
            .lastSckToPcsDelayInNanoSec = 1000000000U / (2*1000*1000),
            .betweenTransferDelayInNanoSec = 1000000000U / (2*1000*1000),
        }, /*!< Set the ctarConfig to the desired CTAR. */

        .whichPcs = kDSPI_Pcs0,                     /*!< The desired Peripheral Chip Select (pcs). */
        .pcsActiveHighOrLow = kDSPI_PcsActiveLow, /*!< The desired PCS active high or low. */

        .enableContinuousSCK = false,   /*!< CONT_SCKE, continuous SCK enable. Note that the continuous SCK is only
                                        supported for CPHA = 1.*/
        .enableRxFifoOverWrite = false, /*!< ROOE, receive FIFO overflow overwrite enable. If ROOE = 0, the incoming
                                        data is ignored and the data from the transfer that generated the overflow
                                        is also ignored. If ROOE = 1, the incoming data is shifted to the
                                        shift register. */

        .enableModifiedTimingFormat = false,        /*!< Enables a modified transfer format to be used if true.*/
        .samplePoint = kDSPI_SckToSin0Clock /*!< Controls when the module master samples SIN in the Modified Transfer
                                                    Format. It's valid only when CPHA=0. */
    },
    .ssport = IOPORT2,
    .sspad = 9,
    .end_cb = NULL,
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


static void read_reg(SPIDriver *spip, uint8_t reg, uint8_t n, uint8_t* b) {
    uint8_t cmd;
    cmd = DIR_READ(reg);
    spiSelect(spip);
    spiSend(spip, 1, &cmd);
    spiReceive(spip, n, b);
    spiUnselect(spip);
}

static uint8_t read_reg8(SPIDriver *spip, uint8_t reg) {
    uint8_t cmd[2], b[0] = {0};
    cmd[0] = DIR_READ(reg);
    cmd[1] = 0;
    spiSelect(spip);
    spiExchange(spip, 2, cmd, b);
    // spiSend(spip, 1, &cmd);
    // spiReceive(spip, 1, &b);
    spiUnselect(spip);
    return b[1];
}

static void write_reg8(SPIDriver *spip, uint8_t reg, uint8_t b) {
    uint8_t cmd;
    cmd = DIR_WRITE(reg);
    spiSelect(spip);
    spiSend(spip, 1, &cmd);
    spiSend(spip, 1, &b);
    spiUnselect(spip);
}


static void modify_reg8(SPIDriver *spip, uint8_t reg, uint8_t clearbits, uint8_t setbits) {
    uint8_t val;
    val = read_reg8(spip, reg);
    val &= ~clearbits;
    val |= setbits;
    write_reg8(spip, reg, val);
}


/*
 * Application entry point.
 */
int main(void) {
  systime_t tmo = MS2ST(4);
  msg_t status;
  float x,y,z;
  uint8_t val, cnt = 0;
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
   * Starts I2C
   */
  /*
   * Creates the blinker thread.
   */
  chThdCreateStatic(waThread1, sizeof(waThread1), NORMALPRIO, Thread1, NULL);
  palSetPad(IOPORT2, 8);
  chThdSleepMilliseconds(2);
  palClearPad(IOPORT2, 8);

  palClearPad(IOPORT4, 12);
  chThdSleepMilliseconds(2);
  palSetPad(IOPORT4, 12);

  palClearPad(IOPORT1, 25);
  chThdSleepMilliseconds(2);
  palSetPad(IOPORT1, 25);
  palSetPad(IOPORT2, 10);
  palSetPadMode(IOPORT2, 16, PAL_MODE_ALTERNATIVE_2_MASK);
  sdStart(&LPSD1, NULL);
  spiStart(&SPID1, &spicfg);

  while (true) {
    val = read_reg8(&SPID1, FXAS21002C_WHO_AM_I);
    chprintf((BaseSequentialStream*)&LPSD1, "WHO_AM_I: 0x%X\n", val);
    //chprintf((BaseSequentialStream*)&LPSD1, "x: %f y: %f z: %f\n", x, y, z);
    chThdSleepMilliseconds(100);
  }

  return 0;
}
