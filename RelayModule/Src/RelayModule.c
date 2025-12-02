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
RelayModule modules[MODULE_COUNT];
/* ------------------------Functions-----------------------------------*/
/**
  * @brief  Инициализация всех модулей реле
  * @retval None
  */
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
		
		   // Пытаемся загрузить сохраненную конфигурацию
    if (!load_relay_configuration()) {
        // Конфигурация не загружена или повреждена
        // Можно здесь вызвать 
			   reset_relay_configuration();
    }
		
}
// Включает или выключает реакцию реле на событие конкретного канала

//relay_set_reaction(&modules[0], 1, 3, 2, 1);
// Модуль 0 > Реле 1 > Канал 3 > Событие 2 включено
/**
  * @brief  Установка реакции реле на событие канала
  * @param  module: указатель на модуль
  * @param  relay_id: ID реле (1-4)
  * @param  channel_id: ID канала (1-10)
  * @param  event_id: ID события (1-4)
  * @param  enabled: 1 - включить реакцию, 0 - выключить
  * @retval None
  */
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
	
	   // Автосохранение при изменении (опционально)
    // save_relay_configuration();
		
}

/**
  * @brief  Сброс всех реакций в модуле
  * @param  module: указатель на модуль
  * @retval None
  */
void reset_module_reactions(RelayModule *module)
{
    if (!module) return;
    
    for (uint8_t r = 0; r < RELAY_COUNT; r++)
    {
        for (uint8_t ch = 0; ch < CHANNEL_COUNT; ch++)
        {
            for (uint8_t ev = 0; ev < EVENTS_PER_CHANNEL; ev++)
            {
                module->relays[r].reactions[ch].active_events[ev] = 0;
            }
        }
    }
}
// Обработка события
//process_event(&modules[0], 2, 1, relay_on);
// Проверяет модуль 0 > канал 2 > событие 1 > вызывает callback для реле, которое реагирует
/**
  * @brief  Обработка события в канале
  * @param  module: указатель на модуль
  * @param  channel_id: ID канала (1-10)
  * @param  event_id: ID события (1-4)
  * @param  action: callback-функция для управления реле
  * @retval None
  */
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
/**
  * @brief  Тестовый callback для управления реле
  * @param  module_id: ID модуля (1-MODULE_COUNT)
  * @param  relay_id: ID реле (1-4)
  * @param  cmd: команда для реле
  * @retval None
  */

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

/**
  * @brief  Установка команд для реле из данных дисплея
  * @param  module: указатель на модуль
  * @param  data: массив данных с дисплея (5 байт)
  * @param  callback: функция обратного вызова для управления реле
  * @retval None
  */
void applyRelayCommandsFromDisplay(RelayModule *module, const uint8_t data[5], RelayAction callback) {
    
	
	  if (!module || !callback) return;

	
    uint16_t relayModuleCmdArry[4];
	
    parseRelayBytes(data, relayModuleCmdArry);

    // Пробегаем по реле и каналам
    for (int relay_idx = 0; relay_idx < RELAY_COUNT; relay_idx++) {
        Relay *relay = &module->relays[relay_idx];

        for (int ch = 0; ch < CHANNEL_COUNT; ch++) {
            RelayReaction *reaction = &relay->reactions[ch];

            // Проходим по событиям на канале
            for (int IdxEvent = 0; IdxEvent < EVENTS_PER_CHANNEL; IdxEvent++) {
                // Используем бит события из соответствующего short
							
                uint16_t mask;
                if (relay_idx < 4) mask = relayModuleCmdArry[relay_idx]; // 4 short = 4 реле
                else mask = 0; // если реле больше 4, пока отключаем

                uint8_t enabled = (mask >> IdxEvent) & 0x01;

                // Устанавливаем реакцию
                reaction->active_events[IdxEvent] = enabled;

                // Если событие включено, вызываем callback с командой
                if (enabled) {
                    RelayCommand cmd = module->channels[ch].events[IdxEvent].action;
                    if (cmd != RELAY_CMD_NONE) {
                        callback(module->module_id, relay->relay_id, cmd);
                    }
                }
            }
        }
    }
}


/**
  * @brief  Массовая установка реакций через битовую маску
  * @param  module: указатель на модуль
  * @param  relay_id: ID реле (1-4)
  * @param  channel_id: ID канала (1-10)
  * @param  events_mask: битовая маска событий (бит 0 = событие 1)
  * @retval None
  */
void relay_set_reactions_bulk(RelayModule *module,
                              uint8_t relay_id,
                              uint8_t channel_id,
                              uint8_t events_mask)
{
    if (!module) return;
    if (relay_id < 1 || relay_id > RELAY_COUNT) return;
    if (channel_id < 1 || channel_id > CHANNEL_COUNT) return;

    RelayReaction *reaction = &module->relays[relay_id - 1].reactions[channel_id - 1];
    reaction->channel_id = channel_id;

    for (int i = 0; i < EVENTS_PER_CHANNEL; i++)
	   {
       reaction->active_events[i] = (events_mask >> i) & 0x01;
		 }
}

/**
  * @brief  Получение состояния реакции реле на событие
  * @param  module: указатель на модуль
  * @param  relay_id: ID реле (1-4)
  * @param  channel_id: ID канала (1-10)
  * @param  event_id: ID события (1-4)
  * @retval 1 если реакция активна, 0 если неактивна, 0xFF при ошибке
  */
uint8_t get_relay_reaction_state(RelayModule *module,
                                 uint8_t relay_id,
                                 uint8_t channel_id,
                                 uint8_t event_id)
{
    if (!module) return 0xFF;
    if (relay_id < 1 || relay_id > RELAY_COUNT) return 0xFF;
    if (channel_id < 1 || channel_id > CHANNEL_COUNT) return 0xFF;
    if (event_id < 1 || event_id > EVENTS_PER_CHANNEL) return 0xFF;

    uint8_t relay_idx = relay_id - 1;
    uint8_t channel_idx = channel_id - 1;
    uint8_t event_idx = event_id - 1;
    

    return module->relays[relay_idx].reactions[channel_idx].active_events[event_idx];
}

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
/**
  * @brief  Установка команды для события канала
  * @param  module: указатель на модуль
  * @param  channel_id: ID канала (1-10)
  * @param  event_id: ID события (1-4)
  * @param  cmd: команда для реле
  * @retval None
  */
void set_channel_event_command(RelayModule *module,
                               uint8_t channel_id,
                               uint8_t event_id,
                               RelayCommand cmd)

{
    if (!module) return;
    if (channel_id < 1 || channel_id > CHANNEL_COUNT) return;
    if (event_id < 1 || event_id > EVENTS_PER_CHANNEL) return;

    uint8_t channel_idx = channel_id - 1;
    uint8_t event_idx = event_id - 1;
    
    module->channels[channel_idx].events[event_idx].action = cmd;
}

/**
  * @brief  Вычисление CRC32 конфигурации
  * @param  config: указатель на конфигурацию
  * @retval CRC32 значение
  */
static uint32_t calculate_config_crc(const RelayConfigStorage *config) {
    // Простая реализация CRC32 (можно заменить на аппаратную)
    uint32_t crc = 0xFFFFFFFF;
    uint8_t *data = (uint8_t*)config;
    uint32_t size = sizeof(RelayConfigStorage) - sizeof(uint32_t); // без поля crc
    
    for (uint32_t i = 0; i < size; i++) {
        crc ^= data[i];
        for (int j = 0; j < 8; j++) {
            if (crc & 1) {
                crc = (crc >> 1) ^ 0xEDB88320;
            } else {
                crc >>= 1;
           }
        }
    }
   
    return ~crc;
}

/**
  * @brief  Сохранение конфигурации всех модулей во Flash
  * @retval HAL status
  */
HAL_StatusTypeDef save_relay_configuration(void) {
	
    RelayConfigStorage config;
    
    // Заполняем магическое число
    config.magic = RELAY_CONFIG_MAGIC_NUMBER;
  // Сохраняем все реакции
    for (uint8_t mod = 0; mod < MODULE_COUNT; mod++) {
        for (uint8_t rel = 0; rel < RELAY_COUNT; rel++) {
            for (uint8_t ch = 0; ch < CHANNEL_COUNT; ch++) {
                for (uint8_t ev = 0; ev < EVENTS_PER_CHANNEL; ev++) {
                    config.reactions[mod][rel][ch][ev] = 
                        modules[mod].relays[rel].reactions[ch].active_events[ev];
                }
            }
        }
    }
    // Вычисляем и сохраняем CRC
    config.crc32 = calculate_config_crc(&config);
    
    // Сохраняем во Flash
    HAL_FLASH_Unlock();
    
    // Стираем сектор
    FLASH_Erase_Sector(RELAY_CONFIG_FLASH_SECTOR, VOLTAGE_RANGE_3);

		
		   // Записываем данные
    uint32_t *src = (uint32_t*)&config;
    uint32_t *dst = (uint32_t*)RELAY_CONFIG_FLASH_ADDRESS;
    uint32_t words = sizeof(RelayConfigStorage) / 4;
		
		    for (uint32_t i = 0; i < words; i++) {
        if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, 
                              (uint32_t)&dst[i], src[i]) != HAL_OK) {
            HAL_FLASH_Lock();
            return HAL_ERROR;
        }
    }
				
		    HAL_FLASH_Lock();
    return HAL_OK;
}


/**
  * @brief  Загрузка конфигурации из Flash
  * @retval 1 если успешно, 0 если конфигурация невалидна
  */

uint8_t load_relay_configuration(void) {
    RelayConfigStorage *config = (RelayConfigStorage*)RELAY_CONFIG_FLASH_ADDRESS;
    
    // Проверяем магическое число
    if (config->magic != RELAY_CONFIG_MAGIC_NUMBER) {
        return 0; // Нет сохраненной конфигурации
    }
		
		   // Проверяем CRC
    uint32_t stored_crc = config->crc32;
    uint32_t calculated_crc = calculate_config_crc(config);
    
    if (stored_crc != calculated_crc) {
        return 0; // Конфигурация повреждена
    }
   // Восстанавливаем реакции
    for (uint8_t mod = 0; mod < MODULE_COUNT; mod++) {
        for (uint8_t rel = 0; rel < RELAY_COUNT; rel++) {
            for (uint8_t ch = 0; ch < CHANNEL_COUNT; ch++) {
                for (uint8_t ev = 0; ev < EVENTS_PER_CHANNEL; ev++) {
                    modules[mod].relays[rel].reactions[ch].active_events[ev] = 
                        config->reactions[mod][rel][ch][ev];
                }
            }
        }
    }
    
    return 1; // Успешно загружено
} 


/**
  * @brief  Экспорт конфигурации в буфер (например, для передачи по UART)
  * @param  buffer: буфер для данных
  * @param  size: размер буфера
  * @retval Количество записанных байт
  */

uint16_t export_relay_configuration(uint8_t *buffer, uint16_t size) {
    if (size < MODULE_COUNT * RELAY_COUNT * CHANNEL_COUNT * EVENTS_PER_CHANNEL) {
        return 0;
    }
		
		
		    
    uint16_t index = 0;
    
    for (uint8_t mod = 0; mod < MODULE_COUNT; mod++) {
        for (uint8_t rel = 0; rel < RELAY_COUNT; rel++) {
            for (uint8_t ch = 0; ch < CHANNEL_COUNT; ch++) {
                for (uint8_t ev = 0; ev < EVENTS_PER_CHANNEL; ev++) {
                    buffer[index++] = 
                        modules[mod].relays[rel].reactions[ch].active_events[ev];
                }
            }
        }
    }
    
    return index;
}

/**
  * @brief  Импорт конфигурации из буфера
  * @param  buffer: буфер с данными
  * @param  size: размер данных
  * @retval 1 если успешно, 0 если ошибка
  */

uint8_t import_relay_configuration(const uint8_t *buffer, uint16_t size) {
    uint16_t expected_size = MODULE_COUNT * RELAY_COUNT * CHANNEL_COUNT * EVENTS_PER_CHANNEL;
    
    if (size != expected_size) {
        return 0;
    }
    uint16_t index = 0;
    
    for (uint8_t mod = 0; mod < MODULE_COUNT; mod++) {
        for (uint8_t rel = 0; rel < RELAY_COUNT; rel++) {
            for (uint8_t ch = 0; ch < CHANNEL_COUNT; ch++) {
                for (uint8_t ev = 0; ev < EVENTS_PER_CHANNEL; ev++) {
                    modules[mod].relays[rel].reactions[ch].active_events[ev] = 
                        buffer[index++];
                }
            }
        }
    }
		    // Сохраняем в Flash
    save_relay_configuration();
    
    return 1;
}
  
/**
  * @brief  Сброс конфигурации к значениям по умолчанию
  * @retval None
  */
void reset_relay_configuration(void) {
    for (uint8_t mod = 0; mod < MODULE_COUNT; mod++) {
        for (uint8_t rel = 0; rel < RELAY_COUNT; rel++) {
            for (uint8_t ch = 0; ch < CHANNEL_COUNT; ch++) {
                for (uint8_t ev = 0; ev < EVENTS_PER_CHANNEL; ev++) {
                    modules[mod].relays[rel].reactions[ch].active_events[ev] = 0;
                }
            }
        }
    // Очищаем Flash
    HAL_FLASH_Unlock();
    FLASH_Erase_Sector(RELAY_CONFIG_FLASH_SECTOR, VOLTAGE_RANGE_3);
    HAL_FLASH_Lock();
  }
}
/************************ (C) COPYRIGHT  OnWert *****END OF FILE****/



