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
