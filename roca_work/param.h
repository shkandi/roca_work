// Версии 
#define PROGRAM_VERSION 1.0 // Версия программы

// Значения по умолчанию
#define DEFAULT_COM 3 // COM порт по умолчанию
#define DEFAULT_ADR 0x00 // Адрес регистра по умолчанию
#define DEFAULT_DATA 0x00 // Данные по умолчанию
#define DEFAULT_NUM_BYTE 1 // Количество байт по умолчанию

// Коды ошибок программы
#define ROCA_WORK_ERR_NOERR 0 // Завершение без ошибок
#define ROCA_WORK_ERR_LOGFUL 1 // Нет свободных имен для файла лога
#define ROCA_WORK_ERR_FILEOPEN 2 // Не удалось открыть файл
#define ROCA_WORK_ERR_ID 3 // Ошибка id 

// Коды пакета
#define ROCA_CODE_READ 0x00 // Код чтения
#define ROCA_CODE_WRITE 0x01 // Код записи
#define ROCA_CODE_AND 0x02 // Код записи через AND
#define ROCA_CODE_OR 0x03 // Код записи через OR

// Номера состояний
#define STATE_UART_ID  2 // Состояние. Чтение из буфера
#define STATE_UART_READ  3 // Состояние. Чтение из буфера
#define STATE_UART_WRITE 4 // Состояние. Запись в буфер
#define STATE_UART_AND 5 // Состояние. Запись в буфер
#define STATE_UART_OR 6 // Состояние. Запись в буфер
#define STATE_UART_GETLOG 7 // Состояние. Получение лога в файл

// Разеные значения
#define MAX_BYTE_NUMBER 16 // Максимальное количество байт
#define MAX_LOG_ENTRIES 32 // Максимальное значение записей в логе


// Адреса
#define ROCA_ADR_ID 0x0000
#define ROCA_ADR_SHO 0x002e
#define ROCA_ADR_LOG 0x1000


// Значения
#define ROCA_DATA_ID 0xca

// Текст для лога
#define FTEXT_LOG_HEADER 1 // Текст заголовка
#define FTEXT_LOG_START 2 // Текст окончания
#define FTEXT_LOG_END 3 // Текст окончания

// Номер текста для функции выхода
#define TNX_PRM_ERR 1 // Текст ошибки параметра
#define TNX_ANO_TSK 2 // Текст двойного параметра задания
#define TNX_LIB_ERR 3 // Текст ошибка при работе с библиотекой
#define TNX_ERR_OCC 4 // Текст произошла ошибка
#define TNX_BUF_EMP 5 // Текст буфер пуст

// Параметры для печати числа pnum
#define PNUM_FLAG_BIN 0x1 // Двоичныая система
#define PNUM_FLAG_OCT 0x2 // Восьмеричная система
#define PNUM_FLAG_DEC 0x4 // Десятичная система
#define PNUM_FLAG_HEX 0x8 // Шеснадцатиричная система
#define PNUM_FLAG_PREFIX 0x10 // Флаг префикса
#define PNUM_FLAG_SPACE 0x20 // Флаг пробела
#define PNUM_FLAG_NEXTS 0x40 // Флаг перехода на новую строку
#define PNUM_VAR_CHAR 1 // char
#define PNUM_VAR_SHORT 2 // short
#define PNUM_VAR_INT 4 // int
#define PNUM_ERR_NOERR 0 // Завершение без ошибки
#define PNUM_ERR_TYPE 1 // Ошибка типа
#define PNUM_ERR_SYSTEM 2 // Ошибка выбора системы


// Флаги
#define FLAG_RISE 1 // Флаг поднят
#define FLAG_DOWN 0 // 0 Флаг сброшен

// Значения выхода из функции конфигурации
#define CONFIG_OK 0 // Успешное завершение конфигурации
#define CONFIG_OUT 1 // Выход из функции конфигурации без конфигурирования
#define CONFIG_ERR 1 // Выход из функции конфигурации с ошибкой

// Значения выхода из функции
#define ROUT_CONTINUE 1 // Продолжение 
#define ROUT_EXIT 0 // Выход 


// Рабочая структура
typedef struct{
	char state; // Состояние
	char com; // Номер COM порта
	char cmd; // Команда
	char numb; // Количество байт
	short adr; // Адрес буфера
	char data[16]; // Данные
}_ws;


