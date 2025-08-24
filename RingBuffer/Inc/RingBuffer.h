/**
  ******************************************************************************
  * @file      RingBuffer.h
  * @author    OnWert
  * @version   
  * @brief     This file contains defines and all the functions prototypes for the RingBuffer.c   
  *     
  ******************************************************************************
*/
#ifndef _RINGBUFFER_H
#define _RINGBUFFER_H

#ifdef __cplusplus
extern "C" {
#endif
  
#include <stdint.h>
#include <stdlib.h>

 /** @defgroup Ringbuffer
  * @{
  */  
         
  
 /**
  * @}
 */  
    
#define  CIRC_BUF_RX_SIZE 256
#define  CIRC_BUF_TX_SIZE 128
  
#define  CIRC_BUF_RX_MASK    (CIRC_BUF_RX_SIZE -1)
#define  CIRC_BUF_TX_MASK    (CIRC_BUF_TX_SIZE -1)
  
extern uint8_t CDC_Transmit_FS(uint8_t* Buf, uint16_t Len);  
  
 
 /** @ingroup Ringbuffer
  *  @struct  RING_buffer_t
  *  @brief   Circular biffer structure
  */ 
typedef struct        /** structure of ring buffer */ 
  {
    uint8_t *buffer;  /** pointer of ring buffer */
    uint16_t idxIn;   /** input index of ring  buffer */
    uint16_t idxOut;  /** output index of ring buffer */
    uint16_t size;    /** size of ring buffer */
    
 } RING_buffer_t;     /** structure of ring buffer */ 
  
   
 /**@ingroup Ringbuffer
  * @enum    RING_ErrorStatus_t
  * @brief   Enumerate for RING_ErrorStatus_t
  */ 

typedef enum 
  {
    RING_ERROR = 0,             
    RING_SUCCESS = !RING_ERROR  
  } RING_ErrorStatus_t;  /** enumeration of RING_ErrorStatus_t */ 
 
 /** @ingroup Ringbuffer
  *  @enum RX_Buffer_State
  *  @brief Enumerate for RX_Buffer_State
  */    
typedef enum 
  {
     RX_BUF_FAIL = 0,
     RX_BUF_DONE,
     RX_BUF_EMPTY,
     RX_BUF_FULL
  }RX_Buffer_State ; 
 
 /** @ingroup Ringbuffer
  *  @enum TX_Buffer_State
  *  @brief Enumerate for TX_Buffer_State
  */    
typedef enum 
  {
     TX_BUF_FAIL = 0,
     TX_BUF_DONE,
     TX_BUF_EMPTY,
     TX_BUF_FULL
  }TX_Buffer_State ;

 /**  @addtogroup Ringbuffer
  * @{
  */

 RING_ErrorStatus_t RING_Init(RING_buffer_t *ring, uint8_t *buffer, uint16_t buffersize);
   
 void RING_Put(uint8_t symbol, RING_buffer_t* buf);
 
 uint8_t RING_Pop(RING_buffer_t *buf);
 
 uint16_t RING_GetCount(RING_buffer_t *buf);

 int32_t RING_ShowSymbol(uint16_t symbolNumber ,RING_buffer_t *buf);
 
 void RING_Clear(RING_buffer_t* buf);
 
 uint16_t  Check_FreeSpace_RING_Buf(RING_buffer_t *buf,uint16_t buffersize);
 
 /**
   * @}
   */ 

#ifdef __cplusplus
}
#endif

#endif  /* _RINGBUFFER_H */
