/**
******************************************************************************
* @file      RelaySystem.c
* @author    OnWert
* @brief     This file provides common functions for the Relay System           
*/  
/* ------------------------Includes ----------------------------------*/
#include <string.h>
#include <stdint.h>
#include "stm32f4xx_hal.h"
#include "DisplayDriver.h"
#include "RelaySystem.h"
#include "cmsis_os.h"
#include "at24cm01.h"
/* ------------------------External variables -------------------------*/
/* ------------------------Global variables----------------------------*/
uint16_t TOTAL_CHANNELS = 0;
RelayModule modules[RELAY_MODULE_COUNT];

/* ------------------------Locale variables----------------------------*/
// Command table for 5 event types
static RelayCommand event_cmd[EVENT_COUNT] = {
    RELAY_CMD_ON,   // POROG_1
    RELAY_CMD_ON,   // POROG_2
    RELAY_CMD_ON,   // POROG_3
    RELAY_CMD_ON,   // ERROR_485
    RELAY_CMD_OFF   // POROG_NORMAL 
};
/* ------------------------Functions-----------------------------------*/

/**
  * @brief  Initialize the relay system with specified number of channels
  * @param  total_channels: Number of channels to support (max MAX_CHANNELS)
  * @retval None
  * @note   Initializes all modules, relays and clears all reaction configurations
  */
void init_system(uint16_t total_channels)
{
    if (total_channels > MAX_CHANNELS)
        total_channels = MAX_CHANNELS;

    TOTAL_CHANNELS = total_channels;

    for (uint8_t m = 0; m < RELAY_MODULE_COUNT; m++) {

        modules[m].module_id = m + 1;

        for (uint8_t r = 0; r < RELAY_PER_MODULE; r++) {

            modules[m].relays[r].relay_id = r + 1;

            // Clear all reaction configurations
            for (uint8_t e = 0; e < EVENT_COUNT; e++) {
                for (uint16_t c = 0; c < TOTAL_CHANNELS; c++) {
                    modules[m].relays[r]
                        .reactions.reaction[e][c] = 0;
                }
            }
        }
    }
}

/**
  * @brief  Configure relay reaction for specific event and channel
  * @param  module_id:  Module identifier (1..RELAY_MODULE_COUNT)
  * @param  relay_id:   Relay identifier within module (1..RELAY_PER_MODULE)
  * @param  event_id:   Event type (EVENT_POROG_1..EVENT_ERROR_485)
  * @param  channel_id: Channel identifier (1..TOTAL_CHANNELS)
  * @param  enable:     Enable (1) or disable (0) the reaction
  * @retval None
  * @note   EVENT_POROG_NORMAL reactions cannot be configured (service event)
  */
void set_reaction(uint8_t module_id,
                  uint8_t relay_id,
                  EventType event_id,
                  uint16_t channel_id,
                  uint8_t enable)
{
    // EVENT_POROG_NORMAL is a service event, reactions are not configurable
    if (event_id == EVENT_POROG_NORMAL) return;
    
    // Validate input parameters
    if (module_id < 1 || module_id > RELAY_MODULE_COUNT) return;
    if (relay_id < 1 || relay_id > RELAY_PER_MODULE) return;
    if (event_id < 1 || event_id > EVENT_COUNT) return;
    if (channel_id < 1 || channel_id > TOTAL_CHANNELS) return;

    // Set reaction configuration
    modules[module_id - 1]
        .relays[relay_id - 1]
        .reactions
        .reaction[event_id - 1][channel_id - 1] = enable;
}

/**
  * @brief  Process incoming event and trigger configured relay actions
  * @param  event_id:   Event type to process
  * @param  channel_id: Channel where event occurred
  * @param  callback:   Callback function to execute relay commands
  * @retval None
  * @note   For EVENT_POROG_NORMAL: turns OFF the relay configured for this channel
  * @note   For other events: executes command from event_cmd table if reaction is configured
  */
void process_event(EventType event_id,
                   uint16_t channel_id,
                   RelayAction callback)
{
    // Validate parameters
    if (!callback) return;
    if (event_id < 1 || event_id > EVENT_COUNT) return;
    if (channel_id < 1 || channel_id > TOTAL_CHANNELS) return;

    // Iterate through all modules and relays
    for (uint8_t m = 0; m < RELAY_MODULE_COUNT; m++) {
        for (uint8_t r = 0; r < RELAY_PER_MODULE; r++) {
            
            if (event_id == EVENT_POROG_NORMAL) {
                // Service event: find relay configured for this channel
                // Check threshold events (1-3) for this channel
                if (modules[m].relays[r].reactions.reaction[EVENT_POROG_1-1][channel_id-1] ||
                    modules[m].relays[r].reactions.reaction[EVENT_POROG_2-1][channel_id-1] ||
                    modules[m].relays[r].reactions.reaction[EVENT_POROG_3-1][channel_id-1]) {
                    
                    // Found relay for this channel - turn it OFF
                    callback(modules[m].module_id, modules[m].relays[r].relay_id, RELAY_CMD_OFF);
                }
            }
            else {
                // Regular event: execute if reaction is configured
                RelayCommand cmd = event_cmd[event_id - 1];
                if (modules[m].relays[r].reactions.reaction[event_id-1][channel_id-1]) {
                    callback(modules[m].module_id, modules[m].relays[r].relay_id, cmd);
                }
            }
        }
    }
}

/**
  * @brief  Set command for specific event type
  * @param  event_id: Event type (EVENT_POROG_1..EVENT_POROG_NORMAL)
  * @param  cmd:      Command to execute for this event (RELAY_CMD_ON/OFF)
  * @retval None
  */
void set_event_command(EventType event_id, RelayCommand cmd) {
    if (event_id < 1 || event_id > EVENT_COUNT) return;
    event_cmd[event_id - 1] = cmd;
}

/**
  * @brief  Configure multiple channel reactions from packed bit field
  * @param  cmd: Pointer to RelaysEvent_t structure containing:
  *         - module_id: Target module
  *         - relays_id: Target relay within module
  *         - channel_id: Starting channel
  *         - warning:   Bit field for EVENT_POROG_1 reactions
  *         - alarm_1:   Bit field for EVENT_POROG_2 reactions
  *         - alarm_2:   Bit field for EVENT_POROG_3 reactions
  *         - error:     Bit field for EVENT_ERROR_485 reactions
  * @retval None
  * @note   Configures CHANNEL_BLOCK_SIZE (10) channels starting from channel_id
  */
void apply_relay_event_block_bits(RelaysEvent_t *cmd)
{
    if (!cmd) return;

    uint16_t ch_start = cmd->channel_id;

    // Configure reactions for a block of channels
    for (uint8_t i = 0; i < CHANNEL_BLOCK_SIZE; i++) {
        uint16_t ch = ch_start + i;
        if (ch > TOTAL_CHANNELS) break;

        // Set EVENT_POROG_1 reaction based on warning bit field
        set_reaction(cmd->module_id, cmd->relays_id,
                     EVENT_POROG_1, ch,
                     (cmd->warning & (1 << i)) ? 1 : 0);

        // Set EVENT_POROG_2 reaction based on alarm_1 bit field
        set_reaction(cmd->module_id, cmd->relays_id,
                     EVENT_POROG_2, ch,
                     (cmd->alarm_1 & (1 << i)) ? 1 : 0);

        // Set EVENT_POROG_3 reaction based on alarm_2 bit field
        set_reaction(cmd->module_id, cmd->relays_id,
                     EVENT_POROG_3, ch,
                     (cmd->alarm_2 & (1 << i)) ? 1 : 0);

        // Set EVENT_ERROR_485 reaction based on error bit field
        set_reaction(cmd->module_id, cmd->relays_id,
                     EVENT_ERROR_485, ch,
                     (cmd->error & (1 << i)) ? 1 : 0);
    }
}
/************************ (C) COPYRIGHT  OnWert *****END OF FILE****/
