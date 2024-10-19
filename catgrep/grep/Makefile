SHELL := /bin/bash
FLAGS =-Wall -Wextra -Werror
FILE = test1.txt test2.txt
PATTERNS = paattern.reg


all: s21_grep tests

s21_grep: clean
	gcc $(FLAGS) s21_grep.c -o s21_grep

tests:
	-diff <(./s21_grep ok $(FILE)) <(grep ok $(FILE))
	-diff <(./s21_grep -v ok $(FILE)) <(grep -v ok $(FILE))
	-diff <(./s21_grep -i ok $(FILE)) <(grep -i ok $(FILE))
	-diff <(./s21_grep -iv ok $(FILE)) <(grep -iv ok $(FILE))
	-diff <(./s21_grep -in ok $(FILE)) <(grep -in ok $(FILE))
	-diff <(./s21_grep -ie ok $(FILE)) <(grep -ie ok $(FILE))
	-diff <(./s21_grep -ince ok $(FILE)) <(grep -ince ok $(FILE))
	-diff <(./s21_grep -lh ok $(FILE)) <(grep -lh ok $(FILE))
	-diff <(./s21_grep -hc ok $(FILE)) <(grep -hc ok $(FILE))

	-diff <(./s21_grep -e ok -e Ok $(FILE)) <(grep -e ok -e Ok $(FILE))
	-diff <(./s21_grep -s ok bruh) <(grep -s ok bruh)
	-diff <(./s21_grep -oi ok $(FILE)) <(grep -oi ok $(FILE))
	-diff <(./s21_grep -oinc ok $(FILE)) <(grep -oinc ok $(FILE))
	-diff <(./s21_grep -f paattern.reg $(FILE)) <(grep -f paattern.reg $(FILE))
	-diff <(./s21_grep -ohf paattern.reg $(FILE)) <(grep -ohf paattern.reg $(FILE))





clean :
	-rm *.o
