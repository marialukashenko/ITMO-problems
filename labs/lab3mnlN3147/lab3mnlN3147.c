#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <regex.h>
#include <stdbool.h>

#define FILE_ERROR 11
#define USAGE_ERROR 10
#define DUMB_INPUT 9
#define OPTION_ERROR 8
#define MEMORY_ERROR 558

#define COLOR_GREEN "\x1b[32m"
#define COLOR_RESET "\x1b[0m"
#define BOLD_TEXT "\x1b[1m"
#define RESET_TEXT "\x1b[0m"

#define MAX_STRING_LENGTH 1000
#define MAX_ARRAY_LENGTH 500

/*xargs ./lab3abcNXXXXX < input.txt > output.txt - команда для работы с перенаправлением ввода-вывода, которое выполняется из командного интерпретатора (bash, sh, ...)*/
/*while IFS= read -r line; do ./lab3abcNXXXXX $line; done < input.txt - команда для дебага запуска программы - запускает из входного файла различные(неверные тоже) входные данные*/


/*глобальные переменные*/

// регулярное выражение, соответствующее формату номера карты
char *pattern = "[0-9][0-9][0-9][0-9]-[0-9][0-9][0-9][0-9]-[0-9][0-9][0-9][0-9]-[0-9][0-9][0-9][0-9]";

// структура, используемая для представления регулярного выражения
regex_t reegex;

// идентификатор вхождения подстроки в рассматриваемую строку
int value;

// структура, используемая для хранения информации о вхождениях подстроки в рассматриваемую строку
regmatch_t match;

// переменная, применющаяся при анализе строки на вхождение подстрок
size_t shift = 0;

// переменные, необходимые для обработки введённых опций
bool n = false;
bool c = false;
int f = 0, t = 0;

// идентификатор ввода
bool console_input = false;

// файлы ввода/вывода
FILE *fr, *fw;

// строки для ввода и обработки данных
char s[MAX_STRING_LENGTH];
char *concatenated_string = NULL;


/*функция-проверка, является ли введенное значение целым неотрицательным числом*/

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


/*процедура-проверка, является ли введённое значение поддерживаемым файлом для ввода/вывода*/

void check_file (char *s, char *mode) {
	if (!strcmp(mode, "r")) {
		if ((fr = fopen(s, "r")) == NULL) {
			fprintf(stderr, "Ошибка: не удалось открыть файл %s для чтения.\n", s);
			exit(FILE_ERROR);
		}
	} else fw = fopen(s, "w");
}


/*алгоритм Луна для проверки корректности найденной подстроки*/

bool luhn_algorithm (char* s) {
	int sum = 0;
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			int ch = s[5*i + j] - '0';
			if (j % 2 == 0) ch *= 2;
			sum += ch - (9 * (ch >= 10));
		}
	}
	return ((sum % 10 == 0) ? true : false);
}


/*процедура-проверка, необходимо ли осуществлять перенос строки*/

void check_gap (int *cur_gap, int rr, int* ind_r, int notcolored) {
	if (*cur_gap < rr && notcolored == ind_r[*cur_gap]) {
    		*cur_gap = *cur_gap + 1;
    		if (fw != NULL) {
    			fprintf(fw, "\n");
    		} else printf("\n");
    	}
}


/*процедура вывода строки/строк с учётом формата вывода*/

void print_modifyed_string(int* ind, char* s, int cur_ind, int* ind_r, int rr) {
    	int notcolored = 0;
    	int cur_gap = 0;
    	for (int i = 0; i < cur_ind; i++) {
    		while (notcolored < ind[i]) {
    			if (rr) {
    				check_gap (&cur_gap, rr, ind_r, notcolored);
    			}
    			if (fw != NULL) fprintf(fw, "%c", (char) (s[notcolored]));
    			else printf("%c", (char) (s[notcolored]));
        		++notcolored;
        	}
        	if (fw != NULL) {
			fprintf(fw, "**");
		} else {
			if (c) printf(COLOR_GREEN);
			else printf(BOLD_TEXT);
		}
        	for(int j = 0; j < 19; j++) {
        		if (rr) {
        			check_gap (&cur_gap, rr, ind_r, notcolored);
        		}
        		if (fw != NULL) fprintf(fw, "%c", (char) (s[notcolored]));
    			else printf("%c", (char) (s[notcolored]));
        		++notcolored;
        	}
        	if (fw != NULL) {
			fprintf(fw, "**");
		} else {
			if (c) printf(COLOR_RESET);
			else printf(RESET_TEXT);
		}
        }
        while (notcolored < (int) strlen(s)) {
        	if (rr) {
        		check_gap (&cur_gap, rr, ind_r, notcolored);
        	}
        	if (fw != NULL) {
    			fprintf(fw, "%c", (char) (s[notcolored]));
    		} else printf("%c", (char) (s[notcolored]));
        	++notcolored;
        }
        if (fw != NULL) {
		fprintf(fw, "\n");
	} else printf("\n");
}


/*процедура поиска подстрок в строке*/

void analyze_string(char* s, int* ind_r, int rr) {
	int counter = 0;
	value = regexec( &reegex, s, 0, NULL, 0);
        if (!value) {
        	shift = 0;
        	int ind[MAX_ARRAY_LENGTH];
        	int cur_ind = 0;			
        	// поиск вхождений подстроки
    		while (regexec(&reegex, s + shift, 1, &match, REG_NOTBOL) == 0) {
        		if (match.rm_so == -1) break;
        		
        		// составление подстроки
        		char substr[19];
        		for (int i = shift + match.rm_so; i < (int) (shift + match.rm_so + 19); i++) {
        			substr[i - shift - match.rm_so] = (char) (s[i]);
        		}	
        				
        		// если найденная подстрока удовлетворяет алгоритму Луна, необходимо запомнить индекс начала такой подстроки
        		if(luhn_algorithm(substr)) {
        			++counter;
        			if (((counter >= f) || (f == 0)) && ((counter <= t) || (t == 0))) {
        				ind[cur_ind] = shift + match.rm_so;
        				++cur_ind;
        			}
        		}
        		
        		// обновление смещения для продолжения поиска
        		shift += match.rm_eo;
    		}
    		print_modifyed_string(ind, s, cur_ind, ind_r, rr);
	} else {
		print_modifyed_string(NULL, s, 0, ind_r, rr);
	}
}


/*процедура обработки вводимых значений*/

void solve() {
	// компиляция регулярного выражения
    	value = regcomp( &reegex, pattern, 0);
	if (n) {
    		if (!console_input) {
    			while (fgets(s, sizeof(s), fr) != NULL) {
    				size_t length = strlen(s);
    				if (s[length - 1] == '\n') {
            				s[length - 1] = '\0';
            				length--;
        			}
        			analyze_string(s, NULL, 0);
    			}
    		} else {
    			while (fgets(s, sizeof(s), stdin) != NULL) {
    				size_t length = strlen(s);
    				if (s[length - 1] == '\n') {
            				s[length - 1] = '\0';
            				length--;
        			}
    				analyze_string(s, NULL, 0);
    			}
		}
	} else {
		//обработка случая неединственной строки
    		size_t total_length = 0;
    		int ind_r[MAX_ARRAY_LENGTH];
    		int rr = 0;
    		if (!console_input) {
    			while (fgets(s, sizeof(s), fr) != NULL) {
    				size_t length = strlen(s);
    				
    				// удаление символа переноса строки
    				if (s[length - 1] == '\n') {
            				s[length - 1] = '\0';
            				length--;
        			}
        			// динамическое выделение памяти для дополненной строки
    				char *temp = realloc(concatenated_string, total_length + length);
    				
    				// обработка ошибки выделения памяти
        			if (temp == NULL) {
            				fprintf(stderr, "Ошибка при выделении памяти\n");
            				free(concatenated_string);
            				exit(MEMORY_ERROR);
            			}
            			
            			// дополнение общей строки
            			concatenated_string = temp;
        			strcpy(concatenated_string + total_length, s);
        			total_length += length;
        			
        			// установка "разрыва" строки
        			ind_r[rr] = ((int) total_length);
        			++rr;
            		}
        	} else {
        		while (fgets(s, sizeof(s), stdin) != NULL) {
    				size_t length = strlen(s);
    				
    				// удаление символа переноса строки
    				if (s[length - 1] == '\n') {
            				s[length - 1] = '\0';
            				length--;
        			}
        			// динамическое выделение памяти для дополненной строки
    				char *temp = realloc(concatenated_string, total_length + length);
    				
    				// обработка ошибки выделения памяти
        			if (temp == NULL) {
            				fprintf(stderr, "Ошибка при выделении памяти\n");
            				free(concatenated_string);
            				exit(MEMORY_ERROR);
            			}
            			
            			// дополнение общей строки
            			concatenated_string = temp;
        			strcpy(concatenated_string + total_length, s);
        			total_length += length;
        			
        			// установка "разрыва" строки
        			ind_r[rr] = ((int) total_length);
        			++rr;
        		}
        	}
        	analyze_string(concatenated_string, ind_r, rr);
    	}
}


/*процедура-проверка корректности вводимых опций и названий файлов*/

void check_option (char *s) {
	int length = strlen(s);
	if (s[0] == '-') {
		switch (length) {
			case 1:
    	    			fprintf(stderr, "Usage: lab3mnlN3147 [опции] [имя_вход_файла [имя_выход_файла]]\n");
    	    			exit(USAGE_ERROR);
    	    		case 2:
    	    			// проверка на принадлежность опции к типам "-c", "-n" и "-v"
    	    			if (!strcmp(s, "-v")) {
    	    				printf("Лукашенко Мария Николаевна, гр. N3147\nВариант: 4-2-1-2\n");
    	        			exit(EXIT_SUCCESS);
    	    			} else if (!strcmp(s, "-c")) {
    	    				if (((fr != NULL) || (fw != NULL))) {
    	    					fprintf(stderr, "Usage: lab3mnlN3147 [опции] [имя_вход_файла [имя_выход_файла]]\n");
    	    					exit(USAGE_ERROR);
    	    				}
					if (c) {
    	    	    	    			fprintf(stderr, "Ошибка: одна и та же опция используется несколько раз.\n");
    	    	            			exit(DUMB_INPUT);
    	        			} else {
    	        				c = true;
    	        				break;
    	        			}
    	        		} else if (!strcmp(s, "-n")) {
    	        			if (n) {
    	                    			fprintf(stderr, "Ошибка: одна и та же опция используется несколько раз.\n");
    	    	            			exit(DUMB_INPUT);
    	    	        		} else {
    	    	        			n = true;
    	    	        			break;
    	    	        		}
    	    			} else {
    	            			fprintf(stderr, "Ошибка: неподдерживаемая опция %s\n", s);
    	    	        		exit(OPTION_ERROR);
    	    			}
    	    		default:
    	    			// проверка на принадлежность опции к типам "-f=N" и "-t=N"
    	    			if (s[1] == 102 || s[1] == 116) {
    	    				if (s[2] == 61) {
    	    					char M[strlen(s) - 2];
    	    	        			for (int i = 3; i < (int) (strlen(s)); i++) {
    	    	            				M[i - 3] = s[i];
    	    	        			}
    	    	        			M[strlen(s) - 3] = '\0';
    	    	        			if (!check(M)) {
    	    	            				fprintf(stderr, "Ошибка: %s не является числом.\n", M);
    	    	            				exit(OPTION_ERROR);
    	    	        			} else {
    	    	        				if (s[1] == 102) {
    	    	    	        				if (f) {
    	    	    	    	    					fprintf(stderr, "Ошибка: одна и та же опция используется несколько раз.\n");
    	    	         	    					exit(DUMB_INPUT);
    	    	                				} else {
    	    	                					if (check(M)) f = atoi(M);
    	    	                					break;
    	    	                				}
    	    	            				} else {
    	    	            					if (t) {
    	    	    	    	    					fprintf(stderr, "Ошибка: одна и та же опция используется несколько раз.\n");
    	    	         	    					exit(DUMB_INPUT);
    	    	                				} else {
    	    	                					if (check(M)) t = atoi(M);
    	    	                					break;
    	    	                				}
    	    	        				}
    	    	        			}
    	    	        		} else {
    	    	        			fprintf(stderr, "Ошибка: неподдерживаемая опция %s", s);
    	    	        			exit(OPTION_ERROR);
    	    	    			}				
    	    			} else {
    	    				fprintf(stderr, "Ошибка: неподдерживаемая опция %s", s);
    	    	    			exit(OPTION_ERROR);
    	    	    		}
		}
	} else {
		if (fr == NULL) {
			check_file(s, "r");
    		} else {
    			check_file(s, "w");
    		}
	}
}		
    	 
    	
int main (int argc, char *argv[]) {
	// включение отладочного вывода $ LAB3DEBUG=1 ./lab3mnlN2147 test.txt
	char *DEBUG = getenv("LAB2DEBUG");
	if (DEBUG) {
		fprintf(stderr, "Включен вывод отладочных сообщений\n");
    	}
    	switch(argc) {
    		case 1:
    			// определение идентификатора ввода
    	    		console_input = true;
    	    		solve();
    	    		break;
    		case 2:
    		case 3:
    		case 4:
    		case 5:
    		case 6:
    		case 7:
    		case 8:
    			// проверка введённых опций
    			for (int i = 1; i < argc; i++) {
    				check_option(argv[i]);
    				if ((i < argc - 2 && ((fr != NULL))) || (i != argc - 1 && fw != NULL)) {
    					fprintf(stderr, "Usage: lab3mnlN3147 [опции] [имя_вход_файла [имя_выход_файла]]\n");
    	    				exit(USAGE_ERROR);
    				}
    			}
    			// определение идентификатора ввода
    			if (fr != NULL) console_input = false;
    			else console_input = true;
    			
    			// проверка всех опций
    			fprintf(stderr, "Debug: -n: %d, -c: %d, f: %d, t: %d, console: %d, output: %d\n", n, c, f, t, console_input, (fw == NULL));
    			
    			solve();
    			break;
    		default: 
    			fprintf(stderr, "Usage: lab3mnlN3147 [опции] [имя_вход_файла [имя_выход_файла]]\n");
    	    		exit(USAGE_ERROR);
    	}
    	//очистка памяти
    	regfree(&reegex);
    	if (fr != NULL) fclose(fr);
    	if (fw != NULL) fclose(fw);
    	free(concatenated_string);
    	
    	exit(EXIT_SUCCESS);
}



