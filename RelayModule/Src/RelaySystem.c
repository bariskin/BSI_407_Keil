#include "stm32f4xx_hal.h"
#include "RelaySystem.h"
#include <string.h>
#include "DisplayDriver.h"

uint16_t TOTAL_CHANNELS = 0;

RelayModule modules[MODULE_COUNT];
volatile uint8_t config_dirty = 0;

typedef struct {
    uint32_t magic;
    RelayModule modules[MODULE_COUNT];
} RelayModulesFlash;

// Таблица команд для 4 типов событий
// По умолчанию: ON, OFF, ON, OFF
static RelayCommand event_cmd[EVENT_COUNT] = {
    RELAY_CMD_ON,
    RELAY_CMD_ON,
    RELAY_CMD_ON,
    RELAY_CMD_ON,
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



void relay_modules_flash_save(void)
{
    RelayModulesFlash data;

    data.magic = MODULES_FLASH_MAGIC;
    memcpy(data.modules, modules, sizeof(modules));

    HAL_FLASH_Unlock();

    FLASH_EraseInitTypeDef erase;
    uint32_t error;

    erase.TypeErase    = FLASH_TYPEERASE_SECTORS;
    erase.Sector       = FLASH_MODULES_SECTOR;
    erase.NbSectors    = 1;
    erase.VoltageRange = FLASH_VOLTAGE_RANGE_3;

    HAL_FLASHEx_Erase(&erase, &error);

    uint32_t addr = FLASH_MODULES_ADDRESS;
    uint32_t *p = (uint32_t *)&data;
    uint32_t words = (sizeof(RelayModulesFlash) + 3) / 4;

    for (uint32_t i = 0; i < words; i++) {
        HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, addr, p[i]);
        addr += 4;
    }

    HAL_FLASH_Lock();
}


void relay_modules_flash_load(void)
{
    RelayModulesFlash *data =
        (RelayModulesFlash *)FLASH_MODULES_ADDRESS;

    if (data->magic != MODULES_FLASH_MAGIC)
        return; // данных нет

    memcpy(modules, data->modules, sizeof(modules));
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



// Предполагается, что EventType и set_reaction определены ранее
// CHANNEL_BLOCK_SIZE и TOTAL_CHANNELS тоже должны быть определены

//void apply_relay_event_block_bits(RelaysEventBlock_t *cmd)
//{
//    if (!cmd) return; // защита от NULL

//    for (uint8_t e = 0; e < cmd->event_count; e++) {
//        EventType event = cmd->events[e].event_id;
//        uint16_t mask   = cmd->events[e].mask;

//        for (uint8_t i = 0; i < CHANNEL_BLOCK_SIZE; i++) {
//            // если бит i не установлен, пропускаем канал
//            if (!(mask & (1 << i))) continue;

//            uint16_t ch = cmd->channel_id + i;

//            // проверка на переполнение TOTAL_CHANNELS
//            if (ch > TOTAL_CHANNELS) break;

//            // записываем реакцию в матрицу
//            set_reaction(cmd->module_id, cmd->relays_id, event, ch, 1);
//        }
//    }
//}

