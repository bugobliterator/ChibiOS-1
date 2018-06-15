# List of all platform files.
PLATFORMSRC = ${CHIBIOS}/os/hal/ports/common/ARMCMx/nvic.c \
              ${CHIBIOS}/os/hal/ports/KINETIS_MCUXPresso/MK66F18/hal_lld.c \
              ${CHIBIOS}/os/hal/ports/KINETIS_MCUXPresso/MK66F18/hal_pal_lld.c \
              ${CHIBIOS}/os/hal/ports/KINETIS_MCUXPresso/MK66F18/hal_st_lld.c \
              ${CHIBIOS}/os/hal/ports/KINETIS_MCUXPresso/MK66F18/hal_serial_lld.c \
              ${CHIBIOS}/os/hal/ports/KINETIS_MCUXPresso/MK66F18/SDK/system_MK66F18.c \
              ${CHIBIOS}/os/hal/ports/KINETIS_MCUXPresso/MK66F18/SDK/drivers/fsl_clock.c \
              ${CHIBIOS}/os/hal/ports/KINETIS_MCUXPresso/MK66F18/SDK/drivers/fsl_gpio.c \
              ${CHIBIOS}/os/hal/ports/KINETIS_MCUXPresso/MK66F18/SDK/drivers/fsl_smc.c \
              ${CHIBIOS}/os/hal/ports/KINETIS_MCUXPresso/MK66F18/SDK/drivers/fsl_pit.c \
              ${CHIBIOS}/os/hal/ports/KINETIS_MCUXPresso/MK66F18/SDK/drivers/fsl_tpm.c \
              ${CHIBIOS}/os/hal/ports/KINETIS_MCUXPresso/MK66F18/SDK/drivers/fsl_lpuart.c \
              ${CHIBIOS}/os/hal/ports/KINETIS_MCUXPresso/MK66F18/SDK/drivers/fsl_uart.c


# Required include directories
PLATFORMINC = ${CHIBIOS}/os/hal/ports/common/ARMCMx \
              ${CHIBIOS}/os/hal/ports/KINETIS_MCUXPresso/MK66F18/SDK \
              ${CHIBIOS}/os/hal/ports/KINETIS_MCUXPresso/MK66F18/SDK/drivers \
              ${CHIBIOS}/os/hal/ports/KINETIS_MCUXPresso/MK66F18
