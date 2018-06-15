/*
    ChibiOS - Copyright (C) 2006..2016 Giovanni Di Sirio

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

/*===========================================================================*/
/* Driver data structures and types.                                         */
/*===========================================================================*/

#ifndef KINETIS_DMA_H
#define KINETIS_DMA_H

#define KINETIS_DMA_CHANNELS 32
#include "fsl_edma.h"
#include "fsl_dmamux.h"

/**
 * @brief   K66 eDMA stream descriptor structure.
 */
typedef struct {
  edma_tcd_t            *channel;        /**< @brief Associated TCD Channel.  */
  uint8_t               *dchpri;         /**< @brief Associated DCHPRI.       */
  uint8_t               selfindex;       /**< @brief Index to self in array. */
} kinetis_edma_channel_t;

#define DMAx_IRQn(n)    (DMA0_IRQn + n)
#define DMA_INT_SHARED_WITH(n) ((n>15)?(n-16):(n+16))

/**
 * @brief   Kinetis eDMA ISR function type.
 *
 * @param[in] p         parameter for the registered function
 */
typedef void (*kinetis_edmaisr_t)(void *p);

#define KINETIS_DMA_CHANNEL(id)  (&_kinetis_dma_channels[id])

/*===========================================================================*/
/* Driver macros.                                                            */
/*===========================================================================*/

/** @} */

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

#if !defined(__DOXYGEN__)
extern const kinetis_edma_channel_t _kinetis_dma_channels[KINETIS_DMA_CHANNELS];
#endif

#ifdef __cplusplus
extern "C" {
#endif
  void dmaInit(void);
  bool dmaChannelAllocate(const kinetis_edma_channel_t *dmach,
                        uint32_t priority,
                        kinetis_edmaisr_t func,
                        void *param);
  void dmaChannelRelease(const stm32_dma_stream_t *dmach);
#ifdef __cplusplus
}
#endif

#endif /* KINETIS_DMA_H */