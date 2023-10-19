#include <stdlib.h>
#include <stdio.h>
#include <time.h> //для применения функции time()

unsigned long UL_MAX = 4294967295;

/*функция-проверка корректности входных данных:
	если введённое значение состоит исключительно из цифр
	и при этом первая цифра ненулевая(или число нулевое) - данные корректны. */

int check(char *argv[]) {
    char *ptr = argv[1];
    int length = 0;
    int zero = (*ptr - '0' == 0);
    int ch = ((*ptr - '0' >= 0) && (*ptr - '1' <= 9));
    while (*ptr) {
        if (!(*ptr - '0' >= 0 && *ptr - '1' <= 9)) {
            ch = 0;
            break;
        }
        ++ptr;
        ++length;
    }
    if (zero && (length != 1)) {
    	ch = 0;
    }
    return ch;
}

//функция-переводчик числа из десятичной СС в двоичную СС
void bin(unsigned long a) {
    //определим, сколько ненулевых байтов содержит число(у типа unsigned long максимум - 4, минимум - 1)
    int bit = 0;
    for (int n = 1; n <= 4; n++) {
        if (a <= (unsigned long) 1 << (8 * n - 1)) {
            bit = n;
            break;
        }
    }

    //битовыми сдвигами "пройдёмся" по каждому биту числа слева направо, выводя их значения и разделяя биты пробелами
    for (int i = 0; i < bit; i++) {
        for (int j = 0; j < 8; j++) {
            if (a & ((unsigned long) 1 << (8 * bit - 1))) {
                printf("1");
            } else printf("0");
            a <<= 1;
        }
        printf(" ");
    }
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
	что если xor двух соседних тетрад в двоичной СС принимает значение 1111,
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
    	res += ml*p(16, cur_tetr);
    	++cur_tetr;
    	res += st*p(16, cur_tetr);
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
    char *DEBUG = getenv("LAB1DEBUG");
    if (DEBUG) {
        fprintf(stderr, "Включен вывод отладочных сообщений\n");
    }

    if (argc != 2) {
        //число не введено - генерируем случайное
        srand(time(NULL));
        unsigned long a = 0 + rand() % (UL_MAX + 1);
        printf("Случайное число: %lu\n", a);
        solve(a);
        return EXIT_FAILURE;
    }

    if (check(argv)) {
        char *end;
        unsigned long a = strtoul(argv[1], &end, 10);
        solve(a);
    } else printf("Ошибка: '%s' не соответствует формату входных данных.\n", argv[1]);

    return EXIT_SUCCESS;
}
