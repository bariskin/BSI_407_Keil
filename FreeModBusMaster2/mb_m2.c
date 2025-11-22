/*
 * FreeModbus Libary: A portable Modbus implementation for Modbus ASCII/RTU.
 * Copyright (C) 2013 Armink <armink.ztl@gmail.com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * File: $Id: mbrtu_m.c,v 1.60 2013/08/20 11:18:10 Armink Add Master Functions $
 */

/* ----------------------- System includes ----------------------------------*/
#include "stdlib.h"
#include "string.h"

/* ----------------------- Platform includes --------------------------------*/
#include "port.h"

/* ----------------------- Modbus includes ----------------------------------*/

#include "mb.h"
#include "mb_m2.h"
#include "mbconfig2.h"
#include "mbframe.h"
#include "mbproto.h"
#include "mbfunc2.h"
#include "mbport2.h"

#if MB_MASTER2_RTU_ENABLED == 1
#include "mbrtu2.h"
#endif

#if MB_MASTER2_RTU_ENABLED > 0

#ifndef MB_PORT2_HAS_CLOSE
#define MB_PORT2_HAS_CLOSE 0
#endif

/* ----------------------- Static variables ---------------------------------*/

static UCHAR    ucMBMasterDestAddress;
static BOOL     xMBRunInMasterMode = FALSE;
static eMBMasterErrorEventType eMBMasterCurErrorType;

static enum
{
    STATE_ENABLED,
    STATE_DISABLED,
    STATE_NOT_INITIALIZED,
    STATE_ESTABLISHED,
} eMBState = STATE_NOT_INITIALIZED;

/* Functions pointer which are initialized in eMBInit( ). Depending on the
 * mode (RTU or ASCII) the are set to the correct implementations.
 * Using for Modbus Master,Add by Armink 20130813
 */
static peMBFrameSend peMBMaster2FrameSendCur;
static pvMBFrameStart pvMBMaster2FrameStartCur;
static pvMBFrameStop pvMBMaster2FrameStopCur;
static peMBFrameReceive peMBMaster2FrameReceiveCur;
static pvMBFrameClose pvMBMaster2FrameCloseCur;

/* Callback functions required by the porting layer. They are called when
 * an external event has happend which includes a timeout or the reception
 * or transmission of a character.
 * Using for Modbus Master,Add by Armink 20130813
 */
BOOL( *pxMBMaster2FrameCBByteReceived ) ( void );
BOOL( *pxMBMaster2FrameCBTransmitterEmpty ) ( void );
BOOL( *pxMBMaster2PortCBTimerExpired ) ( void );

BOOL( *pxMBMaster2FrameCBReceiveFSMCur ) ( void );
BOOL( *pxMBMaster2FrameCBTransmitFSMCur ) ( void );

/* An array of Modbus functions handlers which associates Modbus function
 * codes with implementing functions.
 */
static xMBFunctionHandler xMaster2FuncHandlers[MB_FUNC_HANDLERS_MAX] = {
#if MB_FUNC_OTHER_REP_SLAVEID_ENABLED > 0
    //TODO Add Master function define
    {MB_FUNC_OTHER_REPORT_SLAVEID, eMBFuncReportSlaveID},
#endif
#if MB_FUNC_READ_INPUT_ENABLED > 0
    {MB_FUNC_READ_INPUT_REGISTER, eMBMaster2FuncReadInputRegister},
#endif
#if MB_FUNC_READ_HOLDING_ENABLED > 0
    {MB_FUNC_READ_HOLDING_REGISTER, eMBMaster2FuncReadHoldingRegister},
#endif
#if MB_FUNC_WRITE_MULTIPLE_HOLDING_ENABLED > 0
    {MB_FUNC_WRITE_MULTIPLE_REGISTERS, eMBMaster2FuncWriteMultipleHoldingRegister},
#endif
#if MB_FUNC_WRITE_HOLDING_ENABLED > 0
    {MB_FUNC_WRITE_REGISTER, eMBMaster2FuncWriteHoldingRegister},
#endif
#if MB_FUNC_READWRITE_HOLDING_ENABLED > 0
    {MB_FUNC_READWRITE_MULTIPLE_REGISTERS, eMBMaster2FuncReadWriteMultipleHoldingRegister},
#endif
#if MB_FUNC_READ_COILS_ENABLED > 0
    {MB_FUNC_READ_COILS, eMBMasterFuncReadCoils},
#endif
#if MB_FUNC_WRITE_COIL_ENABLED > 0
    {MB_FUNC_WRITE_SINGLE_COIL, eMBMasterFuncWriteCoil},
#endif
#if MB_FUNC_WRITE_MULTIPLE_COILS_ENABLED > 0
    {MB_FUNC_WRITE_MULTIPLE_COILS, eMBMasterFuncWriteMultipleCoils},
#endif
#if MB_FUNC_READ_DISCRETE_INPUTS_ENABLED > 0
    {MB_FUNC_READ_DISCRETE_INPUTS, eMBMasterFuncReadDiscreteInputs},
#endif
};

/* ----------------------- Start implementation -----------------------------*/
eMBErrorCode
eMBMaster2Init( eMBMode eMode, UCHAR ucPort, ULONG ulBaudRate, eMBParity eParity )
{
    eMBErrorCode    eStatus = MB_ENOERR;

    switch (eMode)
    {
#if MB_MASTER2_RTU_ENABLED > 0
    case MB_RTU:
        pvMBMaster2FrameStartCur = eMBMaster2RTUStart;
        pvMBMaster2FrameStopCur = eMBMaster2RTUStop;
        peMBMaster2FrameSendCur = eMBMaster2RTUSend;
        peMBMaster2FrameReceiveCur = eMBMaster2RTUReceive;
        pvMBMaster2FrameCloseCur = MB_PORT2_HAS_CLOSE ? vMBMaster2PortClose : NULL;
        pxMBMaster2FrameCBByteReceived = xMBMaster2RTUReceiveFSM;
        pxMBMaster2FrameCBTransmitterEmpty = xMBMaster2RTUTransmitFSM;
        pxMBMaster2PortCBTimerExpired = xMBMaster2RTUTimerExpired;

        eStatus = eMBMaster2RTUInit(ucPort, ulBaudRate, eParity);
        break;
#endif

    default:
        eStatus = MB_EINVAL;
        break;
    }

    if (eStatus == MB_ENOERR)
    {
        if (!xMBMaster2PortEventInit())
        {
            /* port dependent event module initalization failed. */
            eStatus = MB_EPORTERR;
        }
        else
        {
            eMBState = STATE_DISABLED;
        }
        /* initialize the OS resource for modbus master. */
        vMBMaster2OsResInit();
    }
    return eStatus;
}

eMBErrorCode
eMBMaster2Close( void )
{
    eMBErrorCode    eStatus = MB_ENOERR;

    if( eMBState == STATE_DISABLED )
    {
        if( pvMBMaster2FrameCloseCur != NULL )
        {
            pvMBMaster2FrameCloseCur(  );
        }
    }
    else
    {
        eStatus = MB_EILLSTATE;
    }
    return eStatus;
}

eMBErrorCode
eMBMaster2Enable( void )
{
    eMBErrorCode    eStatus = MB_ENOERR;

    if( eMBState == STATE_DISABLED )
    {
        /* Activate the protocol stack. */
        pvMBMaster2FrameStartCur(  );
        eMBState = STATE_ENABLED;
    }
    else
    {
        eStatus = MB_EILLSTATE;
    }
    return eStatus;
}

eMBErrorCode
eMBMaster2Disable( void )
{
    eMBErrorCode    eStatus;

    if(( eMBState == STATE_ENABLED ) || ( eMBState == STATE_ESTABLISHED))
    {
        pvMBMaster2FrameStopCur(  );
        eMBState = STATE_DISABLED;
        eStatus = MB_ENOERR;
    }
    else if( eMBState == STATE_DISABLED )
    {
        eStatus = MB_ENOERR;
    }
    else
    {
        eStatus = MB_EILLSTATE;
    }
    return eStatus;
}

BOOL
eMBMaster2IsEstablished( void )
{
    if(eMBState == STATE_ESTABLISHED)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}


eMBErrorCode
eMBMaster2Poll( void )
{
    static UCHAR   *ucMBFrame;
    static UCHAR    ucRcvAddress;
    static UCHAR    ucFunctionCode;
    static USHORT   usLength;
    static eMBException eException;

    int             i , j;
    eMBErrorCode    eStatus = MB_ENOERR;
    eMBMasterEventType    eEvent;
    eMBMasterErrorEventType errorType;

    /* Check if the protocol stack is ready. */
    if(( eMBState != STATE_ENABLED ) && ( eMBState != STATE_ESTABLISHED))
    {
        return MB_EILLSTATE;
    }

    /* Check if there is a event available. If not return control to caller.
     * Otherwise we will handle the event. */
    if( xMBMaster2PortEventGet( &eEvent ) == TRUE )
    {
        switch ( eEvent )
        {
        case EV_MASTER_READY:
            eMBState = STATE_ESTABLISHED;
            break;

        case EV_MASTER_FRAME_RECEIVED:
            eStatus = peMBMaster2FrameReceiveCur( &ucRcvAddress, &ucMBFrame, &usLength );
            /* Check if the frame is for us. If not ,send an error process event. */
            if ( ( eStatus == MB_ENOERR ) && ( ucRcvAddress == ucMBMaster2GetDestAddress() ) )
            {
                ( void ) xMBMaster2PortEventPost( EV_MASTER_EXECUTE );
            }
            else
            {
                vMBMaster2SetErrorType(EV_ERROR_RECEIVE_DATA);
                ( void ) xMBMaster2PortEventPost( EV_MASTER_ERROR_PROCESS );
            }
            break;

        case EV_MASTER_EXECUTE:
            ucFunctionCode = ucMBFrame[MB_PDU_FUNC_OFF];
            eException = MB_EX_ILLEGAL_FUNCTION;
            /* If receive frame has exception .The receive function code highest bit is 1.*/
            if(ucFunctionCode >> 7) {
                eException = (eMBException)ucMBFrame[MB_PDU_DATA_OFF];
            }
            else
            {
                for (i = 0; i < MB_FUNC_HANDLERS_MAX; i++)
                {
                    /* No more function handlers registered. Abort. */
                    if (xMaster2FuncHandlers[i].ucFunctionCode == 0) {
                        break;
                    }
                    else if (xMaster2FuncHandlers[i].ucFunctionCode == ucFunctionCode) {
                        vMBMaster2SetCBRunInMasterMode(TRUE);
                        /* If master request is broadcast,
                         * the master need execute function for all slave.
                         */
                        if ( xMBMaster2RequestIsBroadcast() ) {
                            usLength = usMBMaster2GetPDUSndLength();
                            for(j = 1; j <= MB_MASTER_TOTAL_SLAVE_NUM; j++){
                                vMBMaster2SetDestAddress(j);
                                eException = xMaster2FuncHandlers[i].pxHandler(ucMBFrame, &usLength);
                            }
                        }
                        else {
                            eException = xMaster2FuncHandlers[i].pxHandler(ucMBFrame, &usLength);
                        }
                        vMBMaster2SetCBRunInMasterMode(FALSE);
                        break;
                    }
                }
            }
            /* If master has exception ,Master will send error process.Otherwise the Master is idle.*/
            if (eException != MB_EX_NONE) {
                vMBMaster2SetErrorType(EV_ERROR_EXECUTE_FUNCTION);
                ( void ) xMBMaster2PortEventPost( EV_MASTER_ERROR_PROCESS );
            }
            else {
                vMBMasterCBRequestScuuess( );
                vMBMaster2RunResRelease( );
            }
            break;

        case EV_MASTER_FRAME_SENT:
            /* Master is busy now. */
            vMBMaster2GetPDUSndBuf( &ucMBFrame );
            eStatus = peMBMaster2FrameSendCur( ucMBMaster2GetDestAddress(), ucMBFrame, usMBMaster2GetPDUSndLength() );
            break;

        case EV_MASTER_ERROR_PROCESS:
            /* Execute specified error process callback function. */
            errorType = eMBMaster2GetErrorType();
            vMBMaster2GetPDUSndBuf( &ucMBFrame );
            switch (errorType) {
            case EV_ERROR_RESPOND_TIMEOUT:
                vMBMaster2ErrorCBRespondTimeout(ucMBMaster2GetDestAddress(),
                        ucMBFrame, usMBMaster2GetPDUSndLength());
                break;
            case EV_ERROR_RECEIVE_DATA:
                vMBMaster2ErrorCBReceiveData(ucMBMaster2GetDestAddress(),
                        ucMBFrame, usMBMaster2GetPDUSndLength());
                break;
            case EV_ERROR_EXECUTE_FUNCTION:
                vMBMaster2ErrorCBExecuteFunction(ucMBMaster2GetDestAddress(),
                        ucMBFrame, usMBMaster2GetPDUSndLength());
                break;
            }
            vMBMaster2RunResRelease();
            break;

        default:
            break;
        }

    }
    return MB_ENOERR;
}

/* Get whether the Modbus Master is run in master mode.*/
BOOL xMBMaster2GetCBRunInMasterMode( void )
{
    return xMBRunInMasterMode;
}
/* Set whether the Modbus Master is run in master mode.*/
void vMBMaster2SetCBRunInMasterMode( BOOL IsMasterMode )
{
    xMBRunInMasterMode = IsMasterMode;
}
/* Get Modbus Master send destination address. */
UCHAR ucMBMaster2GetDestAddress( void )
{
    return ucMBMasterDestAddress;
}
/* Set Modbus Master send destination address. */
void vMBMaster2SetDestAddress( UCHAR Address )
{
    ucMBMasterDestAddress = Address;
}
/* Get Modbus Master current error event type. */
eMBMasterErrorEventType eMBMaster2GetErrorType( void )
{
    return eMBMasterCurErrorType;
}
/* Set Modbus Master current error event type. */
void vMBMaster2SetErrorType( eMBMasterErrorEventType errorType )
{
    eMBMasterCurErrorType = errorType;
}



#endif
