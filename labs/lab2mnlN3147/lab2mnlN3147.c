#include <stdlib.h> 
#include <stdio.h> 
#include <string.h> 
#include <float.h> 
#include <stdbool.h>
#include <limits.h> //для генерации случайных чисел типа double
#include <time.h> //для генерации случайных чисел
#include <math.h> //для использования функции sqrt

/*функция-проверка на то, является ли введенное значение целым неотрицательным числом*/

bool check(char *a) {
    int length = 0, zero = (*a - '0' == 0);
    while (*a) {
        if (!((*a - '0' >= 0 && *a - '0' <= 9))) return false;
        ++a;
        ++length;
    }
    if (zero && (length != 1)) return false;
    return true;
}

/*функция-проверка на то, является ли введенное значение числом типа double*/

bool dcheck(char *a) {
    int length = 0, otr = (*a - '-' == 0), zero = (*a - '0' == 0), cnt = 0;
    while(*a) {
    	if (!((*a - '0' >= 0 && *a - '0' <= 9) || (*a - '.' == 0))) return false;
        if (*a - '0' == 0 && ((length == 1 && zero) || (length == 2 && otr && zero))) return false;
        if (otr && length == 1 && *a - '0' == 0) zero = 1;
        cnt += (*a - '.' == 0);
        ++a;
        ++length;
    }
    if (cnt > 1) return false;
    return true;
}

/*функция-генератор случайных чисел типа double*/

double rand_range(double min, double max) {	
	double random = ((double) rand()) / RAND_MAX;
	double range = (max - min) * random;
	double number = range + min;
	return number;
}

/*функция, подсчитывающая количество индивидуальных характеристик столбцов*/

int indcols(double **arr, int rows, int cols, bool *get) {
    double *ch = malloc(cols * sizeof(double)); //массив, в котором будем хранить уже посчитанные значения
    int sz = 0; //переменная, выполняющая функцию как счётчика индивидуальных характеристик, так и индекса первого неизмененного элемента массива ch
    for(int j = 0; j < cols; j++) {
    	//подсчёт суммы по столбцу
    	double cur_sum = 0;
    	for (int i = 0; i < rows; i++) {
    	    cur_sum += arr[i][j]; 
    	}
    	if (cur_sum >= 0) { //если сумма неотрицательна, то можем извлечь из неё корень, иначе сохраняем значение суммы
    	    int value = (int) sqrt(cur_sum);
    	    bool flag = true;
    	    for (int m = 0; m < sz; m++) { //проверка, совпадает ли вычисленная характеристика с уже посчитанными
     	    	if (value == ch[m]) {
     	            flag = false;
     	            get[j] = false;
     	            break;
     	    	}
     	    }
     	    if (flag) { //если значение ещё не встречалось, сохраняем его в массив
     	    	ch[sz] = value;
     	    	get[j] = true;
     	    	++sz;
     	    }
        } else { //если сумма по столбцу отрицательная, считаем её индивидуальным значением
            ch[sz] = cur_sum;
            get[j] = true;
     	    ++sz;
     	}
    }
    return sz; //возврат количества столбцов с индивидуальной характеристикой
}

/*функция, подсчитывающая количество индивидуальных характеристик рядов*/

int indrows(double **arr, int rows, int cols, bool *get, double *values, int size) {
    double *ch = malloc(rows * sizeof(double)); //массив, в котором будем хранить уже посчитанные значения
    int sz = 0; //переменная, выполняющая функцию как счётчика индивидуальных характеристик, так и индекса первого неизмененного элемента массива ch
    for(int j = 0; j < rows; j++) {
        //подсчёт суммы по ряду
    	double cur_sum = 0;
    	for (int i = 0; i < cols; i++) {
    	    cur_sum += arr[j][i];
    	}
    	if (cur_sum >= 0) { //если сумма неотрицательна, то можем извлечь из неё корень, иначе сохраняем значение суммы
    	    int value = (int) sqrt(cur_sum);
    	    bool flag = true;
    	    for (int m = 0; m < sz; m++) { //сравниваем текущее значение характеристики ряда с характеристиками других рядов
     	    	if (value == ch[m]) {
     	            flag = false;
     	            get[j] = false;
     	            break;
     	    	}
     	    }
     	    for (int m = 0; m < size; m++) { //сравниваем текущее значение характеристики ряда с характеристиками столбцов
     	    	if (value == values[m]) {
     	            flag = false;
     	            get[j] = false;
     	            break;
     	    	}
     	    }
     	    if (flag) { //если значение индивидуально, сохраняем его в массив
     	    	ch[sz] = value;
     	    	get[j] = true;
     	    	++sz;
     	    }
        } else { //если сумма по ряду отрицательна, считаем её индивидуальным значением
            ch[sz] = cur_sum;
            get[j] = true;
     	    ++sz;
     	}
    }
    return sz;
}    	    

int main(int argc, char *argv[]) {
    srand(time(NULL)); //для генерации случайных чисел
    //включение отладочного вывода
    char *DEBUG = getenv("LAB2DEBUG");
    if (DEBUG) {
        fprintf(stderr, "Включен вывод отладочных сообщений\n");
    }
    //проверка корректности вводимых параметров первой строки
    switch (argc) {
    	case 4:
    	    if (argv[1][0] == '-') {
    	    	if (argv[1][1] != 'm') {
    	    	    fprintf(stderr, "Ошибка: опция '%s' не поддерживается.\n", argv[1]);
    	    	    return 1;
    	    	}
    	    } else {
    	    	fprintf(stderr, "Usage: %s [-m] число_строк число_столбцов\n", argv[0]);
    	    	return 1;
    	    } 
    	    break; 
    	case 3:
    	    if (!check(argv[1])) {
    	    	fprintf(stderr, "Ошибка: '%s' не является числом.\n", argv[1]);
    	    	return 1;
    	    } else if (!check(argv[2])) {
    	        fprintf(stderr, "Ошибка: '%s' не является числом.\n", argv[1]);
    	        return 1;
    	    }
    	    break;
    	default:
    	    fprintf(stderr, "Usage: %s [-m] число_строк число_столбцов\n", argv[0]);
    	    return 1;
    }
    
    // создание матрицы, заполнение её рандомными/вводимыми значениями
    bool random = true;
    if (!strcmp(argv[1], "-m")) {
    	random = false;
    	printf("Введите строки матрицы:\n");
    } 
    int r = atoi(argv[2 -  random]), c = atoi(argv[3 - random]);
    //создание матрицы
    double **mtx;
    mtx = malloc(r * sizeof(double*));
    for (int i = 0; i < r; i++) {
    	mtx[i] = malloc(c * sizeof(double));
    }
    //заполнение матрицы
    char st[100];
    for (int i = 0; i < r; i++) {
    	for (int j = 0; j < c; j++) {
    	    if (random) { 
    	    	double value = rand_range(-5, 5); //генерация случайного double числа в промежутке (-5; 5)
	    	mtx[i][j] = value;
    	    } else {
    	    	scanf("%s", st);
    	    	if (dcheck(st)) {
    	    	    mtx[i][j] = atof(st);
    	    	} else {
    	    	    fprintf(stderr, "Некорректные входные данные!\n");
    	    	    return 1;
    	    	}
    	    }	    
    	}
    }
    printf("The initial matrix:\n");
    for (int i = 0; i < r; i++) {
    	for (int j = 0; j < c; j++) {
    	    printf("%f ", mtx[i][j]);      	    
    	}
    	printf("\n");
    }
    printf("\n");
    bool flag = false;
    while (!flag) {
    	bool *get = malloc(c * sizeof(bool)); //массив, у которого get[i] = true означает, что i cтолбец матрицы не удаляется при преобразовании
    	int new_c = indcols(mtx, r, c, get); 
        double *column_values = malloc(new_c * sizeof(double)); //массив со значениями характеристик тех столбцов, которые не удаляются при преобразовании
        int cur_ind = 0;
        //подсчёт значений характеристик столбцов
        for (int i = 0; i < c; i++) {
            if (get[i]) {
                double cur_sum = 0;
                for (int j = 0; j < r; j++) {
                    cur_sum += mtx[j][i];
                }
                if (cur_sum >= 0) { 
    	    	    int value = (int) sqrt(cur_sum);
    	    	    column_values[cur_ind] = value;
    	    	    ++cur_ind;
    	    	} else {
    	    	    column_values[cur_ind] = cur_sum;
    	    	    ++cur_ind;
    	    	}
    	    }  	
        }
        bool *getr = malloc(r * sizeof(bool)); //массив, у которого getr[i] = true означает, что i ряд матрицы не удаляется при преобразовании
        int new_r = indrows(mtx, r, c, getr, column_values, new_c);
        if (new_r == r && new_c == c) { //если не нужно удалять ни рядов, ни столбцов - преобразование завершено
            flag = true;
            break;
        } else {
            //генерируем новую матрицу
            double **tmp;
    	    tmp = malloc(new_r * sizeof(double*));
    	    for (int i = 0; i < new_r; i++) {
    	        tmp[i] = malloc(new_c * sizeof(double));
    	    }
            //заполняем её в соответствии с данными массивов get и getr
    	    int cur_r = 0;
    	    for (int i = 0; i < r; i++) {
    	    	if (getr[i]) {
    	    	    int cur_c = 0;
    	    	    for (int j = 0; j < c; j++) {
    	    	    	if (get[j]) {
    	    	    	    tmp[cur_r][cur_c] = mtx[i][j];
    	    	    	    ++cur_c;
    	    	    	}
    	    	    }
    	    	    ++cur_r;
    	    	}
    	    }
    	    printf("Matrix after operations:\n");
    	    for (int i = 0; i < new_r; i++) {
    		for (int j = 0; j < new_c; j++) {
    	    	     printf("%f ", tmp[i][j]);      	    
    		}
    		printf("\n");
    	    }
    	    printf("\n");
    	    //очищение массивов, приравнивание старых значений к актуальным
    	    free(mtx);
    	    mtx = tmp;
    	    r = new_r;
    	    c = new_c;
    	    free(getr);
    	    free(get);    
    	    if (r == c && c == 1) { //матрицы 1 на 1 - частный случай - если пропустить её через этот цикл, матрицы не останется
    	    	flag = true;
    	    }	    	    
    	}
    }
    printf("The final matrix:\n");
    for (int i = 0; i < r; i++) {
    	for (int j = 0; j < c; j++) {
    	    printf("%f ", mtx[i][j]);      	    
    	}
    	printf("\n");
    }
    free(mtx);
    return 0;
}
