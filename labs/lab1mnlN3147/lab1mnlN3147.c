#include <stdlib.h>
#include <stdio.h>
#include <time.h> //для применения функции time()

unsigned long UL_MAX = 4294967295;

/*функция-проверка корректности входных данных:
	если введённое значение состоит исключительно из цифр
	и при этом первая цифра ненулевая - данные корректны. */

int check(char *argv[]) {
    char *ptr = argv[1];
    int check = ((*ptr - '0' >= 1) && (*ptr - '1' <= 9));

    while (*ptr) {
        if (!(*ptr - '0' >= 0 && *ptr - '1' <= 9)) {
            check = 0;
            break;
        }
        ++ptr;
    }
    return check;
}

//функция-переводчик числа из десятичной СС в двоичную СС
void bin(unsigned long a) {
    if (a / 2 > 0) {
        bin(a / 2);
    }
    printf("%ld", a % 2);
}

//функция, возводящая число n в степень m
unsigned long p(unsigned long n, unsigned long m) {
    if (!m) {
        return 1;
    }
    if (m % 2 == 0) {
        unsigned long t = p(n, m / 2);
        return t * t;
    }
    return n * p(n, m - 1);
}

/*функция-преобразователь числа: 
	отталкиваемся от определения обратных друг другу тетрад, получаем,
	что если их xor двух соседних тетрад в двоичной СС принимает значение 1111,
	эти тетрады - обратные. */

unsigned long preobr(unsigned long a) {
    int cur_tetr = 0;
    unsigned long res = 0;
    while (a > 0) {
        unsigned long ml = a % 16;
        unsigned long st = (a / 16) % 16;
        if (ml + st == 15) {
            ml ^= 1;
        }
        res += ml * p(16, cur_tetr);
        ++cur_tetr;
        res += st * p(16, cur_tetr);
        ++cur_tetr;
        a /= 256;
    }
    return res;
}

/*функция-обёртка алгоритма решения - 
	чтобы не повторять в двух условных блоков одного и того же кода */

void solve(unsigned long ch) {
    printf("Двоичное представление числа %lu: ", ch);
    bin(ch);

    //преобразуем число согласно алгоритму и выведем его в десятичной и двоичной СС
    ch = preobr(ch);
    printf("\nРезультат преобразования в десятичной СС: %lu\n", ch);
    printf("Двоичное представление результата преобразования: ");
    bin(ch);
    printf("\n");
}

int main(int argc, char *argv[]) {
    srand(time(NULL));

    char *DEBUG = getenv("LAB1DEBUG");
    if (DEBUG) {
        fprintf(stderr, "Включен вывод отладочных сообщений\n");
    }

    if (argc != 2) {
        //число не введено - генерируем случайное
        unsigned long a = 0 + rand() % (UL_MAX + 1);
        printf("Случайное число: %lu\n", a);
        solve(a);
        return EXIT_FAILURE;
    }

    if (check(argv)) {
        char *end;
        unsigned long a = strtoul(argv[1], &end, 10);
        solve(a);
    } else printf("Ошибка: '%s' не соответствует формату входных данных.", argv[1]);

    return EXIT_SUCCESS;
}
