/**
******************************************************************************
* @file      RelayModule.h
* @author    OnWert
* @version   
* @brief     This file contains defines and all the functions prototypes for the RelayModule.c
******************************************************************************
*/
#ifndef _RELEAY_MODULE_H
#define _RELEAY_MODULE_H

#ifdef __cplusplus
extern "C" {
#endif
  
 /* ------------------------Includes ----------------------------------*/
 
 #include "stm32f4xx_hal.h"
  #include <stdint.h>
  /** @defgroup relay_module
  * @{
  */
  
  /**
  * @}
  */
 /* ------------------------Defines ----------------------------------*/
#define RELAY_COUNT         4
#define CHANNEL_COUNT       10
#define EVENTS_PER_CHANNEL  4
#define MODULE_COUNT        10

typedef enum
{
    RELAY_CMD_NONE = 0,
    RELAY_CMD_ON,
    RELAY_CMD_OFF,
    RELAY_CMD_TOGGLE
} RelayCommand;

// ------- Структуры событий и каналов --------

// Событие канала
typedef struct {
    uint8_t event_id;
    RelayCommand action;   // ON / OFF / TOGGLE
} Event;

// Канал (имеет 4 события)
 typedef struct {
    uint8_t channel_id;                // ID канала (1–10)
    Event events[EVENTS_PER_CHANNEL];  // События
 } Channel;


// ------- Структуры реле и модуля --------

// Канал, на который реагирует конкретное реле
typedef struct {
    uint8_t channel_id;                // ID канала
    uint8_t active_events[EVENTS_PER_CHANNEL]; 
    // Массив 0/1 — реагирует ли реле на конкретное событие этого канала
} RelayReaction;
 
 
// Структура реле
typedef struct {
    uint8_t relay_id;       // ID реле (1–4)
    RelayReaction reactions[CHANNEL_COUNT];
    //RelayAction action;   // функция действия для этого реле
    // Для каждого канала храним реакцию
} Relay;


// Модуль реле
typedef struct {
    uint8_t module_id;            // ID модуля
    Relay relays[RELAY_COUNT];    // 4 реле
    Channel channels[CHANNEL_COUNT]; // 10 каналов
} RelayModule;

 /* ------------------------External variables -------------------------*/
  // Callback для действия реле
typedef void (*RelayAction)(uint8_t module_id,  uint8_t relay_id, RelayCommand cmd);

  /** @addtogroup relay_module
  * @{
  */

 void init_modules();

 void relay_set_reaction(RelayModule *module,
                            uint8_t relay_id,
                          uint8_t channel_id,
                            uint8_t event_id,
                             uint8_t enabled);
 
 void process_event(RelayModule *module,
                     uint8_t channel_id,
                       uint8_t event_id,
                    RelayAction action);
 
// void relay_set_reactions_bulk(RelayModule *module,
//                                  uint8_t relay_id,
//                                uint8_t channel_id,
//                              uint8_t events_mask);
 
 
 void relay_test_callback(uint8_t module_id,
                         uint8_t relay_id,
                         RelayCommand cmd);
 
 
 
  /**
  * @}
  */
  
#ifdef __cplusplus
}
#endif

#endif  /* _RELEAY_MODULE_H */