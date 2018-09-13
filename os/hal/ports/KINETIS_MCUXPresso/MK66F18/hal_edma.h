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
  edma_handle_t         *edma_handle;        /**< @brief Associated EDMA Handle.  */
  uint8_t               selfindex;          /**< @brief Index to self in array. */
  uint8_t               vector;
} kinetis_edma_channel_t;

#define DMAx_IRQn(n)    (DMA0_IRQn + n)
#define DMA_INT_SHARED_WITH(n) ((n>15)?(n-16):(n+16))


#define KINETIS_DMA_CHANNEL(id)  (&_kinetis_dma_channels[id])

/*===========================================================================*/
/* Driver macros.                                                            */
/*===========================================================================*/

/** @} */

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif
  void dmaInit(void);
  bool dmaChannelAllocate(edma_handle_t *edma_handle,
                          uint32_t req_src,
                          uint32_t priority,
                          edma_callback func,
                          void *param);
  void dmaChannelRelease(const edma_handle_t *edma_handle);
#ifdef __cplusplus
}
#endif

#endif /* KINETIS_DMA_H */