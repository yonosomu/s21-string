#include "s21_string.h"

s21_size s21_strlen(const char* str) {
  s21_size length = 0;

  while (str[length] != '\0') length++;

  return length;
}

char* s21_strcpy(char* dest, const char* src) {
  s21_size i = 0;

  while (src[i] != '\0') {
    dest[i] = src[i];
    i++;
  }

  dest[i] = '\0';

  return dest;
}

char* s21_strncpy(char* dest, const char* src, s21_size n) {
  s21_size i = 0;

  while (i < n && src[i] != '\0') {
    dest[i] = src[i];
    i++;
  }

  while (i < n) {
    dest[i] = '\0';
    i++;
  }

  return dest;
}

char* s21_strcat(char* dest, const char* src) {
  s21_size i = 0;
  s21_size j = 0;

  while (dest[i] != '\0') i++;

  while (src[j] != '\0') {
    dest[i] = src[j];
    i++;
    j++;
  }

  dest[i] = '\0';

  return dest;
}

char* s21_strncat(char* dest, const char* src, s21_size n) {
  s21_size i = 0;
  s21_size j = 0;

  while (dest[i] != '\0') i++;

  while (j < n && src[j] != '\0') {
    dest[i] = src[j];
    i++;
    j++;
  }

  dest[i] = '\0';

  return dest;
}

int s21_strcmp(const char* str1, const char* str2) {
  s21_size i = 0;

  while (str1[i] != '\0' && str1[i] == str2[i]) i++;

  return (unsigned char)str1[i] - (unsigned char)str2[i];
}

int s21_strncmp(const char* str1, const char* str2, s21_size n) {
  s21_size i = 0;

  while (i < n && str1[i] != '\0' && str1[i] == str2[i]) i++;

  if (i == n) return 0;

  return (unsigned char)str1[i] - (unsigned char)str2[i];
}

static int s21_char_in_set(unsigned char ch, const char* set) {
  if (set == S21_NULL) return 0;
  while (*set) {
    if ((unsigned char)*set == ch) return 1;
    set++;
  }
  return 0;
}

char* s21_strchr(const char* str, int c) {
  unsigned char uc = (unsigned char)c;

  while ((unsigned char)*str != uc) {
    if (*str == '\0') {
      return S21_NULL;
    }
    str++;
  }

  return (char*)str;
}

char* s21_strrchr(const char* str, int c) {
  unsigned char uc = (unsigned char)c;
  const char* last = S21_NULL;

  while (1) {
    if ((unsigned char)*str == uc) {
      last = str;
    }

    if (*str == '\0') {
      break;
    }

    str++;
  }

  return (char*)last;
}

char* s21_strstr(const char* haystack, const char* needle) {
  if (*needle == '\0') {
    return (char*)haystack;
  }

  while (*haystack) {
    if ((unsigned char)*haystack == (unsigned char)*needle) {
      const char* h = haystack;
      const char* n = needle;

      while (*h && *n && (unsigned char)*h == (unsigned char)*n) {
        h++;
        n++;
      }

      if (*n == '\0') {
        return (char*)haystack;
      }

      if (*h == '\0') {
        return S21_NULL;
      }
    }

    haystack++;
  }

  return S21_NULL;
}

s21_size s21_strspn(const char* str, const char* accept) {
  s21_size i = 0;

  while (str[i] && s21_char_in_set((unsigned char)str[i], accept)) {
    i++;
  }

  return i;
}

s21_size s21_strcspn(const char* str, const char* reject) {
  s21_size i = 0;

  while (str[i] && !s21_char_in_set((unsigned char)str[i], reject)) {
    i++;
  }

  return i;
}

char* s21_strpbrk(const char* str, const char* accept) {
  while (*str) {
    if (s21_char_in_set((unsigned char)*str, accept)) {
      return (char*)str;
    }
    str++;
  }

  return S21_NULL;
}

char* s21_strtok(char* str, const char* delim) {
  static char* state = S21_NULL;
  char* token;

  if (str) {
    state = str;
  }

  if (!state) {
    return S21_NULL;
  }

  while (*state && s21_char_in_set((unsigned char)*state, delim)) {
    state++;
  }

  if (!*state) {
    state = S21_NULL;
    return S21_NULL;
  }

  token = state;

  while (*state && !s21_char_in_set((unsigned char)*state, delim)) {
    state++;
  }

  if (*state) {
    *state = '\0';
    state++;
  } else {
    state = S21_NULL;
  }

  return token;
}
