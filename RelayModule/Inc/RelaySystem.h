/**
******************************************************************************
* @file      RelaySystem.h
* @author    OnWert
* @brief     This file contains defines and all the functions prototypes for the RelaySystem.c
*/
#ifndef RELAY_SYSTEM_H
#define RELAY_SYSTEM_H

#ifdef __cplusplus
extern "C" {
#endif

/* ------------------------Includes ----------------------------------*/
#include <stdint.h>
#include "DisplayDriver.h"
#include "at24cm01.h" 

/**
  * @defgroup relay_system Relay System Module
  * @brief Relay control and event handling system
  * 
  * This module provides functionality for configuring and controlling relay modules
  * based on system events and channel states. Supports multiple relay modules
  * with configurable reactions to different event types.
  */
  
/**
  * @}
  */

/* ------------------------Defines ----------------------------------*/

/** @defgroup relay_system_config Configuration Constants
  * @{
  */

/** Maximum number of relay modules in the system */
#define RELAY_MODULE_COUNT  4      /**< Can be increased (8, 16, 32...) */

/** Number of relays per module */
#define RELAY_PER_MODULE    4      /**< Relays per single module */

/** Number of supported event types */
#define EVENT_COUNT         5

/** Maximum number of channels supported by the system */
#define MAX_CHANNELS        40

/** Magic word for flash memory validation */
#define MODULES_FLASH_MAGIC    0xDEADBEEF /**< Identifies valid relay configuration in memory */

/** Block size for channel configuration from display */
#define CHANNEL_BLOCK_SIZE     10         /**< Information size for 10 channels from display */

/**
  * @}
  */

/* ------------------------Data Types ----------------------------------*/

/** @defgroup relay_system_types Data Types
  * @{
  */

/**
  * @brief Event message structure for queue communication
  */
typedef struct {
    uint8_t event_id;      /**< Event type identifier */
    uint16_t channel_id;   /**< Channel where event occurred */
} EventMessage_t;

/**
  * @brief Relay command enumeration
  */
typedef enum {
    RELAY_CMD_OFF = 0,     /**< Turn relay OFF */
    RELAY_CMD_ON  = 1      /**< Turn relay ON */
} RelayCommand;

/**
  * @brief Event type enumeration
  * @note EVENT_POROG_NORMAL is a service event that cannot be configured
  */
typedef enum {
    EVENT_POROG_1 = 1,     /**< First threshold event */
    EVENT_POROG_2,         /**< Second threshold event */
    EVENT_POROG_3,         /**< Third threshold event */
    EVENT_ERROR_485,       /**< RS-485 communication error event */
    EVENT_POROG_NORMAL     /**< Service event - returns system to normal state (reactions not stored in memory) */
} EventType;

/**
  * @brief Relay action callback function type
  * @param module_id Module identifier
  * @param relay_id Relay identifier within module
  * @param cmd Command to execute (ON/OFF)
  */
typedef void (*RelayAction)(uint8_t module_id, uint8_t relay_id, RelayCommand cmd);

/**
  * @brief Relay reaction configuration structure
  * @note 3D array: [EVENT_COUNT][MAX_CHANNELS]
  *       Defines which events trigger which relays on which channels
  */
typedef struct {
    uint8_t reaction[EVENT_COUNT][MAX_CHANNELS]; /**< Reaction matrix: event × channel → enabled */
} RelayReaction;

/**
  * @brief Individual relay structure
  */
typedef struct {
    uint8_t relay_id;          /**< Relay identifier (1..RELAY_PER_MODULE) */
    RelayReaction reactions;   /**< Reaction configuration for this relay */
} Relay;

/**
  * @brief Relay module structure
  */
typedef struct {
    uint8_t module_id;                     /**< Module identifier (1..RELAY_MODULE_COUNT) */
    Relay relays[RELAY_PER_MODULE];        /**< Array of relays in this module */
} RelayModule;

/**
  * @brief Flash storage header for relay configuration
  */
typedef struct {
    uint32_t magic;     /**< Validation magic word (MODULES_FLASH_MAGIC) */
    uint16_t version;   /**< Configuration version */
    uint16_t count;     /**< Number of modules stored */
    uint32_t crc;       /**< CRC32 checksum for data validation */
} RelaysStorageHeader_t;

/**
  * @}
  */

/* ------------------------Function Prototypes ----------------------------------*/

/** @defgroup relay_system_functions Public Functions
  * @{
  */

/**
  * @brief  Initialize the relay system
  * @param  total_channels Number of channels to support (limited by MAX_CHANNELS)
  * @retval None
  * @note   Must be called before any other relay system functions
  */
void init_system(uint16_t total_channels);

/**
  * @brief  Configure relay reaction for specific event and channel
  * @param  module_id  Module identifier (1..RELAY_MODULE_COUNT)
  * @param  relay_id   Relay identifier within module (1..RELAY_PER_MODULE)
  * @param  event_id   Event type (EVENT_POROG_1..EVENT_ERROR_485)
  * @param  channel_id Channel identifier (1..total_channels)
  * @param  enable     Enable (1) or disable (0) the reaction
  * @retval None
  * @note   EVENT_POROG_NORMAL reactions cannot be configured
  */
void set_reaction(uint8_t module_id,
                  uint8_t relay_id,
                  EventType event_id,
                  uint16_t channel_id,
                  uint8_t enable);

/**
  * @brief  Process incoming event and trigger configured relay actions
  * @param  event_id   Event type to process
  * @param  channel_id Channel where event occurred
  * @param  callback   Callback function to execute relay commands
  * @retval None
  * @note   For EVENT_POROG_NORMAL: turns OFF the relay configured for this channel
  * @note   For other events: executes command from internal table if reaction is configured
  */
void process_event(EventType event_id,
                   uint16_t channel_id,
                   RelayAction callback);

/**
  * @brief  Set default command for specific event type
  * @param  event_id Event type (EVENT_POROG_1..EVENT_POROG_NORMAL)
  * @param  cmd      Command to execute for this event (RELAY_CMD_ON/OFF)
  * @retval None
  */
void set_event_command(EventType event_id, RelayCommand cmd);

/**
  * @brief  Configure multiple channel reactions from packed bit field
  * @param  cmd Pointer to RelaysEvent_t structure containing:
  *         - module_id: Target module identifier
  *         - relays_id: Target relay identifier
  *         - channel_id: Starting channel for configuration block
  *         - warning:   Bit field for EVENT_POROG_1 reactions (10 bits)
  *         - alarm_1:   Bit field for EVENT_POROG_2 reactions (10 bits)
  *         - alarm_2:   Bit field for EVENT_POROG_3 reactions (10 bits)
  *         - error:     Bit field for EVENT_ERROR_485 reactions (10 bits)
  * @retval None
  * @note   Configures CHANNEL_BLOCK_SIZE (10) consecutive channels
  * @note   Typically used for display-driven configuration
  */
void apply_relay_event_block_bits(RelaysEvent_t *cmd);

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* RELAY_SYSTEM_H */

