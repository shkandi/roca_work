#include "stdafx.h"
#include <stdlib.h>
#include "conio.h"
#include "time.h"
#include "roca_conl.h"
#include "param.h"

extern _ws ws, *p_ws; // Рабочая структура и указатель

// Получение системного времени
struct tm *ptm;
time_t timer, timer_n;

// Массив для работы
char ubuf[64];



// Для log файла 
FILE *fp;
char file_name[13] = {'e', 'v', 'l', 'o', 'g', '_', '0', '0', '.', 't', 'x', 't'};
char num_h[2] = {'0'};
char num_l[2] = {'0'};
int pn; // Номер файла протокола
int entries; // Количество записей всего
int read_adr; // Адрес для чтения

// Заглавия функций
void delay(int msec); // Интрвал
char tnx(char tn, char ret, char pnum, char *pstr); // Вывод текста и выход
char pnum(void *num, char var_tp, char flags, char raz); // Вывод числа
void log_text(char text); // Заголовок протокола
int sf_print(unsigned char *oper_b); // Печать расшифровки одной записи в файл
int se_print(unsigned char *oper_b, int *num, char bin_flag); // Печать расшифровки одной записи на экран



// Сама программа
int routine(_ws *str){
	
	int i, a;
	int res;
	char count;
	char rc_oper; 
	char arro[64];
	short int rs_oper;
		
	// Состояние общее
	// Состояние 0. Нет заданий
	switch(str->state){
	case 0:
		printf("No any task to do.\n");
		return ROUT_EXIT;

	// Состояние общее
	// Состояние 1. Ошибка конфигурации
	case 1:
		return ROUT_EXIT;
	
	
	// Состояние при работе по UART
	// Состояние. Запрос id
	/*
	case STATE_UART_ID:
		printf("Connection: UART\n");
		printf("COM port: %d\n", str->com);
		printf("Task is: Request id\n\n");
		str->adr = ROCA_ADR_ID; // Задаем адрес id
		str->numb = 1;
		if (res = rcl_uart(str->com, RCL_COM_READ, str->adr, str->numb + 4, dat_r))
			return tnx(TNX_LIB_ERR, ROUT_EXIT, res, 0);
		else {
			printf("Task compleat. Received id is:\n");
			printf("0x%x\n", (u8)dat_r[4]);
		}
		return ROUT_EXIT;
	*/
	
	// Состояние при работе по UART
	// Состояние. Чтение
	case STATE_UART_READ: 
		printf("Connection:UART\n");
		printf("COM:%d Task:Read ", str->com);
		printf("adr:");
		pnum((void*)&str->adr, PNUM_VAR_SHORT, PNUM_FLAG_HEX, 4);
		printf(" bytes:%d\n", str->numb);

		// Заполняем массив
		ubuf[0] = 6;
		ubuf[1] = 0;
		ubuf[2] = ROCA_CODE_READ;
		ubuf[3] = str->numb;
		*(short*)(ubuf + 4) = str->adr;
		
		// Отправляем и получаем
		if (res = rcl_uart(str->com, ubuf))
			return tnx(TNX_LIB_ERR, ROUT_EXIT, res, 0);
		else {
			printf("Task compleat. Received data is:\n");
			count = ubuf[3];
			for (i = 1; i < count + 1; i++){ 	
				if ((i + 8) % 8)
					res = pnum((void*)&ubuf[i + 6 - 1], PNUM_VAR_CHAR, PNUM_FLAG_HEX | PNUM_FLAG_SPACE, 2);
				else {
					res = pnum((void*)&ubuf[i + 6 - 1], PNUM_VAR_CHAR, PNUM_FLAG_HEX | PNUM_FLAG_NEXTS, 2);
				}
	
			}
			printf("\n");
		}
		return ROUT_EXIT;
	

	// Состояние при работе по UART
	// Состояние. Запись

	case STATE_UART_WRITE:
	case STATE_UART_AND:
	case STATE_UART_OR:
		printf("Connection:UART\n");
		printf("COM:%d Task:Write ", str->com);
		printf("adr:");
		pnum((void*)&str->adr, PNUM_VAR_SHORT, PNUM_FLAG_HEX, 4);
		printf(" bytes:%d method:", str->numb);
			if (str->cmd == STATE_UART_WRITE)
				printf("Over");
			else if (str->cmd == STATE_UART_AND)
				printf("AND");
			else if (str->cmd == STATE_UART_OR)
				printf("OR");
		
		printf("\n");
		printf("Data to write: ");
		res = pnum((void*)&str->data, PNUM_VAR_CHAR, PNUM_FLAG_HEX, 2);
		printf("\n");
		
		// Заполняем массив
		ubuf[0] = 6 + str->numb;
		ubuf[1] = 0;
		ubuf[2] = str->cmd;
		ubuf[3] = str->numb;
		*(short*)(ubuf + 4) = str->adr;
		
		for (i = 0; i < str->numb; i++)
			ubuf[6 + i] = str->data[i];
		
		if (res = rcl_uart(str->com, ubuf))
			return tnx(TNX_LIB_ERR, ROUT_EXIT, res, 0);
		else
			printf("Task compleat.\n");
		return ROUT_EXIT;
	

	// Состояние при работе по UART
	// Состояние. Получение лога и сохраниенеи в файл
	case STATE_UART_GETLOG: 
		printf("Connection:UART\n");
		printf("COM:%d Task:GetLog\n", str->com);

		// Читаем и проверяем id
		
		// Заполняем массив
		ubuf[0] = 6;
		ubuf[1] = 0;
		ubuf[2] = ROCA_CODE_READ;
		ubuf[3] = 1;
		*(short*)(ubuf + 4) = ROCA_ADR_ID;
		
		//ubuf[4] = str->adr;
		//ubuf[5] = str->adr >> 8;
		//str->adr = ROCA_ADR_ID; 
		
		
		if (res = rcl_uart(str->com, ubuf))
			return tnx(TNX_LIB_ERR, ROUT_EXIT, res, 0);
		
		if (ubuf[6] != (char)ROCA_DATA_ID) 
			return tnx(TNX_ERR_OCC, ROUT_EXIT, ROCA_WORK_ERR_ID, 0);
		
		// Определяем файлы в папке и выбираем имя файла-протокола
		// Определяем доступное название файла
		for (i = 99; i >= 0; i--){
			_itoa(i - i % 10, num_h, 10);
			_itoa(i % 10, num_l, 10);	
			file_name[6] = num_h[0];
			file_name[7] = num_l[0];
			fp = fopen(file_name, "r");
			if (i == 0 && fp == 0) // Если нет ни одного лог файла
				break;
			if (i == 99 && fp != 0){ // Если есть последний 99
				fclose(fp);
				return tnx(TNX_ERR_OCC, ROUT_EXIT, ROCA_WORK_ERR_LOGFUL, 0);
			}
			if (fp){ // Перебираем номера до свободного номера сверху
				fclose(fp);
				pn = i + 1;
				_itoa(pn - pn % 10, num_h, 10);
				_itoa(pn % 10, num_l, 10);	
				file_name[6] = num_h[0];
				file_name[7] = num_l[0];
				break;
			}
		}
		fp = fopen(file_name, "a"); // Открываем файл
		if (fp == 0)
			return tnx(TNX_ERR_OCC, ROUT_EXIT, ROCA_WORK_ERR_FILEOPEN, 0);
		
		log_text(FTEXT_LOG_HEADER);  // Записываем заголовок протокола
		
		entries = 64; // Выставляем количество записей
		read_adr = ROCA_ADR_LOG; // Выставляем начальный адрес
		str->numb = 16;
		
		for (a = 0; a < entries; a++){

			// Заполняем массив
			ubuf[0] = 6;
			ubuf[1] = 0;
			ubuf[2] = ROCA_CODE_READ;
			ubuf[3] = str->numb;
			*(short*)(ubuf + 4) = read_adr;
			
			// Получачем байты
			if (res = rcl_uart(str->com, ubuf)){
				fprintf(fp, "\nLibrary error: %x!\n", res); // Если ошибка, то запись в файле
				fclose(fp); // Закрываем файл
				return tnx(TNX_LIB_ERR, ROUT_EXIT, res, 0); // И выходим
			}
			// Счетчик полученных записей
			printf("Entries receicved: %d/%d\r", a + 1, entries);
			read_adr += str->numb;	// Добавляем количество байт к адресу

			// Работаем с полученными байтами
			sf_print((unsigned char*)&(ubuf[6]));  // Печатаем расшифровку записи в файл

		}
		
		log_text(FTEXT_LOG_END);  // Записываем окончание протокола
		fclose(fp); // Закрываем файл
		printf("\n");
		printf("Log received and saved to %s\n", file_name);
		printf("Task compleat.\n");
		return ROUT_EXIT;
		
	
	default:
		break;
	}	

	printf("\n\n");
	printf("Task compleat.\n");
	return ROUT_EXIT;
}


