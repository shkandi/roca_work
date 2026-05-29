#include "stdafx.h"
#include <stdlib.h>
#include "conio.h"
#include "time.h"
#include <math.h>

#include "param.h"

extern _ws ws, *p_ws; // Рабочая структура и указатель

// Получение системного времени
struct tm *ptm;
extern time_t timer, timer_n;

// Переменные для файла
extern FILE *fp;

// Переменная для счетчика
int mar_count;
char ar_marker[21] = {4, 4, 2, 2, 4, 4, 2, 2, 4, 4, 2, 2, 3, 1, 1, 3, 1, 1, 3, 1, 1};


// Функция сравнения буферов
char buf_cmp(char *base, char *target, char count){
	char i;
	for (i = 0; i < count; i++){ // Проверяем
		if (*(base + i) != *(target + i))
				return 0;
	}
	return 1;
}



// Функция печати числа
char pnum(void *num, char var_tp, char flags, char raz){
	
	int pn_c; // Счетчик
	unsigned int pn_div; // Делитель
	unsigned int pn_numb; // Переменная для числа
	unsigned int pn_oper; // Оперативная переменная
	unsigned char pn_char; // Char
	unsigned short pn_short; // Short
	unsigned int pn_int; // int
	
	// Записываем число в оперативную переменную согласно типу
	switch(var_tp){
	case PNUM_VAR_CHAR:
		pn_char = *(unsigned char*)num;
		pn_oper = (unsigned int)pn_char;
		break;
	case PNUM_VAR_SHORT:
		pn_short = *(unsigned short*)num;
		pn_oper = (unsigned int)pn_short;
		break;
	case PNUM_VAR_INT:
		pn_int = *(unsigned int*)num;
		pn_oper = (unsigned int)pn_int;
		break;
	default: 
		return PNUM_ERR_TYPE;
	}


	if (flags & PNUM_FLAG_HEX){ // Если флаг HEX
		if (flags & PNUM_FLAG_PREFIX) // Если флаг префикса
			printf("0x");
		for (pn_c = 1; pn_c < raz + 1; pn_c++){
			pn_div = (int)pow((double)16, (double)(raz - pn_c));
			pn_numb = pn_oper / pn_div;	
			printf("%x", pn_numb);
			pn_oper %= pn_div;			
		}
		if (flags & PNUM_FLAG_SPACE)
			printf(" ");
		if (flags & PNUM_FLAG_NEXTS)
			printf("\n");
	}
	else
		return PNUM_ERR_SYSTEM;

	return PNUM_ERR_NOERR;
}



// Заголовок лога
void log_text(char text){
	if (text == FTEXT_LOG_HEADER){
		timer = time(NULL); // Берем системное время
		ptm = localtime(&timer); // Заполняем структуру времени
		//fprintf(fp, "\n");
		//fprintf(fp, "Log file of mobile platform events.\n");
		fprintf(fp, "Protocol file.\n");
		fprintf(fp, "Date: %d.%d.%d Time: %d:%d:%d\n", ptm->tm_mday, ptm->tm_mon, ptm->tm_year - 100, ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
		fprintf(fp,"\n");
	}
	else if (text == FTEXT_LOG_START)
		fprintf(fp, "//////// START OF ROCA LOG ////////\n");
	else if (text == FTEXT_LOG_END){
		fprintf(fp, "///////// END OF ROCA LOG /////////\n");
	}
	return;
}



// Печать расшифровки одной записи в файл.
int sf_print(unsigned char *oper_b){
	
	int f;
	int r_strok;
	int pv; // Переменная для степени
	unsigned char nibble; // Для половины байта
	
	// Пишем номер записи
	r_strok = *oper_b;
	r_strok <<= 8;
	r_strok |= *(oper_b + 1);
	for (f = 0; f < 4; f++){
		pv = (int)pow((double)10, (double)f);
		fprintf(fp,"%d", r_strok/(1000/pv));
		r_strok = r_strok % (1000/pv);
	}	
	fprintf(fp,"  ");

	// Пишем дату и время
	fprintf(fp," ");
	fprintf(fp,"00.00 12:00:00.00");
	fprintf(fp," ");
	// Пока не сделано
	
	// Приходящее или уходящее
	if (*(oper_b + 7))
		fprintf(fp,"Inc");
	else
		fprintf(fp,"Out");
	fprintf(fp," ");

	// Номер события
	fprintf(fp,"Ev:");
	r_strok = *(oper_b + 8);
	for (f = 0; f < 3; f++){
		pv = (int)pow((double)10, (double)f);
		fprintf(fp,"%d", r_strok/(1000/pv));
		r_strok = r_strok % (1000/pv);
	}	
	fprintf(fp,"  ");

	
	// Доп код, данные и резерв
	for (f = 9; f < 16; f++){
		if(f == 9)
			fprintf(fp,"Co:");
		else if(f == 10)
			fprintf(fp,"Da:");
		else if(f == 14)
			fprintf(fp,"Rs:");
		
		nibble = *(oper_b + f);
		nibble >>= 4;
		fprintf(fp, "%X", nibble);
		nibble = *(oper_b + f);
		nibble &= 0x0f;
		fprintf(fp, "%X", nibble);

		if(f == 9 || f == 10)
			fprintf(fp," ");
	
	}

	fprintf(fp,"\n");	
	return 0;
}



// Печать расшифровки параметра в файл
int pf_print(unsigned char *oper_b){
	
	int f;
	int r_strok;
	char p_flag;
	int simb;
	int pv; // Переменная для степени
	unsigned char nibble; // Для половины байта
	
	mar_count++;
	
	// Пишем номер записи
	fprintf(fp,"Marker: ");
	r_strok = mar_count;
	for (f = 0; f < 2; f++){
		pv = (int)pow((double)10, (double)f);
		fprintf(fp,"%d", r_strok/(10/pv));
		r_strok = r_strok % (10/pv);
	}	
	
	fprintf(fp," | ");
	
	if (ar_marker[mar_count - 1] == 1)
		fprintf(fp,"AKS");
	else if (ar_marker[mar_count - 1] == 2)
		fprintf(fp,"MKS");
	else if (ar_marker[mar_count - 1] == 3)
		fprintf(fp,"AES");
	else if (ar_marker[mar_count - 1] == 4)
		fprintf(fp,"BES");
	
	fprintf(fp," | ");

	// Ошибки
	/*
	fprintf(fp,"Err: ");
	r_strok = *oper_b;
	p_flag = FLAG_DOWN;
	for (f = 0; f < 3; f++){
		pv = (int)pow((double)10, (double)f);
		simb = r_strok/(100/pv);
		
		if (simb)
			p_flag = FLAG_RISE;	
		
		if (p_flag)
			fprintf(fp,"%d", simb);
		else {
			if (f == 2)
				fprintf(fp,"0");
			else
				fprintf(fp," ");	
		}
		r_strok = r_strok % (100/pv);
	}	
	fprintf(fp," | ");
	*/
		
	// Счетчик
	fprintf(fp,"Counter: ");
	r_strok = *(oper_b + 1);
	r_strok <<= 8;
	r_strok |= *(oper_b + 2);
	r_strok <<= 8;
	r_strok |= *(oper_b + 3);
	p_flag = FLAG_DOWN;
	for (f = 0; f < 8; f++){
		pv = (int)pow((double)10, (double)f);
		simb = r_strok/(10000000/pv);

		if (simb)
			p_flag = FLAG_RISE;	
		
		if (p_flag)
			fprintf(fp,"%d", simb);
		else {
			if (f == 7)
				fprintf(fp,"0");
			else
				fprintf(fp," ");	
		}		
		r_strok = r_strok % (10000000/pv);
	}	
	fprintf(fp," | ");

	// Среднее
	fprintf(fp,"Middle: ");
	r_strok = *(oper_b + 5);
	r_strok <<= 8;
	r_strok |= *(oper_b + 4);
	p_flag = FLAG_DOWN;
	for (f = 0; f < 5; f++){
		if (f == 3)
			fprintf(fp,",");

		pv = (int)pow((double)10, (double)f);
		simb = r_strok/(10000/pv);
		
		if (simb)
			p_flag = FLAG_RISE;	
		
		if (p_flag)
			fprintf(fp,"%d", simb);
		else {
			if (f > 1)
				fprintf(fp,"0");
			else
				fprintf(fp," ");	
		}
		r_strok = r_strok % (10000/pv);
	}	
	fprintf(fp," ");
	fprintf(fp,"ms");
	fprintf(fp," | ");

	
	// Максимальное
	fprintf(fp,"Max: ");
	r_strok = *(oper_b + 7);
	r_strok <<= 8;
	r_strok |= *(oper_b + 6);
	p_flag = FLAG_DOWN;
	for (f = 0; f < 5; f++){
		if (f == 3)
			fprintf(fp,",");

		pv = (int)pow((double)10, (double)f);
		simb = r_strok/(10000/pv);
		
		if (simb)
			p_flag = FLAG_RISE;	
		
		if (p_flag)
			fprintf(fp,"%d", simb);
		else {
			if (f > 1)
				fprintf(fp,"0");
			else
				fprintf(fp," ");	
		}
		r_strok = r_strok % (10000/pv);
	}	
	fprintf(fp," ");
	fprintf(fp,"ms");
	fprintf(fp," | ");
	
	// Минимальное
	fprintf(fp,"Min: ");
	r_strok = *(oper_b + 9);
	r_strok <<= 8;
	r_strok |= *(oper_b + 8);
	p_flag = FLAG_DOWN;
	for (f = 0; f < 5; f++){
		if (f == 3)
			fprintf(fp,",");

		pv = (int)pow((double)10, (double)f);
		simb = r_strok/(10000/pv);
		
		if (simb)
			p_flag = FLAG_RISE;	
		
		if (p_flag)
			fprintf(fp,"%d", simb);
		else {
			if (f > 1)
				fprintf(fp,"0");
			else
				fprintf(fp," ");	
		}
		r_strok = r_strok % (10000/pv);
	}	
	fprintf(fp," ");
	fprintf(fp,"ms");



	fprintf(fp,"\n");	
	return 0;
}


// Функция вывода вступительного текста
void start_text(_ws *str){
	printf("Welcome to roca_work programm.\n");
	printf("\n");
	return;
}



// Функция вывода сообщения и выхода
char tnx(char tn, char ret, char pnum, char *pstr){
	if (tn == TNX_PRM_ERR)	
		printf("Parameter error: %s\n", pstr);
	else if (tn == TNX_ANO_TSK)	
		printf("Error! Too many task's: %s\n", pstr);
	else if (tn == TNX_LIB_ERR)	
		printf("Library error occured: %d\n", pnum);
	else if (tn == TNX_ERR_OCC)	
		printf("Programm error occurred: %d\n", pnum);
	else if (tn == TNX_BUF_EMP)
		printf("No data to receive. Buffer is emty.\n");

	return ret;
}



// Интервал
void delay(int msec){
	int cst = clock();
	while (clock() < cst + msec);
	return;
}