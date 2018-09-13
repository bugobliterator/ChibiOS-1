#include "osal.h"
#include "hal.h"

/**
 * @brief   DMA channels descriptors.
 * @details This table keeps the association between an unique channel
 *          identifier and the involved physical registers.
 * @note    Don't use this array directly, use the appropriate wrapper macros
 *          instead.
 */
kinetis_edma_channel_t _kinetis_dma_channels[FSL_FEATURE_EDMA_DMAMUX_CHANNELS] = {
  {NULL, 0,    DMA0_DMA16_IRQn},
  {NULL, 1,    DMA1_DMA17_IRQn},
  {NULL, 2,    DMA2_DMA18_IRQn},
  {NULL, 3,    DMA3_DMA19_IRQn},
  {NULL, 4,    DMA4_DMA20_IRQn},
  {NULL, 5,    DMA5_DMA21_IRQn},
  {NULL, 6,    DMA6_DMA22_IRQn},
  {NULL, 7,    DMA7_DMA23_IRQn},
  {NULL, 8,    DMA8_DMA24_IRQn},
  {NULL, 9,    DMA9_DMA25_IRQn},
  {NULL, 10,   DMA10_DMA26_IRQn},
  {NULL, 11,   DMA11_DMA27_IRQn},
  {NULL, 12,   DMA12_DMA28_IRQn},
  {NULL, 13,   DMA13_DMA29_IRQn},
  {NULL, 14,   DMA14_DMA30_IRQn},
  {NULL, 15,   DMA15_DMA31_IRQn},
  {NULL, 16,   DMA0_DMA16_IRQn},
  {NULL, 17,   DMA1_DMA17_IRQn},
  {NULL, 18,   DMA2_DMA18_IRQn},
  {NULL, 19,   DMA3_DMA19_IRQn},
  {NULL, 20,   DMA4_DMA20_IRQn},
  {NULL, 21,   DMA5_DMA21_IRQn},
  {NULL, 22,   DMA6_DMA22_IRQn},
  {NULL, 23,   DMA7_DMA23_IRQn},
  {NULL, 24,   DMA8_DMA24_IRQn},
  {NULL, 25,   DMA9_DMA25_IRQn},
  {NULL, 26,   DMA10_DMA26_IRQn},
  {NULL, 27,   DMA11_DMA27_IRQn},
  {NULL, 28,   DMA12_DMA28_IRQn},
  {NULL, 29,   DMA13_DMA29_IRQn},
  {NULL, 30,   DMA14_DMA30_IRQn},
  {NULL, 31,   DMA15_DMA31_IRQn}
};

OSAL_IRQ_HANDLER(KINETIS_DMA0_IRQ_VECTOR) {
  OSAL_IRQ_PROLOGUE();
  if ((EDMA_GetChannelStatusFlags(DMA0, 0) & kEDMA_InterruptFlag) != 0U) {
    if (_kinetis_dma_channels[0].edma_handle != NULL) {
      EDMA_HandleIRQ(_kinetis_dma_channels[0].edma_handle);
    }
  }
  if ((EDMA_GetChannelStatusFlags(DMA0, 16) & kEDMA_InterruptFlag) != 0U) {
    if (_kinetis_dma_channels[16].edma_handle != NULL) {
      EDMA_HandleIRQ(_kinetis_dma_channels[16].edma_handle);
  }
  }
  OSAL_IRQ_EPILOGUE();
}

OSAL_IRQ_HANDLER(KINETIS_DMA1_IRQ_VECTOR) {
  OSAL_IRQ_PROLOGUE();
  if ((EDMA_GetChannelStatusFlags(DMA0, 1) & kEDMA_InterruptFlag) != 0U) {
    if (_kinetis_dma_channels[1].edma_handle != NULL) {
      EDMA_HandleIRQ(_kinetis_dma_channels[1].edma_handle);
    }
  }
  if ((EDMA_GetChannelStatusFlags(DMA0, 17) & kEDMA_InterruptFlag) != 0U) {
    if (_kinetis_dma_channels[17].edma_handle != NULL) {
      EDMA_HandleIRQ(_kinetis_dma_channels[17].edma_handle);
  }
  }
  OSAL_IRQ_EPILOGUE();
}

OSAL_IRQ_HANDLER(KINETIS_DMA2_IRQ_VECTOR) {
  OSAL_IRQ_PROLOGUE();
  if ((EDMA_GetChannelStatusFlags(DMA0, 2) & kEDMA_InterruptFlag) != 0U) {
    if (_kinetis_dma_channels[2].edma_handle != NULL) {
      EDMA_HandleIRQ(_kinetis_dma_channels[2].edma_handle);
    }
  }
  if ((EDMA_GetChannelStatusFlags(DMA0, 18) & kEDMA_InterruptFlag) != 0U) {
    if (_kinetis_dma_channels[18].edma_handle != NULL) {
      EDMA_HandleIRQ(_kinetis_dma_channels[18].edma_handle);
  }
  }
  OSAL_IRQ_EPILOGUE();
}

OSAL_IRQ_HANDLER(KINETIS_DMA3_IRQ_VECTOR) {
  OSAL_IRQ_PROLOGUE();
  if ((EDMA_GetChannelStatusFlags(DMA0, 3) & kEDMA_InterruptFlag) != 0U) {
    if (_kinetis_dma_channels[3].edma_handle != NULL) {
      EDMA_HandleIRQ(_kinetis_dma_channels[3].edma_handle);
    }
  }
  if ((EDMA_GetChannelStatusFlags(DMA0, 19) & kEDMA_InterruptFlag) != 0U) {
    if (_kinetis_dma_channels[19].edma_handle != NULL) {
      EDMA_HandleIRQ(_kinetis_dma_channels[19].edma_handle);
  }
  }
  OSAL_IRQ_EPILOGUE();
}

OSAL_IRQ_HANDLER(KINETIS_DMA4_IRQ_VECTOR) {
  OSAL_IRQ_PROLOGUE();
  if ((EDMA_GetChannelStatusFlags(DMA0, 4) & kEDMA_InterruptFlag) != 0U) {
    if (_kinetis_dma_channels[4].edma_handle != NULL) {
      EDMA_HandleIRQ(_kinetis_dma_channels[4].edma_handle);
    }
  }
  if ((EDMA_GetChannelStatusFlags(DMA0, 20) & kEDMA_InterruptFlag) != 0U) {
    if (_kinetis_dma_channels[20].edma_handle != NULL) {
      EDMA_HandleIRQ(_kinetis_dma_channels[20].edma_handle);
  }
  }
  OSAL_IRQ_EPILOGUE();
}

OSAL_IRQ_HANDLER(KINETIS_DMA5_IRQ_VECTOR) {
  OSAL_IRQ_PROLOGUE();
  if ((EDMA_GetChannelStatusFlags(DMA0, 5) & kEDMA_InterruptFlag) != 0U) {
    if (_kinetis_dma_channels[5].edma_handle != NULL) {
      EDMA_HandleIRQ(_kinetis_dma_channels[5].edma_handle);
    }
  }
  if ((EDMA_GetChannelStatusFlags(DMA0, 21) & kEDMA_InterruptFlag) != 0U) {
    if (_kinetis_dma_channels[21].edma_handle != NULL) {
      EDMA_HandleIRQ(_kinetis_dma_channels[21].edma_handle);
  }
  }
  OSAL_IRQ_EPILOGUE();
}

OSAL_IRQ_HANDLER(KINETIS_DMA6_IRQ_VECTOR) {
  OSAL_IRQ_PROLOGUE();
  if ((EDMA_GetChannelStatusFlags(DMA0, 6) & kEDMA_InterruptFlag) != 0U) {
    if (_kinetis_dma_channels[6].edma_handle != NULL) {
      EDMA_HandleIRQ(_kinetis_dma_channels[6].edma_handle);
    }
  }
  if ((EDMA_GetChannelStatusFlags(DMA0, 22) & kEDMA_InterruptFlag) != 0U) {
    if (_kinetis_dma_channels[22].edma_handle != NULL) {
      EDMA_HandleIRQ(_kinetis_dma_channels[22].edma_handle);
  }
  }
  OSAL_IRQ_EPILOGUE();
}

OSAL_IRQ_HANDLER(KINETIS_DMA7_IRQ_VECTOR) {
  OSAL_IRQ_PROLOGUE();
  if ((EDMA_GetChannelStatusFlags(DMA0, 7) & kEDMA_InterruptFlag) != 0U) {
    if (_kinetis_dma_channels[7].edma_handle != NULL) {
      EDMA_HandleIRQ(_kinetis_dma_channels[7].edma_handle);
    }
  }
  if ((EDMA_GetChannelStatusFlags(DMA0, 23) & kEDMA_InterruptFlag) != 0U) {
    if (_kinetis_dma_channels[23].edma_handle != NULL) {
      EDMA_HandleIRQ(_kinetis_dma_channels[23].edma_handle);
  }
  }
  OSAL_IRQ_EPILOGUE();
}

OSAL_IRQ_HANDLER(KINETIS_DMA8_IRQ_VECTOR) {
  OSAL_IRQ_PROLOGUE();
  if ((EDMA_GetChannelStatusFlags(DMA0, 8) & kEDMA_InterruptFlag) != 0U) {
    if (_kinetis_dma_channels[8].edma_handle != NULL) {
      EDMA_HandleIRQ(_kinetis_dma_channels[8].edma_handle);
    }
  }
  if ((EDMA_GetChannelStatusFlags(DMA0, 24) & kEDMA_InterruptFlag) != 0U) {
    if (_kinetis_dma_channels[24].edma_handle != NULL) {
      EDMA_HandleIRQ(_kinetis_dma_channels[24].edma_handle);
  }
  }
  OSAL_IRQ_EPILOGUE();
}

OSAL_IRQ_HANDLER(KINETIS_DMA9_IRQ_VECTOR) {
  OSAL_IRQ_PROLOGUE();
  if ((EDMA_GetChannelStatusFlags(DMA0, 9) & kEDMA_InterruptFlag) != 0U) {
    if (_kinetis_dma_channels[9].edma_handle != NULL) {
      EDMA_HandleIRQ(_kinetis_dma_channels[9].edma_handle);
    }
  }
  if ((EDMA_GetChannelStatusFlags(DMA0, 25) & kEDMA_InterruptFlag) != 0U) {
    if (_kinetis_dma_channels[25].edma_handle != NULL) {
      EDMA_HandleIRQ(_kinetis_dma_channels[25].edma_handle);
  }
  }
  OSAL_IRQ_EPILOGUE();
}

OSAL_IRQ_HANDLER(KINETIS_DMA10_IRQ_VECTOR) {
  OSAL_IRQ_PROLOGUE();
  if ((EDMA_GetChannelStatusFlags(DMA0, 10) & kEDMA_InterruptFlag) != 0U) {
    if (_kinetis_dma_channels[10].edma_handle != NULL) {
      EDMA_HandleIRQ(_kinetis_dma_channels[10].edma_handle);
    }
  }
  if ((EDMA_GetChannelStatusFlags(DMA0, 26) & kEDMA_InterruptFlag) != 0U) {
    if (_kinetis_dma_channels[26].edma_handle != NULL) {
      EDMA_HandleIRQ(_kinetis_dma_channels[26].edma_handle);
  }
  }
  OSAL_IRQ_EPILOGUE();
}

OSAL_IRQ_HANDLER(KINETIS_DMA11_IRQ_VECTOR) {
  OSAL_IRQ_PROLOGUE();
  if ((EDMA_GetChannelStatusFlags(DMA0, 11) & kEDMA_InterruptFlag) != 0U) {
    if (_kinetis_dma_channels[11].edma_handle != NULL) {
      EDMA_HandleIRQ(_kinetis_dma_channels[11].edma_handle);
    }
  }
  if ((EDMA_GetChannelStatusFlags(DMA0, 27) & kEDMA_InterruptFlag) != 0U) {
    if (_kinetis_dma_channels[27].edma_handle != NULL) {
      EDMA_HandleIRQ(_kinetis_dma_channels[27].edma_handle);
  }
  }
  OSAL_IRQ_EPILOGUE();
}

OSAL_IRQ_HANDLER(KINETIS_DMA12_IRQ_VECTOR) {
  OSAL_IRQ_PROLOGUE();
  if ((EDMA_GetChannelStatusFlags(DMA0, 12) & kEDMA_InterruptFlag) != 0U) {
    if (_kinetis_dma_channels[12].edma_handle != NULL) {
      EDMA_HandleIRQ(_kinetis_dma_channels[12].edma_handle);
    }
  }
  if ((EDMA_GetChannelStatusFlags(DMA0, 28) & kEDMA_InterruptFlag) != 0U) {
    if (_kinetis_dma_channels[28].edma_handle != NULL) {
      EDMA_HandleIRQ(_kinetis_dma_channels[28].edma_handle);
  }
  }
  OSAL_IRQ_EPILOGUE();
}

OSAL_IRQ_HANDLER(KINETIS_DMA13_IRQ_VECTOR) {
  OSAL_IRQ_PROLOGUE();
  if ((EDMA_GetChannelStatusFlags(DMA0, 13) & kEDMA_InterruptFlag) != 0U) {
    if (_kinetis_dma_channels[13].edma_handle != NULL) {
      EDMA_HandleIRQ(_kinetis_dma_channels[13].edma_handle);
    }
  }
  if ((EDMA_GetChannelStatusFlags(DMA0, 29) & kEDMA_InterruptFlag) != 0U) {
    if (_kinetis_dma_channels[29].edma_handle != NULL) {
      EDMA_HandleIRQ(_kinetis_dma_channels[29].edma_handle);
  }
  }
  OSAL_IRQ_EPILOGUE();
}

OSAL_IRQ_HANDLER(KINETIS_DMA14_IRQ_VECTOR) {
  OSAL_IRQ_PROLOGUE();
  if ((EDMA_GetChannelStatusFlags(DMA0, 14) & kEDMA_InterruptFlag) != 0U) {
    if (_kinetis_dma_channels[14].edma_handle != NULL) {
      EDMA_HandleIRQ(_kinetis_dma_channels[14].edma_handle);
    }
  }
  if ((EDMA_GetChannelStatusFlags(DMA0, 30) & kEDMA_InterruptFlag) != 0U) {
    if (_kinetis_dma_channels[30].edma_handle != NULL) {
      EDMA_HandleIRQ(_kinetis_dma_channels[30].edma_handle);
  }
  }
  OSAL_IRQ_EPILOGUE();
}

OSAL_IRQ_HANDLER(KINETIS_DMA15_IRQ_VECTOR) {
  OSAL_IRQ_PROLOGUE();
  if ((EDMA_GetChannelStatusFlags(DMA0, 15) & kEDMA_InterruptFlag) != 0U) {
    if (_kinetis_dma_channels[15].edma_handle != NULL) {
      EDMA_HandleIRQ(_kinetis_dma_channels[15].edma_handle);
    }
  }
  if ((EDMA_GetChannelStatusFlags(DMA0, 31) & kEDMA_InterruptFlag) != 0U) {
    if (_kinetis_dma_channels[31].edma_handle != NULL) {
      EDMA_HandleIRQ(_kinetis_dma_channels[31].edma_handle);
  }
  }
  OSAL_IRQ_EPILOGUE();
}

edma_config_t global_dma_config;

void dmaInit(void)
{
  DMAMUX_Init(DMAMUX0);
  EDMA_GetDefaultConfig(&global_dma_config);
  EDMA_Init(DMA0, &global_dma_config);
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
bool dmaChannelAllocate(edma_handle_t *edma_handle,
                        uint32_t req_src,
                        uint32_t priority,
                        edma_callback func,
                        void *param) {
  kinetis_edma_channel_t *dmach;
  for(uint8_t i = 0; i < FSL_FEATURE_EDMA_DMAMUX_CHANNELS; i++) {
    /* Checks if the stream is already taken.*/
    dmach = KINETIS_DMA_CHANNEL(i);
    if (dmach->edma_handle != NULL)
      continue;
    
    DMAMUX_SetSource(DMAMUX0, i, req_src);
    if (req_src != kDmaRequestMux0Disable) {
      DMAMUX_EnableChannel(DMAMUX0, i);
    }
    EDMA_CreateHandle(edma_handle, DMA0, i);

    dmach->edma_handle = edma_handle;
    if (func != NULL) {
      EDMA_SetCallback(edma_handle, func, param);
    }
    nvicEnableVector(dmach->vector, priority);
    return false;
  }
  return true;
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
void dmaChannelRelease(const edma_handle_t *edma_handle) {

  osalDbgCheck(edma_handle != NULL);
  kinetis_edma_channel_t *dmach = KINETIS_DMA_CHANNEL(edma_handle->channel);
  /* Check if the streams is not taken.*/
  osalDbgAssert(dmach->edma_handle != NULL,
                "not allocated");

  /* Disables the associated IRQ vector if unused.*/
  if (KINETIS_DMA_CHANNEL(DMA_INT_SHARED_WITH(edma_handle->channel))->edma_handle == NULL) {
    nvicDisableVector(dmach->vector);
  }

  /* Marks the stream as not allocated.*/
  dmach->edma_handle = NULL;
}