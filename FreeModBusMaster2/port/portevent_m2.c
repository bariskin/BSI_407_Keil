/*
 * FreeModbus Libary: RT-Thread Port
 * Copyright (C) 2013 Armink <armink.ztl@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * File: $Id: portevent_m.c v 1.60 2013/08/13 15:07:05 Armink add Master Functions$
 */

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mb_m2.h"
#include "mbport2.h"
#include "port.h"



/* ----------------------- Defines ------------------------------------------*/

/* ----------------------- Variables ----------------------------------------*/

static eMBMasterEventType eQueuedEvent;
static BOOL xEventInQueue;



/* ----------------------- Start implementation -----------------------------*/

/*----------------------------------------------------------------------------*/
BOOL xMBMaster2PortEventInit(void)
{
  xEventInQueue = FALSE;
  return TRUE;
}



/*----------------------------------------------------------------------------*/
BOOL xMBMaster2PortEventPost(eMBMasterEventType eEvent)
{
  xEventInQueue = TRUE;
  eQueuedEvent = eEvent;
  return TRUE;
}



/*----------------------------------------------------------------------------*/
BOOL xMBMaster2PortEventGet(eMBMasterEventType * eEvent)
{
  BOOL xEventHappened = FALSE;

  if(xEventInQueue)
  {
    *eEvent = eQueuedEvent;
    xEventInQueue = FALSE;
    xEventHappened = TRUE;
  }

  return xEventHappened;
}



/*----------------------------------------------------------------------------*/
/**
 * This function is initialize the OS resource for modbus master.
 * Note:The resource is define by OS.If you not use OS this function can be empty.
 *
 */
void vMBMaster2OsResInit(void)
{
}



/*----------------------------------------------------------------------------*/
/**
 * This function is take Mobus Master running resource.
 * Note:The resource is define by Operating System.If you not use OS this function can be just return TRUE.
 *
 * @param lTimeOut the waiting time.
 *
 * @return resource taked result
 */
BOOL xMBMaster2RunResTake(int32_t lTimeOut)
{
  return TRUE;
}



/*----------------------------------------------------------------------------*/
/**
 * This function is release Mobus Master running resource.
 * Note:The resource is define by Operating System.If you not use OS this function can be empty.
 *
 */
void vMBMaster2RunResRelease(void)
{
}



/*----------------------------------------------------------------------------*/
/**
 * This is modbus master respond timeout error process callback function.
 * @note There functions will block modbus master poll while execute OS waiting.
 * So,for real-time of system.Do not execute too much waiting process.
 *
 * @param ucDestAddress destination salve address
 * @param pucPDUData PDU buffer data
 * @param ucPDULength PDU buffer length
 *
 */
void vMBMaster2ErrorCBRespondTimeout(UCHAR ucDestAddress, const UCHAR* pucPDUData,
                                    USHORT ucPDULength)
{
  xMBMaster2PortEventPost(EV_MASTER_ERROR_RESPOND_TIMEOUT);
}



/*----------------------------------------------------------------------------*/
void vMBMaster2ErrorCBReceiveData(UCHAR ucDestAddress, const UCHAR* pucPDUData,
                                 USHORT ucPDULength)
{
  xMBMaster2PortEventPost(EV_MASTER_ERROR_RECEIVE_DATA);
}



/*----------------------------------------------------------------------------*/
void vMBMaster2ErrorCBExecuteFunction(UCHAR ucDestAddress, const UCHAR* pucPDUData,
                                     USHORT ucPDULength)
{
  xMBMaster2PortEventPost(EV_MASTER_ERROR_EXECUTE_FUNCTION);
}



/*----------------------------------------------------------------------------*/
void vMBMaster2CBRequestScuuess(void)
{
  xMBMaster2PortEventPost(EV_MASTER_PROCESS_SUCESS);
}



/*----------------------------------------------------------------------------*/
eMBMasterReqErrCode eMBMaster2WaitRequestFinish(void)
{
  eMBMasterReqErrCode eErrStatus = MB_MRE_NO_ERR;

  switch (eQueuedEvent)
  {
    case EV_MASTER_PROCESS_SUCESS:
      break;

    case EV_MASTER_ERROR_RESPOND_TIMEOUT:
      eErrStatus = MB_MRE_TIMEDOUT;
      break;

    case EV_MASTER_ERROR_RECEIVE_DATA:
      eErrStatus = MB_MRE_REV_DATA;
      break;

    case EV_MASTER_ERROR_EXECUTE_FUNCTION:
      eErrStatus = MB_MRE_EXE_FUN;
      break;

    default:
      break;
  }

  return eErrStatus;
}



/*----------------------------------------------------------------------------*/
