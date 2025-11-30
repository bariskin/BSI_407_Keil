/**
******************************************************************************
* @file      RelayModule.c
* @author    OnWert
* @version   
* @brief     This file provides common functions for the  RelayModule
*           
*/       
/* ------------------------Includes ----------------------------------*/
#include "RelayModule.h"

/* ------------------------External variables -------------------------*/

/* ------------------------Global variables----------------------------*/

/* ------------------------Locale variables----------------------------*/
RelayModule modules[MODULE_COUNT];
/* ------------------------Functions-----------------------------------*/

void init_modules()
{
    for (int IdxModule = 0; IdxModule < MODULE_COUNT; IdxModule++)
    {
        modules[IdxModule].module_id = IdxModule + 1;

        // Инициализация каналов модуля
        for (int ch = 0; ch < CHANNEL_COUNT; ch++)
        {
            modules[IdxModule].channels[ch].channel_id = ch + 1;
            
              

            // Инициализация событий
            for (int IdxEvent = 0; IdxEvent < EVENTS_PER_CHANNEL; IdxEvent++)
            {
                   modules[IdxModule].channels[ch].events[IdxEvent].event_id = IdxEvent + 1;
                
                 // < Здесь добавляем команду для реле
                  modules[IdxModule].channels[ch].events[IdxEvent].action = RELAY_CMD_OFF; // все реле выключить 
                
            }
        }

        // Инициализация реле
       for (int IdxRelay = 0; IdxRelay < RELAY_COUNT; IdxRelay++)
         {
           modules[IdxModule].relays[IdxRelay].relay_id = IdxRelay + 1;
    
             for (int ch = 0; ch < CHANNEL_COUNT; ch++)
              {
                modules[IdxModule].relays[IdxRelay].reactions[ch].channel_id = ch + 1;
								
                for (int IdxEvent = 0; IdxEvent < EVENTS_PER_CHANNEL; IdxEvent++)
								{
                 modules[IdxModule].relays[IdxRelay].reactions[ch].active_events[IdxEvent] = 0;
								}
             }
         }
    }
}
// Включает или выключает реакцию реле на событие конкретного канала

//relay_set_reaction(&modules[0], 1, 3, 2, 1);
// Модуль 0 > Реле 1 > Канал 3 > Событие 2 включено

void relay_set_reaction(RelayModule *module,
                        uint8_t relay_id,
                        uint8_t channel_id,
                        uint8_t event_id,
                        uint8_t enabled)
{
    if (!module) return;
    if (relay_id < 1 || relay_id > RELAY_COUNT) return;
    if (channel_id < 1 || channel_id > CHANNEL_COUNT) return;
    if (event_id < 1 || event_id > EVENTS_PER_CHANNEL) return;

    Relay *relay = &module->relays[relay_id - 1];
    RelayReaction *reaction = &relay->reactions[channel_id - 1];

    reaction->channel_id = channel_id;
    reaction->active_events[event_id - 1] = enabled;
}



// Обработка события
//process_event(&modules[0], 2, 1, relay_on);
// Проверяет модуль 0 > канал 2 > событие 1 > вызывает callback для реле, которое реагирует

void process_event(RelayModule *module,
                   uint8_t channel_id,
                   uint8_t event_id,
                   RelayAction action)
{
    if (!module) return;
    if (channel_id < 1 || channel_id > CHANNEL_COUNT) return;
    if (event_id < 1 || event_id > EVENTS_PER_CHANNEL) return;

    Channel *ch = &module->channels[channel_id - 1];
    Event *evt  = &ch->events[event_id - 1];

    RelayCommand cmd = evt->action;
    if (cmd == RELAY_CMD_NONE)
        return;   // событие не несёт команды > пропускаем

    for (int r = 0; r < RELAY_COUNT; r++)
    {
        Relay *relay = &module->relays[r];
        RelayReaction *reaction = &relay->reactions[channel_id - 1];

        if (reaction->active_events[event_id - 1])
        {
            action(module->module_id,
                   relay->relay_id,
                   cmd);
        }
    }
}


void relay_test_callback(uint8_t module_id,
                         uint8_t relay_id,
                         RelayCommand cmd)
{
    const char *cmd_text =
        (cmd == RELAY_CMD_ON)     ? "ON" :
        (cmd == RELAY_CMD_OFF)    ? "OFF" :
        (cmd == RELAY_CMD_TOGGLE) ? "TOGGLE" :
                                    "UNKNOWN";

    //printf("  [CALLBACK] Module %u > Relay %u command = %s\n",
   //        module_id, relay_id, cmd_text);
}

//// events_mask — битовая маска событий (бит 0 = EVENT_1, бит 1 = EVENT_2 и т.д.)
//void relay_set_reactions_bulk(RelayModule *module,
//                              uint8_t relay_id,
//                              uint8_t channel_id,
//                              uint8_t events_mask)
//{
//    if (!module) return;
//    if (relay_id < 1 || relay_id > RELAY_COUNT) return;
//    if (channel_id < 1 || channel_id > CHANNEL_COUNT) return;

//    RelayReaction *reaction = &module->relays[relay_id - 1].reactions[channel_id - 1];
//    reaction->channel_id = channel_id;

//    for (int i = 0; i < EVENTS_PER_CHANNEL; i++)
//	   {
//        reaction->active_events[i] = (events_mask >> i) & 1;
//		 }
//}


/*  
 ****************************   EXAMPLE for using *********************
int main(void)
{
    printf("=== Initializing modules ===\n");
    init_modules();

    printf("=== START MASS TEST ===\n\n");

    for (int m = 0; m < MODULE_COUNT; m++)
{
    RelayModule *module = &modules[m];
    printf("=== TEST MODULE %d ===\n", module->module_id);

    for (int r = 1; r <= RELAY_COUNT; r++)
    {
        printf("  > Relay %d:\n", r);

        for (int ch = 1; ch <= CHANNEL_COUNT; ch++)
        {
            for (int ev = 1; ev <= EVENTS_PER_CHANNEL; ev++)
            {
                printf("    - Testing Channel %d, Event %d...\n", ch, ev);

                relay_set_reaction(module, r, ch, ev, 1);  // включить реакцию на событие 

                modules[m].channels[ch - 1].events[ev - 1].action = RELAY_CMD_ON;
                process_event(module, ch, ev, relay_test_callback);

                modules[m].channels[ch - 1].events[ev - 1].action = RELAY_CMD_OFF;
                process_event(module, ch, ev, relay_test_callback);

                relay_set_reaction(module, r, ch, ev, 0);    // выключить реакцию на событие 
            }
        }
    }

    printf("\n=== MODULE %d TEST DONE ===\n\n", module->module_id);
}


    printf("=== ALL MODULES TESTED ===\n");
    return 0;
}


 *************************************************
*/
/************************ (C) COPYRIGHT  OnWert *****END OF FILE****/



