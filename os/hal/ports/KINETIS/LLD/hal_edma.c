
#include "hal.h"

/**
 * @brief   DMA channels descriptors.
 * @details This table keeps the association between an unique channel
 *          identifier and the involved physical registers.
 * @note    Don't use this array directly, use the appropriate wrapper macros
 *          instead.
 */
const kinetis_dma_channel_t _kinetis_dma_channels[KINETIS_DMA_CHANNELS] = {
  {&DMA->TCD[0], &DMA->DCHPRI0, 0, KINETIS_DMA0_IRQ_VECTOR},
  {&DMA->TCD[1], &DMA->DCHPRI1, 1, KINETIS_DMA1_IRQ_VECTOR},
  {&DMA->TCD[2], &DMA->DCHPRI2, 2, KINETIS_DMA2_IRQ_VECTOR},
  {&DMA->TCD[3], &DMA->DCHPRI3, 3, KINETIS_DMA3_IRQ_VECTOR},
  {&DMA->TCD[4], &DMA->DCHPRI4, 4, KINETIS_DMA4_IRQ_VECTOR},
  {&DMA->TCD[5], &DMA->DCHPRI5, 5, KINETIS_DMA5_IRQ_VECTOR},
  {&DMA->TCD[6], &DMA->DCHPRI6, 6, KINETIS_DMA6_IRQ_VECTOR},
  {&DMA->TCD[7], &DMA->DCHPRI7, 7, KINETIS_DMA7_IRQ_VECTOR},
  {&DMA->TCD[8], &DMA->DCHPRI8, 8, KINETIS_DMA8_IRQ_VECTOR},
  {&DMA->TCD[9], &DMA->DCHPRI9, 9, KINETIS_DMA9_IRQ_VECTOR},
  {&DMA->TCD[10], &DMA->DCHPRI10, 10, KINETIS_DMA10_IRQ_VECTOR},
  {&DMA->TCD[11], &DMA->DCHPRI11, 11, KINETIS_DMA11_IRQ_VECTOR},
  {&DMA->TCD[12], &DMA->DCHPRI12, 12, KINETIS_DMA12_IRQ_VECTOR},
  {&DMA->TCD[13], &DMA->DCHPRI13, 13, KINETIS_DMA13_IRQ_VECTOR},
  {&DMA->TCD[14], &DMA->DCHPRI14, 14, KINETIS_DMA14_IRQ_VECTOR},
  {&DMA->TCD[15], &DMA->DCHPRI15, 15, KINETIS_DMA15_IRQ_VECTOR},
  {&DMA->TCD[16], &DMA->DCHPRI16, 16, KINETIS_DMA0_IRQ_VECTOR},
  {&DMA->TCD[17], &DMA->DCHPRI17, 17, KINETIS_DMA1_IRQ_VECTOR},
  {&DMA->TCD[18], &DMA->DCHPRI18, 18, KINETIS_DMA2_IRQ_VECTOR},
  {&DMA->TCD[19], &DMA->DCHPRI19, 19, KINETIS_DMA3_IRQ_VECTOR},
  {&DMA->TCD[20], &DMA->DCHPRI20, 20, KINETIS_DMA4_IRQ_VECTOR},
  {&DMA->TCD[21], &DMA->DCHPRI21, 21, KINETIS_DMA5_IRQ_VECTOR},
  {&DMA->TCD[22], &DMA->DCHPRI22, 22, KINETIS_DMA6_IRQ_VECTOR},
  {&DMA->TCD[23], &DMA->DCHPRI23, 23, KINETIS_DMA7_IRQ_VECTOR},
  {&DMA->TCD[24], &DMA->DCHPRI24, 24, KINETIS_DMA8_IRQ_VECTOR},
  {&DMA->TCD[25], &DMA->DCHPRI25, 25, KINETIS_DMA9_IRQ_VECTOR},
  {&DMA->TCD[26], &DMA->DCHPRI26, 26, KINETIS_DMA10_IRQ_VECTOR},
  {&DMA->TCD[27], &DMA->DCHPRI27, 27, KINETIS_DMA11_IRQ_VECTOR},
  {&DMA->TCD[28], &DMA->DCHPRI28, 28, KINETIS_DMA12_IRQ_VECTOR},
  {&DMA->TCD[29], &DMA->DCHPRI29, 29, KINETIS_DMA13_IRQ_VECTOR},
  {&DMA->TCD[30], &DMA->DCHPRI30, 30, KINETIS_DMA14_IRQ_VECTOR},
  {&DMA->TCD[31], &DMA->DCHPRI31, 31, KINETIS_DMA15_IRQ_VECTOR}
};

#define DMA_IRQ_HANDLER(DMA_ID1, DMA_ID2) \
OSAL_IRQ_HANDLER(KINETIS_DMA##DMA_ID1##_IRQ_VECTOR) { \
  OSAL_IRQ_PROLOGUE(); \
  if (DMA->INT & DMA_INT_INT##DMA_ID1##_MASK){ \
    DMA->INT |= DMA_INT_INT##DMA_ID1##_MASK; \
    if (dma_isr_redir[##DMA_ID1##].dma_func) \
        dma_isr_redir[##DMA_ID1##].dma_func(dma_isr_redir[##DMA_ID1##].dma_param); \
  } \
  if (DMA->INT & DMA_INT_INT##DMA_ID2##_MASK){ \
    DMA->INT |= DMA_INT_INT##DMA_ID2##_MASK; \
    if (dma_isr_redir[##DMA_ID2##].dma_func) \
        dma_isr_redir[##DMA_ID2##].dma_func(dma_isr_redir[##DMA_ID2##].dma_param); \
  } \
  OSAL_IRQ_EPILOGUE(); \
} \

DMA_IRQ_HANDLER(0, 16)
DMA_IRQ_HANDLER(1, 17)
DMA_IRQ_HANDLER(2, 18)
DMA_IRQ_HANDLER(3, 19)
DMA_IRQ_HANDLER(4, 20)
DMA_IRQ_HANDLER(5, 21)
DMA_IRQ_HANDLER(6, 22)
DMA_IRQ_HANDLER(7, 23)
DMA_IRQ_HANDLER(8, 24)
DMA_IRQ_HANDLER(9, 25)
DMA_IRQ_HANDLER(10, 26)
DMA_IRQ_HANDLER(11, 27)
DMA_IRQ_HANDLER(12, 28)
DMA_IRQ_HANDLER(13, 29)
DMA_IRQ_HANDLER(14, 30)
DMA_IRQ_HANDLER(15, 31)

/**
 * @brief   DMA ISR redirector type.
 */
typedef struct {
  kinetis_dmaisr_t        dma_func;       /**< @brief DMA callback function.  */
  void                  *dma_param;     /**< @brief DMA callback parameter. */
} dma_isr_redir_t;

/**
 * @brief   Mask of the allocated streams.
 */
static uint32_t dma_channels_mask;

/**
 * @brief   DMA IRQ redirectors.
 */
static dma_isr_redir_t dma_isr_redir[KINETIS_DMA_CHANNELS];

void dmaInit(void)
{
    for (i = 0U; i < KINETIS_DMA_STREAMS; i++) {
        DMAMUX->CHCFG[i] = 0x00;
        dma_isr_redir[i].dma_func = NULL;
    }
    DMA->INT = 0;
    DMA->ERR = 0x0F;
}

/**
 * @brief   Allocates a DMA stream.
 * @details The stream is allocated and, if required, the DMA clock enabled.
 *          The function also enables the IRQ vector associated to the stream
 *          and initializes its priority.
 * @pre     The stream must not be already in use or an error is returned.
 * @post    The stream is allocated and the default ISR handler redirected
 *          to the specified function.
 * @post    The stream ISR vector is enabled and its priority configured.
 * @post    The stream must be freed using @p dmaStreamRelease() before it can
 *          be reused with another peripheral.
 * @post    The stream is in its post-reset state.
 * @note    This function can be invoked in both ISR or thread context.
 *
 * @param[in] dmastp    pointer to a stm32_dma_stream_t structure
 * @param[in] priority  IRQ priority mask for the DMA stream
 * @param[in] func      handling function pointer, can be @p NULL
 * @param[in] param     a parameter to be passed to the handling function
 * @return              The operation status.
 * @retval false        no error, stream taken.
 * @retval true         error, stream already taken.
 *
 * @special
*/
bool dmaChannelAllocate(const kinetis_edma_channel_t *dmach,
                        uint32_t priority,
                        kinetis_edmaisr_t func,
                        void *param) {

  osalDbgCheck(dmach != NULL);

  /* Checks if the stream is already taken.*/
  if ((dma_channels_mask & (1U << dmach->selfindex)) != 0U)
    return true;

  /* Marks the stream as allocated.*/
  dma_isr_redir[dmach->selfindex].dma_func  = func;
  dma_isr_redir[dmach->selfindex].dma_param = param;
  dma_channels_mask |= (1U << dmach->selfindex);

  /* Enabling DMA clocks.*/
  SIM->SCGC6 |= SIM_SCGC6_DMAMUX;
  SIM->SCGC7 |= SIM_SCGC7_DMA;

  /* Putting the stream in a safe state.*/
  dmaStreamDisable(dmach);
  dmach->channel->CSR = 0;

  /* Enables the associated IRQ vector if a callback is defined.*/
  if (func != NULL) {
    DMA->INT |= (1 << (dmach)->selfindex);
    nvicEnableVector(dmach->vector, priority);
  }

  return false;
}

/**
 * @brief   Releases a DMA stream.
 * @details The stream is freed and, if required, the DMA clock disabled.
 *          Trying to release a unallocated stream is an illegal operation
 *          and is trapped if assertions are enabled.
 * @pre     The stream must have been allocated using @p dmaStreamAllocate().
 * @post    The stream is again available.
 * @note    This function can be invoked in both ISR or thread context.
 *
 * @param[in] dmastp    pointer to a stm32_dma_stream_t structure
 *
 * @special
 */
void dmaChannelRelease(const stm32_dma_stream_t *dmach) {

  osalDbgCheck(dmach != NULL);

  /* Check if the streams is not taken.*/
  osalDbgAssert((dma_channels_mask & (1U << dmach->selfindex)) != 0U,
                "not allocated");

  /* Disables the associated IRQ vector if unused.*/
  if (!(dma_channels_mask & (1U << DMA_INT_SHARED_WITH(dmach->selfindex)))) {
    nvicDisableVector(dmach->vector);
  }

  /* Marks the stream as not allocated.*/
  dma_channels_mask &= ~(1U << dmach->selfindex);

  /* Shutting down clocks that are no more required, if any.*/
  if ((dma_channels_mask) == 0U) {
    SIM->SCGC6 &= ~SIM_SCGC6_DMAMUX;
    SIM->SCGC7 &= ~SIM_SCGC7_DMA;
  }
}