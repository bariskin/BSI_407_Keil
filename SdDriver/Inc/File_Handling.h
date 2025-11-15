/*
 * File_Handling_RTOS.h
 *
 *  Created on: 14-May-2020
 *      Author: Controllerstech
 */

#ifndef FILE_HANDLING_RTOS_H_
#define FILE_HANDLING_RTOS_H_

#include "fatfs.h"
#include "string.h"
#include "stdio.h"
#include "fatfs.h"
#include <stdlib.h> // для malloc

extern char SDbuffer[30];

#define 	UART 			&huart1
#define 	CLUSTER_SIZE	16384

extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart4;


#endif /* FILE_HANDLING_RTOS_H_ */
