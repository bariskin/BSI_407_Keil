#include "RelaySystem.h"

RelayModule modules[MODULE_COUNT];
uint16_t TOTAL_CHANNELS = 0;

// Таблица команд для 4 типов событий
// По умолчанию: ON, OFF, ON, OFF
static RelayCommand event_cmd[EVENT_COUNT] = {
    RELAY_CMD_ON,
    RELAY_CMD_ON,
    RELAY_CMD_ON,
    RELAY_CMD_ON
};

void set_event_command(EventType event_id, RelayCommand cmd) {
    if (event_id < 1 || event_id > EVENT_COUNT) return;
    event_cmd[event_id - 1] = cmd;
}

// ----------------------------------------------------------
// INIT
// ----------------------------------------------------------
void init_system(uint16_t total_channels)
{
    if (total_channels > MAX_CHANNELS)
        total_channels = MAX_CHANNELS;

    TOTAL_CHANNELS = total_channels;

    for (uint8_t m = 0; m < MODULE_COUNT; m++) {

        modules[m].module_id = m + 1;

        for (uint8_t r = 0; r < RELAY_PER_MODULE; r++) {

            modules[m].relays[r].relay_id = r + 1;

            for (uint8_t e = 0; e < EVENT_COUNT; e++) {
                for (uint16_t c = 0; c < TOTAL_CHANNELS; c++) {
                    modules[m].relays[r]
                        .reactions.reaction[e][c] = 0;
                }
            }
        }
    }
}

// ----------------------------------------------------------
// SET REACTION
// ----------------------------------------------------------
void set_reaction(uint8_t module_id,
                  uint8_t relay_id,
                  EventType event_id,
                  uint16_t channel_id,
                  uint8_t enable)
{
    if (module_id < 1 || module_id > MODULE_COUNT) return;
    if (relay_id < 1 || relay_id > RELAY_PER_MODULE) return;
    if (event_id < 1 || event_id > EVENT_COUNT) return;
    if (channel_id < 1 || channel_id > TOTAL_CHANNELS) return;

    modules[module_id - 1]
        .relays[relay_id - 1]
        .reactions
        .reaction[event_id - 1][channel_id - 1] = enable;
}

// ----------------------------------------------------------
// PROCESS EVENT
// ----------------------------------------------------------
void process_event(EventType event_id,
                   uint16_t channel_id,
                   RelayAction callback)
{
    if (!callback) return;
    if (event_id < 1 || event_id > EVENT_COUNT) return;
    if (channel_id < 1 || channel_id > TOTAL_CHANNELS) return;

    RelayCommand cmd = event_cmd[event_id - 1];

    for (uint8_t m = 0; m < MODULE_COUNT; m++) {
        for (uint8_t r = 0; r < RELAY_PER_MODULE; r++) {

            if (modules[m].relays[r]
                .reactions.reaction[event_id-1][channel_id-1])
            {
                callback(
                    modules[m].module_id,
                    modules[m].relays[r].relay_id,
                    cmd
                );
            }
        }
    }
}


