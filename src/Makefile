TARGETS=lexer parser shell

all: $(TARGETS)

lexer: lex.l
	flex lex.l
	gcc -o lexer -c lex.yy.c

parser: parse.y
	bison -y -d -v parse.y
	gcc -o parser -c y.tab.c

shell: make_cmd.c print_cmd.c dispose_cmd.c localremote.c convert.c new_cost.c copy_cmd.c plan.c execute_cmd.c shell.c
	gcc -Wall -g -std=gnu99 lexer parser make_cmd.c print_cmd.c dispose_cmd.c localremote.c convert.c new_cost.c copy_cmd.c plan.c execute_cmd.c shell.c -o shell -lreadline

clean:
	rm -f $(TARGETS)

