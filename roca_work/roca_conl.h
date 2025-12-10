// roca_conl.h заглавный файл для библиотеки roca_conl.dll

#ifndef _RCL_ROCA
#define _RCL_ROCA

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

// Возвращаемые коды функции rcl_uart
#define RCL_ERR_NOERR 0 // Работа успешно выполнена 
#define RCL_ERR_WRONG_COM 1 // Неправильный номер COM порта
#define RCL_ERR_WRONG_NB 2 // Неправильное количество байт
#define RCL_ERR_WRONG_CODE 3 // Неправильный код 
#define RCL_ERR_OPEN_COM 4 // Не удалось открыть COM порт
#define RCL_ERR_UART_SEND 5 // Не удалось отправить заданное количество байт по UART
#define RCL_ERR_UART_SILENT 6 // Не приходило никакого ответа по UART
#define RCL_ERR_UART_FIELD 7 // В принятом пакете ошибка в служебных полях
#define RCL_ERR_UART_CSUM 8 // В принятом пакете ошибка контрольной суммы 
#define RCL_ERR_UART_UNKN 9 // Принят неизвестный пакет
#define RCL_ERR_UART_ERROR 10 // Принят пакет с сообщением об ошибке MCU


// Функция rcl_version
float rcl_version(void);
// Функция возвращает версию библиоткеи



// Функция rcl_uart
int rcl_uart(unsigned char cp, char *arr);
// ## Описание
// Функция производит обмен данными с устройством по интерфейсу UART.
// Функция позводяет производить чтение или запись устройства
// Функция возвращает код резульатата (см. #define). 

#endif