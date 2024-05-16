.PHONY: all clean

APP=lab4mnlN3147
CFLAGS=-Wall -Wextra -Werror -g

all: $(APP)

$(APP): $(APP).c
	gcc -o $(APP) $(CFLAGS) $(APP).c

clean:
	rm $(APP)

build:
	gcc -o $(APP) -g $(APP).c