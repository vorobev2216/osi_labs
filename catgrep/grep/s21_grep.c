#include "s21_grep.h"

#include <getopt.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

arguments flag_parser(int argc, char **argv);
void read_file(arguments arg, char *path, regex_t *reg);

void add_pattern(arguments *arg, char *pattern) {
  if (arg->len_pattern != 0) {
    strcat(arg->pattern + arg->len_pattern, "|");
    arg->len_pattern++;
  }
  arg->len_pattern += sprintf(arg->pattern + arg->len_pattern, "(%s)", pattern);
}

void outline(char *line, int n) {
  for (int i = 0; i < n; i++) {
    putchar(line[i]);
  }
  if (line[n - 1] != '\n') putchar('\n');
}

void print_match(regex_t *re, char *line) {
  regmatch_t match;
  int offset = 0;
  while (1) {
    int result = regexec(re, line + offset, 1, &match, 0);

    if (result != 0) {
      break;
    }
    for (int i = match.rm_so; i < match.rm_eo; i++) {
      putchar(line[i]);
    }
    putchar('\n');
    offset += match.rm_eo;
  }
}

void output(arguments arg, int argc, char **argv) {
  regex_t reg;
  int errore = regcomp(&reg, arg.pattern, REG_EXTENDED | arg.i);
  if (errore) perror("Errore");
  for (int i = optind; i < argc; i++) {
    read_file(arg, argv[i], &reg);
  }
  regfree(&reg);
}

void add_reg_file(arguments *arg, char *path) {
  FILE *f = fopen(path, "r");
  if (f == NULL) {
    if (!arg->s) perror(path);
    exit(1);
  }

  char *line = NULL;
  size_t mem_len = 0;
  int read = getline(&line, &mem_len, f);

  while (read != -1) {
    if (line[read - 1] == '\n') line[read - 1] = '\0';
    add_pattern(arg, line);
    read = getline(&line, &mem_len, f);
  }
  free(line);
  fclose(f);
}

void read_file(arguments arg, char *path, regex_t *reg) {
  FILE *f = fopen(path, "r");
  if (f == NULL) {
    if (!arg.s) perror(path);
    exit(1);
  }
  char *line = NULL;
  size_t mem_len = 0;
  int read = 0;
  read = getline(&line, &mem_len, f);
  int line_count = 1;
  int c = 0;
  while (read != -1) {
    int result = regexec(reg, line, 0, NULL, 0);
    if ((result == 0 && !arg.v) || (arg.v && result != 0)) {
      if (!arg.c && !arg.l) {
        if (!arg.h) printf("%s:", path);
        if (arg.n) printf("%d:", line_count);
        if (arg.o) {
          print_match(reg, line);
        } else {
          outline(line, read);
        }
      }
      c++;
    }
    read = getline(&line, &mem_len, f);
    line_count++;
  }
  free(line);
  if (arg.c && !arg.l) {
    if (!arg.h) printf("%s:", path);
    printf("%d\n", c);
  }
  if (arg.l && c > 0) printf("%s\n", path);

  fclose(f);
}

arguments flag_parser(int argc, char **argv) {
  arguments arg = {0};
  int opt;
  while ((opt = getopt(argc, argv, "e:ivclnhsf:o")) != -1) {
    switch (opt) {
      case 'e':
        arg.e = 1;
        add_pattern(&arg, optarg);
        break;
      case 'i':
        arg.i = REG_ICASE;
        break;
      case 'v':
        arg.v = 1;
        break;
      case 'c':
        arg.c = 1;
        break;
      case 'l':
        arg.c = 1;
        arg.l = 1;
        break;
      case 'n':
        arg.n = 1;
        break;
      case 'h':
        arg.h = 1;
        break;
      case 's':
        arg.s = 1;
        break;
      case 'f':
        arg.f = 1;
        add_reg_file(&arg, optarg);
        break;
      case 'o':
        arg.o = 1;
        break;
      default:
        break;
    }
  }
  if (arg.len_pattern == 0) {
    add_pattern(&arg, argv[optind]);
    optind++;
  }
  if (argc - optind == 1) {
    arg.h = 1;
  }
  return arg;
}

int main(int argc, char *argv[]) {
  arguments arg = flag_parser(argc, argv);
  output(arg, argc, argv);
  return 0;
}