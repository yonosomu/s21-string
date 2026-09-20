#ifndef S21_STRING_H
#define S21_STRING_H

#include <stddef.h>

typedef size_t s21_size;

#define S21_NULL ((void*)0)

/* Memory functions */
void* s21_memchr(const void* str, int c, s21_size n);
int s21_memcmp(const void* str1, const void* str2, s21_size n);
void* s21_memcpy(void* dest, const void* src, s21_size n);
void* s21_memmove(void* dest, const void* src, s21_size n);
void* s21_memset(void* str, int c, s21_size n);

/* String functions */
s21_size s21_strlen(const char* str);
char* s21_strcpy(char* dest, const char* src);
char* s21_strncpy(char* dest, const char* src, s21_size n);
char* s21_strcat(char* dest, const char* src);
char* s21_strncat(char* dest, const char* src, s21_size n);
int s21_strcmp(const char* str1, const char* str2);
int s21_strncmp(const char* str1, const char* str2, s21_size n);

char* s21_strchr(const char* str, int c);
char* s21_strrchr(const char* str, int c);
char* s21_strstr(const char* haystack, const char* needle);
s21_size s21_strspn(const char* str, const char* accept);
s21_size s21_strcspn(const char* str, const char* reject);
char* s21_strpbrk(const char* str, const char* accept);
char* s21_strtok(char* str, const char* delim);
char* s21_strerror(int errnum);
int s21_sprintf(char* str, const char* format, ...);

#endif
