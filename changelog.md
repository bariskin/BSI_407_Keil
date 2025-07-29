
## 2025-07-28

### changed:

####   #define TIME_DEFAULT_1       60
####	 #define TIME_STEP_MIN_2      40
####   #define TIME_STEP_DEFAULT_2  60
#### NUMBER_SLAVE_DEVICES           50

## 2025-07-27

### Added:

#### цикл определения активных приборов 

#### функция получения адреса активного прибора на линии

#### NUMBER_SLAVE_DEVICES 100

#### минимальное время считывания приборов 80 ms, определенно опытным путем

#### адреса для slave части, для обмена с компьютером:
   ##### BaudRate 40001
   ##### Parity   40002
   ##### StopBits 40003
   ##### Add      40004
   ##### TimeStep 40005


## 2025-07-26

### Added:

#### Modbus Master: обработка конечных Slave устройств согласно последнему описанию регистров. Количество устройств устанавливается по define NUMBER_SLAVE_DEVICES.

#### Modbus Slave: настройка скорости обмена и адреса устройства c верхнего уровня. 

#### default Modbus Slave: 1 