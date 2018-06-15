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

/**
 * @brief   K66 eDMA stream descriptor structure.
 */
typedef struct {
  DMA_TCD_TypeDef       *channel;        /**< @brief Associated TCD Channel.  */
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


#define dmaChannelSetSource(dmach, source_id) {                              \
  DMAMUX->CHCFG[dmach->selfindex]  =  DMAMUX_CHCFGn_ENBL |                   \
                                      DMAMUX_CHCFGn_SOURCE(source_id);       \
}

#define dmaChannelSetSourceAddr(dmach, addr, off) {                                 \
  (dmach)->channel->SADDR  = (uint32_t)(addr);                               \
  (dmach)->channel->SOFF  = (uint32_t)(off);                               \
}

#define dmaChannelSetSourceAddrAdj(dmach, addr_adj) {                                 \
  (dmach)->channel->SLAST  = (uint32_t)(addr_adj);                               \
}

#define dmaChannelSetDestinyAddr(dmach, addr, off) {                                 \
  (dmach)->channel->DADDR  = (uint32_t)(addr);                               \
  (dmach)->channel->DOFF  = (uint32_t)(off);                               \
}

#define dmaChannelSetDestinyAddrAdj(dmach, addr_adj) {                                 \
  (dmach)->channel->DLASTSGA  = (uint32_t)(addr_adj);                               \
}

#define dmaChannelSetNumBytesPerTransaction(dmach, nbytes) {                         \
  (dmach)->channel->NBYTES_MLNO  = (uint32_t)(nbytes);                               \
}
//TODO: Add Support for ELINK
#define dmaChannelSetTransactionSize(dmach, size) {                         \
  (dmach)->channel->BITER_ELINKNO = size;                                   \
  (dmach)->channel->CITER_ELINKNO = size;                                   \
}

#define dmaChannelGetTransactionSize(dmach) (0x7FFFU & (dmach)->channel->CITER_ELINKNO)

#define dmaChannelSetAttr(dmach, attr) {                                    \
  (dmach)->channel->ATTR  = (uint32_t)(attr);                                 \
}

#define dmaChannelEnable(dmach) {                                           \
  DMA->SERQ = (dmach)->selfindex;                                              \
}

#define dmaChannelDisable(dmach) {                                          \
  DMA->CERQ = (dmach)->selfindex;                                              \
  while ((DMA->ERQ & (1 << (dmach)->selfindex)) != 0)                     \
    ;                                                                       \
  dmaChannelClearInterrupt(dmach);                                          \
}

#define dmaChannelClearInterrupt(dmach) {                                   \
      DMA->INT &= ~(1 << (dmach)->selfindex);                 \
}

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
  bool dmaChannelAllocate(const kinetis_edma_channel_t *dmastp,
                         uint32_t priority,
                         kinetis_edmaisr_t func,
                         void *param);
  void dmaChannelRelease(const kinetis_edma_channel_t *dmastp);
#ifdef __cplusplus
}
#endif

#endif /* KINETIS_DMA_H */