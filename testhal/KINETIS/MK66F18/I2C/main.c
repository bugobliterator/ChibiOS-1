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

/* buffers depth */
#define BARO_RX_DEPTH 6
#define BARO_TX_DEPTH 4

#define MPL3115A2_ADDRESS        0x60
#define MPL3115A2_REG_WHO_AM_I   0x0c
#define MPL3115A2_WHO_AM_I       0xC4

#define OUT_P_MSB                0x01

#define MPL3115A2_CTRL_REG1      0x26
#  define CTRL_REG1_ALT          (1 << 7)
#  define CTRL_REG1_RAW          (1 << 6)
#  define CTRL_REG1_OS_SHIFTS    (3)
#  define CTRL_REG1_OS_MASK      (0x7 << CTRL_REG1_OS_SHIFTS)
#  define CTRL_REG1_OS(n)        (((n)& 0x7) << CTRL_REG1_OS_SHIFTS)
#  define CTRL_REG1_RST          (1 << 2)
#  define CTRL_REG1_OST          (1 << 1)
#  define CTRL_REG1_SBYB         (1 << 0)

typedef struct __attribute__((__packed__)) MPL3115A2_data_t {
	union {
		uint32_t q;
		uint16_t w[2];
		uint8_t  b[4];
	} pressure;

	union {
		uint16_t w;
		uint8_t  b[2];
	} temperature;
} MPL3115A2_data_t;

static uint8_t rxbuf[BARO_RX_DEPTH];
static uint8_t txbuf[BARO_TX_DEPTH];
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

const I2CConfig i2ccfg = {
    .enableMaster = true, /*!< Enables the I2C peripheral at initialization time. */
#if defined(FSL_FEATURE_I2C_HAS_STOP_HOLD_OFF) && FSL_FEATURE_I2C_HAS_STOP_HOLD_OFF
    .enableStopHold = false,  /*!< Controls the stop hold enable. */
#endif
#if defined(FSL_FEATURE_I2C_HAS_DOUBLE_BUFFER_ENABLE) && FSL_FEATURE_I2C_HAS_DOUBLE_BUFFER_ENABLE
    .enableDoubleBuffering = false, /*!< Controls double buffer enable; notice that
                                     enabling the double buffer disables the clock stretch. */
#endif
    .baudRate_Bps = 100000,     /*!< Baud rate configuration of I2C peripheral. */
    .glitchFilterWidth = 0, /*!< Controls the width of the glitch. */
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
  systime_t tmo = MS2ST(4);
  msg_t status;
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
  i2cStart(&I2CD1, &i2ccfg);
  /*
   * Creates the blinker thread.
   */
  chThdCreateStatic(waThread1, sizeof(waThread1), NORMALPRIO, Thread1, NULL);
  sdStart(&LPSD1, NULL);
  /**
   * Prepares the accelerometer
   */
  txbuf[0] = MPL3115A2_REG_WHO_AM_I; /* register address */

  MPL3115A2_data_t reading;
  /* read the most recent measurement
      * 3 Pressure and 2 temprtture
      */
  uint8_t	b[3 + 2];
  uint8_t reg;
  float temp, press;

  txbuf[0] = 0x26;
  txbuf[1] = 0xB8;
  i2cAcquireBus(&I2CD1);
  status = i2cMasterTransmitTimeout(&I2CD1, MPL3115A2_ADDRESS, &txbuf[0], 2, NULL, 0, tmo);
  i2cReleaseBus(&I2CD1);

  txbuf[0] = 0x13;
  txbuf[1] = 0x07;
  i2cAcquireBus(&I2CD1);
  status = i2cMasterTransmitTimeout(&I2CD1, MPL3115A2_ADDRESS, &txbuf[0], 2, NULL, 0, tmo);
  i2cReleaseBus(&I2CD1);

  txbuf[0] = 0x26;
  txbuf[1] = 0xB9;
  i2cAcquireBus(&I2CD1);
  status = i2cMasterTransmitTimeout(&I2CD1, MPL3115A2_ADDRESS, &txbuf[0], 2, NULL, 0, tmo);
  i2cReleaseBus(&I2CD1);
  chThdSleepMilliseconds(500);

  while (true) {
    reg = 0x00;
    i2cAcquireBus(&I2CD1);
    status = i2cMasterTransmitTimeout(&I2CD1, MPL3115A2_ADDRESS, &reg, 1, rxbuf, 1, tmo);
    i2cReleaseBus(&I2CD1);

    chThdSleepMilliseconds(500);

    reg = 0x01;
    i2cAcquireBus(&I2CD1);
    status = i2cMasterTransmitTimeout(&I2CD1, MPL3115A2_ADDRESS, &reg, 1, &b[0], sizeof(b), tmo);
    i2cReleaseBus(&I2CD1);
    reading.pressure.q = ((uint32_t)b[0]) << 18 | ((uint32_t) b[1]) <<  10 | (((uint32_t)b[2]) & 0xc0) << 2 | ((
								          b[2] & 0x30) >> 4);
    reading.temperature.w = ((uint16_t) b[3]) << 8 | (b[4] >> 4);

    temp = (float) reading.temperature.b[1] + ((float)(reading.temperature.b[0]) / 16.0f);

    press = (float)(reading.pressure.q >> 8) + ((float)(reading.pressure.b[0]) / 4.0f);

    chprintf((BaseSequentialStream*)&LPSD1, "Pressure: %f Temp: %f\n", press, temp);
    chThdSleepMilliseconds(500);
  }

  return 0;
}
