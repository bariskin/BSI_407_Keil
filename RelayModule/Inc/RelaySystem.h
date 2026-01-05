#ifndef RELAY_SYSTEM_H
#define RELAY_SYSTEM_H

#include <stdint.h>
#include "DisplayDriver.h"


#define MODULE_COUNT        4      // Можно увеличивать (8, 16, 32…)
#define RELAY_PER_MODULE    4      // количество реле на один модуль
#define EVENT_COUNT         4
#define MAX_CHANNELS        48    


#define MODULES_FLASH_MAGIC    0xDEADBEEF // магическое слова в памяти, оно определяет наличение настроек для модулей релей
#define CHANNEL_BLOCK_SIZE     10         // Размер информации с дисплея для 10 каналов 

typedef struct {
    uint8_t event_id;
    uint16_t channel_id;
} EventMessage_t;

typedef enum {
    RELAY_CMD_OFF = 0,
    RELAY_CMD_ON  = 1
} RelayCommand;

// ------------------------------------------------------------------
// Типы событий
// ------------------------------------------------------------------
typedef enum {
    EVENT_POROG_1 = 1, 
    EVENT_POROG_2,        
    EVENT_POROG_3,
	  EVENT_ERROR_485,
	  EVENT_POROG_NORMAL
} EventType;


typedef struct {
    uint16_t event_id;
    uint16_t mask;  // какие каналы активны
} RelayEventMask_t;

typedef struct {
    uint16_t module_id;
    uint16_t relays_id;
    uint16_t channel_id;  // первый канал блока
    uint8_t  event_count;
    RelayEventMask_t events[];
} RelaysEventBlock_t;

// Callback тип: (module_id, relay_id, cmd)
typedef void (*RelayAction)(uint8_t, uint8_t, RelayCommand);

typedef struct {
    uint8_t reaction[EVENT_COUNT][MAX_CHANNELS];
} RelayReaction;

typedef struct {
    uint8_t relay_id;
    RelayReaction reactions;
} Relay;

typedef struct {
    uint8_t module_id;
    Relay relays[RELAY_PER_MODULE];
} RelayModule;

extern RelayModule modules[MODULE_COUNT];
extern uint16_t TOTAL_CHANNELS;

// ИНИЦИАЛИЗАЦИЯ
void init_system(uint16_t total_channels);

// НАСТРОЙКА РЕАКЦИЙ
void set_reaction(uint8_t module_id,
                  uint8_t relay_id,
                  EventType event_id,
                  uint16_t channel_id,
                  uint8_t enable);

// ОБРАБОТКА СОБЫТИЯ
void process_event(EventType event_id,
                   uint16_t channel_id,
                   RelayAction callback);

// Установка таблицы команд для 4 событий
void set_event_command(EventType event_id, RelayCommand cmd);
// Callback тип: (module_id, relay_id, cmd)
void relay_modules_flash_load(void);
void apply_relay_event_block_bits(RelaysEvent_t *cmd);
void relay_modules_flash_save(void);
#endif // RELAY_SYSTEM_H
