#include "stdafx.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "conio.h"

#include "roca_conl.h"
#include "param.h"

// Массивы с текстом параметров для сравнения
char nb_prm[3] = {'-', 'n', 'b'}; // -nb 
char c_prm[2] = {'-', 'c'}; // -c 
char ugl_prm[5] = {'-', 'u', 'g', 'l', 0}; // -ugl 
char ubr_prm[4] = {'-', 'u', 'b', 'r'}; // -ubr
char ubw_prm[4] = {'-', 'u', 'b', 'w'}; // -ubw
char uba_prm[4] = {'-', 'u', 'b', 'a'}; // -uba
char ubo_prm[4] = {'-', 'u', 'b', 'o'}; // -ubo

// Массивы с символами
char com_test[7] = {'2', '3', '4', '5', '6', '7', '8'}; // Проверка COM порта
char hs_test[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'}; // Проверка hex числа
char dec_test[10] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'}; // Проверка dec числа

// Массивы для работы
char numc[2]; // Буфер для atoi конвертирования
char hs_buf[16]; // Буфер для strtol конвертирования

// Заглавия функций 
char tnx(char tn, char ret, char pnum, char *pstr); // Вывод текста и выход
char buf_cmp(char *base, char *target, char count); // Функция сравнения буферов



// Конфигурация рабочей структуры по умолчанию
void str_clar(_ws *str){
	int i;
	
	str->com = DEFAULT_COM; // Номер COM порта
	str->adr = DEFAULT_ADR; // Адрес
	
	for (i = 0; i < 16; i++)
		str->data[i] = DEFAULT_DATA; // Данные
	
	str->numb = DEFAULT_NUM_BYTE; // Количество байт
	str->state = 0; // Состояние
	return;
}



// Конфигурация рабочей структуры по параметрам
int str_conf(_ws *str, int n_param, char *param[]){
	
	int i, t, f, nd, g;
	char task_flag;
	
	task_flag = 0;

	// Проверяем все параметры по очереди
	for (i = 1; i < n_param; i ++){

		// Проверяем признак номера COM порта -c
		if (buf_cmp(c_prm, param[i], 2)){
			if (param[i][3]) // Проверяем что после значения COM порта ничего нет 
				return tnx(TNX_PRM_ERR, CONFIG_ERR, 0, param[i]);
			if (param[i][2] != 0){ // Проверяем что есть значение
				for (t = 0; t < 7; t++){ // Проверяем содержание разряда
					if (param[i][2] == com_test[t]) // Проверяем содержание
						break;
					if (t == 6) // Если не подошло ни одно значение
						return tnx(TNX_PRM_ERR, CONFIG_ERR, 0, param[i]); // То выходим с ошибкой
				}
			}
			else
				return tnx(TNX_PRM_ERR, CONFIG_ERR, 0, param[i]);
			numc[0] = param[i][2]; // Копируем символ в буфер
			str->com = atoi((char*)&numc);
		}
	
		// Проверяем на признак запроса лога по UART -ugl
		else if (buf_cmp(ugl_prm, param[i], 5)){
			if (task_flag)
				return tnx(TNX_ANO_TSK, CONFIG_ERR, 0, param[i]);
			task_flag = FLAG_RISE; // Ставим флаг задания
			str->state = STATE_UART_GETLOG; // Задаем состояние
		}

		// Проверяем признак количества байт -nb
		else if (buf_cmp(nb_prm, param[i], 3)){
			if (str->state != STATE_UART_READ) // Проверем что задано чтение
				return tnx(TNX_PRM_ERR, CONFIG_ERR, 0, param[i]); // Ошибка
			
			for (t = 0; t < 2; t++){ // Проверяем содержание разрядов после -nb
				if (param[i][t+3]){ // Проверяем что есть значение
					if (t == 1){ // Если последний возможный разряд
						if (param[i][t+4]) // Проверяем следующий смивол на 0
							return tnx(TNX_PRM_ERR, CONFIG_ERR, 0, param[i]); // Ошибка
					}
					for (f = 0; f < 10; f++){ // Проверяем содержание разряда
						if (param[i][t+3] == dec_test[f]) // Проверяем содержание
							break;
						if (f == 9) // Если не подошло ни одно значение
							return tnx(TNX_PRM_ERR, CONFIG_ERR, 0, param[i]); // То выходим с ошибкой
					}
					if (t) // Если больше 1 разряда
						str->numb *= 10; // То умножаем предыдущее число на 10
					else  // Если первый разряд после -nb
						str->numb = 0; // То обнуляем
					numc[0] = param[i][t + 3]; // Копируем символ в буфер
					str->numb += atoi((char*)&numc); // Конвертируем и добавляем значение
				}
				else { // Если нет значения
					if(t) // Если проверяли второй и далее после -nb разряд
						break; // Поскольку не было ошибок выходим из цикла
					else // Если проверяли первый после -nb разряд
						return tnx(TNX_PRM_ERR, CONFIG_ERR, 0, param[i]); // Ошибка
				}
			}
			if (str->numb == 0) // Если нулевое значение
				return tnx(TNX_PRM_ERR, CONFIG_ERR, 0, param[i]); // Ошибка
			else if (str->numb > MAX_BYTE_NUMBER) // Проверяем на макс. значение  
				return tnx(TNX_PRM_ERR, CONFIG_ERR, 0, param[i]); // Ошибка
		}

		// Проверяем на признак чтения по UART -ubr
		else if (buf_cmp(ubr_prm, param[i], 4)){
			if (task_flag)
				return tnx(TNX_ANO_TSK, CONFIG_ERR, 0, param[i]);
			task_flag = FLAG_RISE; // Ставим флаг задания
			if (param[i][8]) // Проверяем что после значения адреса ничего нет 
				return tnx(TNX_PRM_ERR, CONFIG_ERR, 0, param[i]);
			for (f = 4; f < 8; f++){ // Проверяем количество и содержание разрядов 
				if (param[i][f] != 0){
					for (t = 0; t < 17; t++){
						if (param[i][f] == hs_test[t]) // Проверяем содержание
							break;
						if (t == 16) // Если не подошло ни одно значение
							return tnx(TNX_PRM_ERR, CONFIG_ERR, 0, param[i]);
					}
				}
				else
					return tnx(TNX_PRM_ERR, CONFIG_ERR, 0, param[i]);
			}
			for (f = 0; f < 16; f++) // Обнуляем буфер под hex адрес
				hs_buf[f] = 0; 
			for (f = 0; f < 4; f++) // Копируем содержимое разрядов в временный буфер
				hs_buf[f] = param[i][f + 4]; 
			str->adr = strtol(hs_buf, 0, 16); // Преобразовываем символы в значение адреса
			str->state = STATE_UART_READ; // Задаем состояние
		}
		
		// Проверяем на признаки записи по UART -ubw -uba -ubo
		else if (buf_cmp(ubw_prm, param[i], 4) || buf_cmp(uba_prm, param[i], 4) || buf_cmp(ubo_prm, param[i], 4) ){
			if (task_flag)
				return tnx(TNX_ANO_TSK, CONFIG_ERR, 0, param[i]);
			task_flag = FLAG_RISE; // Ставим флаг задания
			
			for (f = 4; f < 8; f++){ // Проверяем количество и содержание разрядов адреса
				if (param[i][f] != 0){
					for (t = 0; t < 17; t++){
						if (param[i][f] == hs_test[t]) // Проверяем содержание
							break;
						if (t == 16) // Если не подошло ни одно значение
							return tnx(TNX_PRM_ERR, CONFIG_ERR, 0, param[i]);
					}
				}
				else
					return tnx(TNX_PRM_ERR, CONFIG_ERR, 0, param[i]);
			}
			for (f = 0; f < 16; f++) // Обнуляем буфер под hex адрес
				hs_buf[f] = 0; 
			for (f = 0; f < 4; f++) // Копируем содержимое разрядов в временный буфер
				hs_buf[f] = param[i][f + 4]; 
			str->adr = strtol(hs_buf, 0, 16); // Преобразовываем символы в значение адреса

			if (param[i][8] != 'd') // Проверяем что после значения адреса стоит d 
				return tnx(TNX_PRM_ERR, CONFIG_ERR, 0, param[i]);
			
			// Проверяем сколько еще d
			nd = 1;
			for (f = 0; f < 15; f++){
				if (param[i][11 + f * 3] != 'd')
					break;
				else
					nd++;
			}

			if (nd > 16) // Проверяем что значений не больше 16
				return tnx(TNX_PRM_ERR, CONFIG_ERR, 0, param[i]);

			if (param[i][8 + nd * 3]) // Проверяем что после последнего значения данных ничего нет 
				return tnx(TNX_PRM_ERR, CONFIG_ERR, 0, param[i]);

			// Проверяем содержание всех разрядов данных
			for (g = 0; g < nd; g++){
				for (f = 9; f < 11; f++){ // Проверяем количество и содержание разрядов 
					if (param[i][f + g * 3] != 0){
						for (t = 0; t < 17; t++){
							if (param[i][f + g * 3] == hs_test[t]) // Проверяем содержание
								break;
							if (t == 16) // Если не подошло ни одно значение
								return tnx(TNX_PRM_ERR, CONFIG_ERR, 0, param[i]); // То выходим с ошибкой
						}
					}
					else
						return tnx(TNX_PRM_ERR, CONFIG_ERR, 0, param[i]);
				}
			}
			
			// Преобразовываем данные
			for (g = 0; g < nd; g++){ 
				
				for (f = 0; f < 16; f++) // Обнуляем буфер под hex данные
					hs_buf[f] = 0; 
				
				for (f = 0; f < 2; f++) // Копируем содержимое разрядов в временный буфер
					hs_buf[f] = param[i][f + 9 + g * 3]; 
				
				str->data[g] = strtol(hs_buf, 0, 16); // Преобразовываем символы в значение данных
			}

			str->numb = nd; // Записываем количество байт
						
			if (buf_cmp(ubw_prm, param[i], 4)) // Задаем команду
				str->cmd = ROCA_CODE_WRITE;
			else if (buf_cmp(uba_prm, param[i], 4))
				str->cmd = ROCA_CODE_AND;
			else if (buf_cmp(ubo_prm, param[i], 4))
				str->cmd = ROCA_CODE_OR;

			str->state = STATE_UART_WRITE;
		}
	
		// Другие параметры
		else
			return tnx(TNX_PRM_ERR, CONFIG_ERR, 0, param[i]);
	
	}
	return str->state;
}