// roca_work.cpp : Defines the entry point for the console application.

#include "stdafx.h"
#include "time.h"
#include <windows.h>
#include "conio.h"
#include "roca_conl.h"
#include "param.h"

_ws ws, *p_ws; // Рабочая структуруа и указатель

// Переменные общего назначения
int i, n, t, f, k; // Счетчики 
int count; // Счетчик общего назначения
int res; // Для возврата значений функций

void str_clar(_ws *str); // Настройка рабочей структуры по умолчанию
void start_text(_ws *str); // Вступительный текст
int str_conf(_ws *str, int n_param, char *param[]); // Установка параметров
int routine(_ws *str); // Сама программа


int main(int argc, char *argv[]){
	p_ws = &ws; // Определяем указатель рабочей структуры

	str_clar(p_ws); // Конфигурация структуры по умолчанию
	start_text(p_ws); // Выводим вступительный текст

	if (argc > 1) // Если есть параметры
		ws.state = (str_conf(p_ws, argc, argv)); // Конфигурация структуры c учетом параметров	
	else
		printf("No any parameters set.\n\n");

	while(routine(p_ws)); // Сама программа
	
	printf("Press any key to exit\n");
	getch();
	return 0;
}
