#ifndef RELAY_SYSTEM_H
#define RELAY_SYSTEM_H

#include <stdint.h>

#define MODULE_COUNT        6       // Можно увеличивать (8, 16, 32…)
#define RELAY_PER_MODULE    4
#define EVENT_COUNT         5
#define MAX_CHANNELS        32     // Можно ставить 512 или 1024


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
    EVENT_MODULE4_ON = 1, // для релейного модуля 4, ON
    EVENT_POROG_2,        // средний второй  порог 
    EVENT_MODULE4_OFF,    // для релейного модуля 4, OFF
    EVENT_ERROR_485,
	  EVENT_POROG_NORMAL
} EventType;

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

#endif // RELAY_SYSTEM_H
