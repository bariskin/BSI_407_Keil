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
#include "DisplayDriver.h"
/* ------------------------External variables -------------------------*/

/* ------------------------Global variables----------------------------*/

/* ------------------------Locale variables----------------------------*/
/* ------------------------Functions-----------------------------------*/








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
*/



/************************ (C) COPYRIGHT  OnWert *****END OF FILE****/



