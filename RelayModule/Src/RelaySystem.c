
#include <string.h>
#include <stdint.h>
#include "stm32f4xx_hal.h"
#include "DisplayDriver.h"
#include "RelaySystem.h"
#include "cmsis_os.h"
#include "at24cm01.h"

volatile uint8_t config_dirty = 0;
uint16_t TOTAL_CHANNELS = 0;

RelayModule modules[RELAY_MODULE_COUNT];

// Таблица команд для 5 типов событий
static RelayCommand event_cmd[EVENT_COUNT] = {
    RELAY_CMD_ON,   // POROG_1
    RELAY_CMD_ON,   // POROG_2
    RELAY_CMD_ON,   // POROG_3
    RELAY_CMD_ON,   // ERROR_485
    RELAY_CMD_OFF   // POROG_NORMAL 
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

    for (uint8_t m = 0; m < RELAY_MODULE_COUNT; m++) {

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
	  if (event_id == EVENT_POROG_NORMAL) return;
	
    if (module_id < 1 || module_id > RELAY_MODULE_COUNT) return;
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

    for (uint8_t m = 0; m < RELAY_MODULE_COUNT; m++) {
        for (uint8_t r = 0; r < RELAY_PER_MODULE; r++) {
            
            if (event_id == EVENT_POROG_NORMAL) {
                // Ищем реле, привязанное к этому каналу
                // Проверяем пороговые события (1-3)
                if (modules[m].relays[r].reactions.reaction[EVENT_POROG_1-1][channel_id-1] ||
                    modules[m].relays[r].reactions.reaction[EVENT_POROG_2-1][channel_id-1] ||
                    modules[m].relays[r].reactions.reaction[EVENT_POROG_3-1][channel_id-1]) {
                    
                    // Нашли реле для этого канала - выключаем
                    callback(modules[m].module_id, modules[m].relays[r].relay_id, RELAY_CMD_OFF);
                }
            }
            else {
                // Обычное событие
                RelayCommand cmd = event_cmd[event_id - 1];
                if (modules[m].relays[r].reactions.reaction[event_id-1][channel_id-1]) {
                    callback(modules[m].module_id, modules[m].relays[r].relay_id, cmd);
                }
            }
        }
    }
}

void apply_relay_event_block_bits(RelaysEvent_t *cmd)
{
    if (!cmd) return;

    uint16_t ch_start = cmd->channel_id;

    for (uint8_t i = 0; i < CHANNEL_BLOCK_SIZE; i++) {
        uint16_t ch = ch_start + i;
        if (ch > TOTAL_CHANNELS) break;

        set_reaction(cmd->module_id, cmd->relays_id,
                     EVENT_POROG_1, ch,
                     (cmd->warning & (1 << i)) ? 1 : 0);

        set_reaction(cmd->module_id, cmd->relays_id,
                     EVENT_POROG_2, ch,
                     (cmd->alarm_1 & (1 << i)) ? 1 : 0);

        set_reaction(cmd->module_id, cmd->relays_id,
                     EVENT_POROG_3, ch,
                     (cmd->alarm_2 & (1 << i)) ? 1 : 0);

        set_reaction(cmd->module_id, cmd->relays_id,
                     EVENT_ERROR_485, ch,
                     (cmd->error & (1 << i)) ? 1 : 0);
    }
}

// CRC32 простая
uint32_t crc32_simple(const uint8_t *data, uint32_t len)
{
    uint32_t crc = 0xFFFFFFFF;
    for (uint32_t i = 0; i < len; i++) {
        crc ^= data[i];
        for (uint8_t b = 0; b < 8; b++)
            crc = (crc & 1) ? (crc >> 1) ^ 0xEDB88320 : (crc >> 1);
    }
    return ~crc;
}
RelaysEvent_t events[128];

void relay_modules_flash_save(void)
{
    //RelaysEvent_t events[128];
    uint16_t count = 0;

    for (uint8_t m = 0; m < RELAY_MODULE_COUNT; m++) {
        for (uint8_t r = 0; r < RELAY_PER_MODULE; r++) {

            for (uint16_t ch = 0; ch < TOTAL_CHANNELS; ch += CHANNEL_BLOCK_SIZE) {

                if (count >= 128) break;

                uint16_t w = 0, a1 = 0, a2 = 0, e = 0;

                for (uint8_t i = 0; i < CHANNEL_BLOCK_SIZE; i++) {
                    uint16_t idx = ch + i;
                    if (idx >= TOTAL_CHANNELS) break;

                    if (modules[m].relays[r].reactions.reaction[EVENT_POROG_1 - 1][idx])
                        w |= (1 << i);
                    if (modules[m].relays[r].reactions.reaction[EVENT_POROG_2 - 1][idx])
                        a1 |= (1 << i);
                     if (modules[m].relays[r].reactions.reaction[EVENT_POROG_3 - 1][idx])
                        a2 |= (1 << i);
                    if (modules[m].relays[r].reactions.reaction[EVENT_ERROR_485 -1][idx])
                        e |= (1 << i);
                }

                if (w || a1 || a2 || e) {
                    events[count++] = (RelaysEvent_t){
                        .module_id  = m,
                        .relays_id  = r,
                        .channel_id = ch,
                        .warning    = w,
                        .alarm_1    = a1,
                        .alarm_2    = a2,
                        .error      = e
                    };
                }
            }
        }
    }

    RelaysStorageHeader_t hdr;
    hdr.magic   = MODULES_FLASH_MAGIC;
    hdr.version = 1;
    hdr.count   = count;
    hdr.crc     = crc32_simple((uint8_t*)events, count * sizeof(RelaysEvent_t));

    AT24_Write(EXTERN_EEPROM_ADDR, (uint8_t*)&hdr, sizeof(hdr));
    if (count > 0)
        AT24_Write(sizeof(hdr), (uint8_t*)events, count * sizeof(RelaysEvent_t));
}
