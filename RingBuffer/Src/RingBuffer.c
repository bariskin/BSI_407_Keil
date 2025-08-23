/**
  ******************************************************************************
  * @file      RingBuffer.c
  * @author    OnWert
  * @version   
  * @brief     This file provides common functions for the RingBuffer                
  ******************************************************************************
*/
/* ------------------------Includes ----------------------------------*/
#include "RingBuffer.h"

/**
  * @brief       Init ring buffer structure
  * @param[in]   ring   - structure of ring buffer 
  * @param[in]   buffer - array of ring buffer 
  * @param[in]   size   - size of array
  * @retval      RING_ErrorStatus_t
 */

RING_ErrorStatus_t RING_Init(RING_buffer_t *ring, uint8_t *buffer, uint16_t buffersize)
{
  ring->size = buffersize;
  ring->buffer = buffer;
  ring->idxIn = 0;
  ring->idxOut = 0;
  
  return ( ring->buffer ? RING_SUCCESS : RING_ERROR ) ;
}

/**
  * @brief      Put char (byte) to ring buffer
  * @param[in]  symbol - input char
  * @param[in]  buf    - structure of ring buffer 
  * @retval     None
 */
void RING_Put(uint8_t symbol, RING_buffer_t* buf){

   buf->buffer[buf->idxIn++] = symbol;
   
   if (buf->idxIn >= buf->size)
   {
     buf->idxIn = 0;
   }
}

/**
  * @brief     Get char (byte) from ring buffer
  * @param[in] buf  - structure of ring buffer  
  * @retval    char (byte)
  */
uint8_t RING_Pop(RING_buffer_t *buf ){
  
  uint8_t retval = buf->buffer[buf->idxOut++];
  
  if (buf->idxOut >= buf->size){
    buf->idxOut = 0;
  }
  return retval;
}

/**
  * @brief      Get count of elements in ring buffer
  * @param[in]  buf - structure of ring buffer 
  * @retval     count of elements in ring buffer
 */
uint16_t RING_GetCount(RING_buffer_t *buf){
  
  uint16_t retval = 0;
  
  if(buf->idxIn >= buf->idxOut)
  {
    retval = buf->idxIn - buf->idxOut;
  }
  else 
  {
    retval =  CIRC_BUF_TX_SIZE - buf->idxOut; 
  }   
  return retval;
}

/**
  * @brief      Show char 
  * @param[in]  symbolNumber - symbol number
  * @param[in]  buf - structure of ring buffer 
  * @retval     char (byte)
 */
int32_t RING_ShowSymbol(uint16_t symbolNumber ,RING_buffer_t *buf){
  
  uint32_t pointer = buf->idxOut + symbolNumber;
  
  int32_t  retval = -1;
  
  if (symbolNumber < RING_GetCount(buf))
  {
    if (pointer > buf->size) {
      pointer -= buf->size;
    }
    retval = buf->buffer[ pointer ] ;
  }
  return retval;
}

/**
  * @brief      Clear ring buffer
  * @param[in]  buf - structure of ring buffer 
  * @retval     None
 */
void RING_Clear(RING_buffer_t* buf){

  buf->idxIn = 0;
  buf->idxOut = 0;
}

/**
  * @brief      Check free space of ring buffer 
  * @param[in]  buf - structure of ring buffer 
  * @param[in]  buffersize
  * @retval     free space 
 */
uint16_t  Check_FreeSpace_RING_Buf(RING_buffer_t *buf,uint16_t buffersize){
  
  uint16_t free_space = 0;
  
  free_space = buffersize  -  RING_GetCount(buf); 
  
  return  free_space;
}

/************************ (C) COPYRIGHT  OnWert*****END OF FILE****/
