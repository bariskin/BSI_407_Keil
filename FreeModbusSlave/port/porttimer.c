#include "port.h"
#include "mb.h"
#include "mbport.h"

#if MB_SLAVE_RTU_ENABLED > 0 || MB_SLAVE_ASCII_ENABLED > 0

/* ----------------------- User defenitions ---------------------------------*/

extern	TIM_HandleTypeDef *ModBusSlaveTimer;
extern	uint16_t ModBusSlaveTimeout;
extern	volatile uint16_t ModBusSlaveCounter;

/* ----------------------- Start implementation -----------------------------*/
BOOL xMBPortTimersInit( USHORT usTim1Timerout50us, void *dHTIM )
{
	ModBusSlaveTimeout = 0;
	ModBusSlaveTimer = (TIM_HandleTypeDef *)dHTIM;
	ModBusSlaveTimeout = usTim1Timerout50us;
	return TRUE;
}

inline void vMBPortTimersEnable(  )
{
	ModBusSlaveCounter = 0;
	  //HAL_GPIO_WritePin(GPIOA, LED3_CNT_OUT1_Pin, GPIO_PIN_SET);
	HAL_TIM_Base_Start_IT(ModBusSlaveTimer);
}

inline void vMBPortTimersDisable(  )
{
	   //HAL_GPIO_WritePin(GPIOA, LED3_CNT_OUT1_Pin, GPIO_PIN_RESET);
	HAL_TIM_Base_Stop_IT(ModBusSlaveTimer);
}
#endif
