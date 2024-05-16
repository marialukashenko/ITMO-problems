#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>
#include <ctype.h>

#define INITIAL_STRING_SIZE 256
#define FILE_ERROR 7
#define USAGE_ERROR 6
#define OPTION_ERROR 5
#define MEMORY_ERROR 4
#define DATA_ERROR 3

typedef struct item {
  struct item *next, *prev;   
  char* data;             
} item;

typedef struct list_t {
  struct item *head, *tail;
} list_t;

void push_back (list_t *lst, char *s) {
  // создание нового элемента
  item *new_item = malloc(sizeof(*new_item));
  new_item->next = new_item->prev = NULL;
  new_item->data = s;

  // элементы в списке есть => обновление "хвоста" 
  if (lst->tail) {
    new_item->prev = lst->tail;
    lst->tail->next = new_item;
    lst->tail = new_item;
  } else { 
    assert(!lst->head); // ситуация, в которой есть "голова", но нет "хвоста" недопустима
    lst->head = lst->tail = new_item; // добавление первого элемента в списке
  }
}

void push_front (list_t *lst, char *s) {
  // создание нового элемента
  item *new_item = malloc(sizeof(*new_item));
  new_item->prev = new_item->next = NULL;
  new_item->data = s;
  
  // элементы в списке есть => обновление "головы" 
  if (lst->head) {
    new_item->next = lst->head;
    lst->head->prev = new_item;
    lst->head = new_item;
  } else {
    assert(!lst->tail); // ситуация, в которой нет "головы", но есть "хвост" недопустима
    lst->head = lst->tail = new_item; // добавление первого элемента в списке
  }
}

void pop_back (list_t *lst) {
  assert (lst->tail); // проверка на наличие элемента, который необходимо удалить
  item *del = lst->tail;
  if (del->prev) {
    del->prev->next = NULL;
    lst->tail = del->prev;
  } else { // единственный элемент в списке => полное обнуление списка
    lst->head = NULL;
    lst->tail = NULL;
  }
  free(del);
}
  
void pop_front (list_t *lst) {
  assert (lst->head); // проверка на наличие элемента, который необходимо удалить
  item *del = lst->head;
  if (del->next) { 
    del->next->prev = NULL;
    lst->head = del->next;
  } else { // единственный элемент в списке => полное обнуление списка
    lst->head = NULL;
    lst->tail = NULL;
  }
  free(del);
}    

void print_lst (list_t *lst) {
  if (lst->head) {
    item *cur_item = lst->head;
    while (cur_item) {
      for (int j = 0; j < (int)strlen(cur_item->data); j++) {
        printf("%c", (cur_item->data)[j]);
      }
      printf("\n");
      cur_item = cur_item->next;
    }
  } else {
    printf("Список пуст!\n");
  }
}

void dump (list_t *lst, char* path) {
  bool f = false;
  FILE *write;

  // функция поддерживает вывод в консоль и файл, проверка на введение пути к файлу
  if (strcmp(path, "")) {
    write = fopen(path, "w");
    if (!write) {
      fprintf(stderr, "Ошибка: не удалось открыть файл для чтения.\n");
      exit(FILE_ERROR);
    }
    f = true;
  }

  if (lst->head) {
    item *cur_item = lst->head;
    while (cur_item) {
      if (f) fprintf(write, "%p %p %p %s\n", (char *)cur_item->prev, (char *)cur_item, (char *)cur_item->next, cur_item->data);
      else printf("%p %p %p %s\n", (char *)cur_item->prev, (char *)cur_item, (char *)cur_item->next, cur_item->data);
      cur_item = cur_item->next;
    }
    free(cur_item);
  }
  if (f) fclose(write);
} 

// процедура смещения - составная, её можно представить как последовательный перенос последнего элемента перед текущим началом списка, его удаление с конца списка
void rot_left (list_t *lst, int N) {
  if (lst->tail) {
    for (int i = 0; i < N; i++) {
      item *temp  = lst->tail;
      pop_back(lst);
      push_front(lst, temp->data);
    }
  }
}

// функция получения данных n-го элемента списка
char* elem_data(list_t *lst, int n) {
  item *current = lst->head;
  int count = 0;
  while (current != NULL && count < n) {
      current = current->next;
      ++count;
  }
  return current->data;
}

// функция-подсчёт длины списка
short int size (list_t *lst) {
  int s = 0;
  if (lst->head) {
    item *cur_item = lst->head;
    while (cur_item) {
      cur_item = cur_item->next;
      ++s;
    }
  }
  return s;
}

FILE *file, *w;
bool read_binary = true;

// проверка валидности названия файла
bool check_filename(char *name) {
  int length = strlen(name);
  if (length < 4) return false;
  if (strcmp(name + length - 4, ".bin") != 0) return false;
  for (int i = 0; i < length - 4; ++i) {
    if (!(isalnum(name[i]) || name[i] == '.' || name[i] == '_' || name[i] == '-')) return false;
  }
  return true;
}

// проверка существования файла/возможности его открыть
void check_file (char *s) {
  if (check_filename(s)) {
    file = fopen(s, "rb");
    if (!file) {
      fprintf(stderr, "Ошибка: не удалось открыть файл для чтения.\n");
      read_binary = false;
    }
  } else {
    fprintf(stderr, "Ошибка: некорректный формат файла.\n");
		exit(FILE_ERROR);
  }
}

// функция получения введенных после команды данных
char* grep_command(char* line) {
  int cur_ind = 0;
  char* result = (char*)malloc(strlen(line) + 1); 
  for (int i = 0; i < (int)strlen(line); i++) {
    if (line[i] == ' ' || line[i] == '\n') break;
    result[i] = line[i];
    ++cur_ind;
  }
  result[cur_ind] = '\0';
  return result;
}

// некоторые команды(push_front, push_back) не предполагают определённый формат ввода, для обработки случаев удобно анализировать аргументы при чтении
list_t grep_list_of_args(char* line, int cur_ind) {
  // список аргументов
  list_t result;
  result.head = result.tail = NULL;

  int not_beginning = 1;
  int cur_input = 0;
  char* r = (char*)malloc(strlen(line));

  // идея алгоритма - проходиться по строке, содержащей аргументы, каждый раз проверяя следующие условия:
  // a) все элементы разделены одним пробелом
  // б) каждый элемент заключен в двойные кавычки
  // в) двойные кавычки внутри аргумента экранированы 

  int i = cur_ind;
  while (i < (int)strlen(line)) {
    if (not_beginning == 1) {
      if (line[i] != '\"') {
        fprintf(stderr, "Некорректный формат ввода!\n");
        exit(DATA_ERROR);
      } else {
        r[cur_input] = line[i];
        ++cur_input;
        not_beginning = 0;
      }
    } else {
      if (line[i] != '\"') {
        r[cur_input] = line[i];
        ++cur_input;
      } else {
        if (line[i - 1] == '\\') {
          r[cur_input] = line[i];
          ++cur_input;
        } else {
          r[cur_input] = line[i];
          r[cur_input + 1] = '\0';
          char* copied_string = malloc(strlen(r) * sizeof(char));

          // занесение аргумента в список
          strcpy(copied_string, r);
          push_back(&result, copied_string);

          if (line[i + 1] == ' ' || line[i + 1] == '\0' || line[i + 1] == '\n') ++i;
          not_beginning = 1;
          r[0] = '\0';
          cur_input = 0;
        }
      }
    }
    ++i;
  }
  return result;
}


list_t grep_args(char* line, char* command) {

  // считывание команды (строка до первого пробела)
  int cur_ind = 0;
  while (cur_ind < (int)strlen(line) && line[cur_ind] != ' ') ++cur_ind;
  ++cur_ind;

  // при необходимости анализ вводимых аргументов
  if (!strcmp(command, "push_front") || !strcmp(command, "push_back")) {
    return grep_list_of_args(line, cur_ind);
  } 
  // в ином случае запись введённых после команды данных
  else {
    list_t result;
    result.head = result.tail = NULL;
    
    char* r = (char*)malloc(strlen(line));
    int cur_input = 0;
    for (int i = cur_ind; i < (int)strlen(line); i++) {
      if (line[i] == ' ' || line[i] == '\0' || line[i] == '\n') {
        r[cur_input] = '\0';
        if (strcmp(r, "\0")) {
          char* copied_string = malloc(strlen(r) * sizeof(char));
          strcpy(copied_string, r);
          push_back(&result, copied_string);
        }
        r[0] = '\0';
        cur_input = 0;
      } else {
        r[cur_input] = line[i];
        ++cur_input;
      }
    }
    if (strcmp(r, "\0")) {
      char* copied_string = malloc(strlen(r) * sizeof(char));
      strcpy(copied_string, r);
      push_back(&result, copied_string);
    }
    return result;
  }
}

// проверка корректности введённой команды
bool check_command(char* command) {
  if (!strcmp("dump", command) || !strcmp("push_front", command) || !strcmp("push_back", command) \
      || !strcmp("print", command) || !strcmp("pop_front", command) || !strcmp("pop_back", command) || !strcmp("rot_left", command)) {
    return true;
  }
  return false;
}

// проверка соответствия строки формату, определённому вариантом
bool check_string(char* s) {
  if (s[0] == '"' && s[strlen(s) - 1] == '"') {
    for (int i = 1; i < (int)strlen(s) - 1; i++) {
      if (s[i] == '"' && s[i - 1] != '\\') {
        return false;
      }
    }
    return true;
  }
  return false;
}

// проверка соответствия введённых данных числовому формату
bool check_int(char *a) {
    int length = 0, zero = (*a - '0' == 0);
    while (*a) {
        if (!((*a - '0' >= 0 && *a - '0' <= 9))) return false;
        ++a;
        ++length;
    }
    if (zero && (length != 1)) return false;
    return true;
}

// выполнение команд с проверкой корректности запроса
void make_action(list_t *lst, char* command, list_t *args) {
  if (!strcmp("push_front", command)) {
    if (args->head) {
      item *cur_item = args->head;
      while (cur_item) {
        push_front(lst, cur_item->data);
        cur_item = cur_item->next;
      }
    }
  }
  if (!strcmp("push_back", command)) {
    if (args->head) {
      item *cur_item = args->head;
      while (cur_item) {
        push_back(lst, cur_item->data);
        cur_item = cur_item->next;
      }
    }
  }
  if (!strcmp("dump", command)) {
    if (size(args) == 1) {
      dump(lst, args->head->data);
    } else if (size(args) == 0) {
      dump(lst, "");
    } else {
      fprintf(stderr, "Операция '%s' требует единственный аргумент - название файла!\n", command);
      exit(DATA_ERROR);
    }
  }
  if (!strcmp("rot_left", command)) {
    if (!(size(args) == 1 && check_int(args->head->data))) {
      fprintf(stderr, "Операция '%s' требует единственнsй аргумент - неотрицательное число!\n", command);
      exit(DATA_ERROR);
    } else {
      rot_left(lst, atoi(args->head->data));
    }
  }
  if (!strcmp("pop_front", command) || !strcmp("pop_back", command)) {
    if (size(args) != 0) {
      fprintf(stderr, "Операция '%s' не подразумевает аргументы!\n", command);
      exit(DATA_ERROR);
    } else {
      if (!strcmp("pop_front", command)) {
        pop_front(lst);
      } else pop_back(lst);
    }
  }
}


// вывод результат преобразования в выходной файл
void export_list(list_t *lst, FILE *result) {
  short int cnt = size(lst);
  fwrite(&cnt, sizeof(short int), 1, result);

  if (lst->head) {
    item *cur_item = lst->head;
    while (cur_item) {
      cur_item->data += '\0';
      size_t length = strlen(cur_item->data);
      fwrite((char *)(&cur_item->data), sizeof(char)*length, 1, result);
      cur_item = cur_item->next;
    }
  }

  for (short int i = 1; i <= cnt; i++) {
    fwrite((const char*) &i, sizeof(short int), 1, result);
  }
}

/* ./lab4mnlN3147 < input.bin > output.txt - команда для работы с перенаправлением ввода-вывода, которое выполняется из командного интерпретатора (bash, sh, ...) (не применяю xargs)*/
    	
int main (int argc, char *argv[]) {

	// включение отладочного вывода $ LAB4DEBUG=1 ./lab4mnlN3147 test.bin
	char *DEBUG = getenv("LAB4DEBUG");
	if (DEBUG) {
		fprintf(stderr, "Включен вывод отладочных сообщений\n");
  }

  // инициализация двусвязного списка
  list_t d;
  d.head = NULL, d.tail = NULL;

  // обработка ввода
  if (argc == 1) {
    fprintf(stderr, "Ошибка: не задано имя файла.\n");
    exit(USAGE_ERROR);
  } else {
    if (!strcmp(argv[1], "-v")) {
      printf("Лукашенко Мария Николаевна, гр. N3147\nВариант: 2-6-14-1\n");
    	exit(EXIT_SUCCESS);
    } else {
      check_file(argv[1]);
    }
  }

  if (read_binary) {
    // вычисление размера файла в байтах
    fseek(file, 0, SEEK_END);
    unsigned int file_size = ftell(file);
    rewind(file);

    // считывание 2-байтного числа
    short int cnt;
    fread(&cnt, sizeof(short int), 1, file);
    if (DEBUG) fprintf(stderr, "cnt = %u\n", cnt);

    // считывание строк
    int str_size = INITIAL_STRING_SIZE;
    char *cur_str = (char*)malloc(str_size * sizeof(char));
    if (!cur_str) {
      fprintf(stderr, "Error allocating memory\n");
      exit(MEMORY_ERROR);
    } 

    // считывание строк
    list_t t;
    t.head = NULL, t.tail = NULL;
    int ind = 0, str_cnt = 0; 
    unsigned long bytes_read = ftell(file);

    while (str_cnt < cnt && (bytes_read = fread(&cur_str[ind], sizeof(char), 1, file)) > 0) {
      if (cur_str[ind] == '\0') {
        if (DEBUG) fprintf(stderr, "Текущая строка: %s\n", cur_str);
        ind = 0;
        ++str_cnt;
        size_t length = strlen(cur_str) + 1;

        if (!check_string(cur_str)) {
          fprintf(stderr, "Некорректный формат ввода!\n");
          exit(DATA_ERROR);
        }

        char* copied_string = malloc(length * sizeof(char));
        strcpy(copied_string, cur_str);
        push_back(&t, copied_string);
      } else {
        ++ind;
        if (ind >= str_size) {
          str_size *= 2; 
          cur_str = realloc(cur_str, str_size); // перевыделение памяти для считываемой строки
          if (!cur_str) {
            fprintf(stderr, "Ошибка выделения памяти\n");
            exit(MEMORY_ERROR);
          }
        }
      }
    }

    bytes_read = ftell(file);
    if (DEBUG) fprintf(stderr, "%u %lu %ld\n", file_size, bytes_read, sizeof(short int));

    // cчитывание произвольных данных
    char variable_data;
    while (bytes_read < file_size - 2*(cnt)) {
      variable_data = fread(&variable_data, sizeof(char), 1, file);
      bytes_read = ftell(file);
    }

    // считывание массива индексов
    short int *indices = (short int*)malloc(cnt * sizeof(short int));
    fread(indices, sizeof(short int), cnt, file);
    if (DEBUG) {
      fprintf(stderr, "Индексы: ");
      for (int i = 0; i < cnt; ++i) {
        fprintf(stderr, "%d ", indices[i]);
      }
      fprintf(stderr, "\n");
    }

    // проверка массива индексов на корректность
    bool ind_cnt[cnt];
    for (int i = 0; i < cnt; i++) {
      ind_cnt[i] = false;
    }

    for (int i = 0; i < cnt; i++) {
      short int ind = indices[i];
      if (ind >= 1 && ind <= cnt) {
        if (ind_cnt[ind - 1]) {
          fprintf(stderr, "Ошибка: некорректный формат ввода.\n");
          exit(DATA_ERROR);
        }
        ind_cnt[ind - 1] = true;
      } else {
        fprintf(stderr, "Ошибка: некорректный формат ввода.\n");
          exit(DATA_ERROR);
      }
    }

    // построение двусвязного списка
    int cur_place = 1;
    while (cur_place <= cnt) {
      for (int i = 0; i < cnt; i++) {
        if (cur_place == indices[i]) {
          push_back(&d, elem_data(&t, i));
          ++cur_place;
        }
      }
    }

    // проверка корректности
    if (DEBUG) {
      fprintf(stderr, "Текущий список:\n");
      print_lst(&d);
      fprintf(stderr, "\n");
    }
  }

  char line[INITIAL_STRING_SIZE];
  
  // считывание и обработка команд
  while (fgets(line, sizeof(line), stdin)) {
    char* command = grep_command(line);
    if (DEBUG) printf("Прочитана строка: %s\n", line);
    if (check_command(command)) {
      if (DEBUG) fprintf(stderr, "Команда: %s\n", command);
      list_t args = grep_args(line, command);
      make_action(&d, command, &args);
    } else {
      fprintf(stderr, "Ошибка: неподдерживаемая команда: '%s'.\n", command);
    }
  }

  if (DEBUG) print_lst(&d);

  FILE *result = fopen("result.bin", "wb");
  if (d.head && d.tail) {
    export_list(&d, result);
  } 
  fclose(result);
  
}
