#include "s21_sprintf.h"

#include <float.h>

#include "s21_string.h"

static char* s21_lltos(long long n) {
  char* str = NULL;
  if (n == 0) {
    str = calloc(2, sizeof(char));
    if (str) str[0] = '0';
  } else if (n == LLONG_MIN) {
    str = calloc(21, sizeof(char));
    if (str) s21_strcpy(str, "-9223372036854775808");
  } else {
    int is_neg = n < 0;
    if (is_neg) n = -n;

    long long dup = n;
    int len = 0;
    if (is_neg) len++;
    for (; dup > 0; dup /= 10) len++;
    str = calloc(len + 1, sizeof(char));
    for (int i = len - 1; str && n > 0; n /= 10) str[i--] = '0' + (n % 10);

    if (str && is_neg) str[0] = '-';
  }
  return str;
}

static char* s21_ulltos(unsigned long long n) {
  char* str = NULL;
  if (n == 0) {
    str = calloc(2, sizeof(char));
    if (str) str[0] = '0';
  } else {
    unsigned long long dup = n;
    int len = 0;
    for (; dup > 0; dup /= 10) len++;
    str = calloc(len + 1, sizeof(char));
    for (int i = len - 1; str && n > 0; n /= 10) str[i--] = '0' + (n % 10);
  }
  return str;
}

/* Exact decimal expansion of a double, stored least significant digit first. */
static int s21_float_digits(double value, unsigned char* digits, int* scale) {
  int exponent = 0;
  double fraction = frexp(value, &exponent);
  unsigned long long mantissa =
      (unsigned long long)ldexp(fraction, DBL_MANT_DIG);
  exponent -= DBL_MANT_DIG;
  int len = 0;
  do {
    digits[len++] = mantissa % 10;
    mantissa /= 10;
  } while (mantissa);
  *scale = exponent < 0 ? -exponent : 0;
  int factor = exponent < 0 ? 5 : 2;
  int steps = exponent < 0 ? -exponent : exponent;
  for (int step = 0; step < steps; ++step) {
    int carry = 0;
    for (int i = 0; i < len; ++i) {
      int digit = digits[i] * factor + carry;
      digits[i] = digit % 10;
      carry = digit / 10;
    }
    if (carry) digits[len++] = carry;
  }
  return len;
}

static char* s21_ftos_norm(long double n, int precision) {
  unsigned char digits[DBL_MANT_DIG - DBL_MIN_EXP + DBL_MAX_10_EXP + 4] = {0};
  int scale = 0;
  int len = s21_float_digits(fabs((double)n), digits, &scale);
  int cut = scale - precision;
  if (cut > 0) {
    int sticky = 0;
    for (int i = 0; i < cut - 1; ++i) sticky |= digits[i];
    /* Round to nearest; exact halfway cases go to an even last digit. */
    int round_up = digits[cut - 1] > 5 ||
                   (digits[cut - 1] == 5 && (sticky || digits[cut] % 2));
    for (int i = cut; round_up; ++i) {
      digits[i]++;
      round_up = digits[i] == 10;
      if (round_up) digits[i] = 0;
      if (i >= len) len = i + 1;
    }
  }
  int negative = signbit(n) ? 1 : 0;
  int integer_len = len > scale ? len - scale : 1;
  char* str = calloc(
      (size_t)negative + integer_len + (precision > 0) + (size_t)precision + 1,
      1);
  if (str) {
    int pos = 0;
    if (negative) str[pos++] = '-';
    for (int i = integer_len - 1; i >= 0; --i)
      str[pos++] = '0' + digits[scale + i];
    if (precision > 0) str[pos++] = '.';
    for (int i = 0; i < precision; ++i)
      str[pos++] = '0' + (i < scale ? digits[scale - i - 1] : 0);
  }
  return str;
}

static char* s21_ftos(long double n, int precision) {
  char* str = NULL;
  if (isnan(n)) {
    int negative = 0;
#ifndef __APPLE__
    negative = signbit(n) ? 1 : 0;
#endif
    str = calloc(4 + negative, sizeof(char));
    if (str) {
      if (negative) str[0] = '-';
      s21_strcpy(str + negative, "nan");
    }
  } else if (isinf(n)) {
    if (n > 0) {
      str = calloc(4, sizeof(char));
      if (str) s21_strcpy(str, "inf");
    } else {
      str = calloc(5, sizeof(char));
      if (str) s21_strcpy(str, "-inf");
    }
  } else {
    str = s21_ftos_norm(n, precision);
  }
  return str;
}

static const char* s21_parsingSpecifier(Specifier* sp, const char* format) {
  format++;
  while (*format == '-' || *format == '+' || *format == ' ') {
    if (*format == '-')
      sp->minus = 1;
    else if (*format == '+')
      sp->plus = 1;
    else if (*format == ' ')
      sp->space = 1;
    format++;
  }
  while (*format >= '0' && *format <= '9') {
    sp->width = sp->width * 10 + (*format - '0');
    format++;
  }
  if (*format == '.') {
    sp->flag_precision = 1;
    format++;
  }
  while (sp->flag_precision && *format >= '0' && *format <= '9') {
    sp->precision = sp->precision * 10 + (*format - '0');
    format++;
  }
  if (*format == 'h' || *format == 'l') {
    sp->length = *format;
    format++;
  }
  sp->specifier = *format;
  format++;
  if (!sp->flag_precision && sp->specifier == 'f') sp->precision = 6;
  return format;
}

static char* s21_vtos(const Value* val, const Specifier* sp) {
  char* str = NULL;
  if (sp->specifier == 'd') {
    str = s21_lltos(val->ll);
  } else if (sp->specifier == 'f') {
    str = s21_ftos(val->f, sp->precision);
  } else if (sp->specifier == 'u') {
    str = s21_ulltos(val->ull);
  } else if (sp->specifier == 'c') {
    str = calloc(2, sizeof(char));
    if (str) str[0] = val->c;
  } else if (sp->specifier == 's') {
    s21_size len = 0;
    while ((!sp->flag_precision || len < (s21_size)sp->precision) &&
           val->s[len])
      len++;
    str = calloc(len + 1, sizeof(char));
    if (str) s21_memcpy(str, val->s, len);
  }
  return str;
}

static char* s21_addZero(char* str, int* len, const Specifier* sp) {
  char* new_str = NULL;
  if (str[0] == '0' && str[1] == '\0' && sp->precision == 0) {
    new_str = calloc(1, sizeof(char));
    *len = 0;
    if (new_str)
      free(str);
    else
      new_str = str;
  } else {
    int is_neg = str[0] == '-';
    new_str = calloc(is_neg + sp->precision + 1, sizeof(char));
    for (int i = 0; new_str && i < sp->precision; i++)
      if (i < *len - is_neg)
        new_str[sp->precision + is_neg - 1 - i] = str[*len - 1 - i];
      else
        new_str[sp->precision + is_neg - 1 - i] = '0';
    if (new_str) {
      if (is_neg) new_str[0] = '-';
      free(str);
      *len = is_neg + sp->precision;
    } else {
      new_str = str;
    }
  }
  return new_str;
}

static char* s21_addSpace(char* str, int* len, const Specifier* sp) {
  char* new_str = calloc(sp->width + 1, sizeof(char));
  if (sp->minus) {
    for (int i = 0; new_str && i < sp->width; i++)
      if (i < *len)
        new_str[i] = str[i];
      else
        new_str[i] = ' ';
  } else {
    for (int i = 0; new_str && i < sp->width; i++)
      if (i < *len)
        new_str[sp->width - 1 - i] = str[*len - 1 - i];
      else
        new_str[sp->width - 1 - i] = ' ';
  }
  if (new_str) {
    *len = sp->width;
    free(str);
  } else {
    new_str = str;
  }
  return new_str;
}

static char* s21_addPlusOrSpace(char* str, int* len, const Specifier* sp) {
  char* new_str = str;
  if (str[0] != '-') {
    new_str = calloc(1 + *len + 1, sizeof(char));
    if (new_str) {
      if (sp->plus) {
        new_str[0] = '+';
        (*len)++;
      } else if (sp->space) {
        new_str[0] = ' ';
        (*len)++;
      }
      s21_strcat(new_str, str);
      free(str);
    } else {
      new_str = str;
    }
  }
  return new_str;
}

static void s21_catVal(char* str, int* size, const Value* val,
                       const Specifier* sp) {
  char* str_val = s21_vtos(val, sp);
  if (!str_val) return;

  int len = sp->specifier == 'c' ? 1 : (int)s21_strlen(str_val);

  if ((sp->specifier == 'd' || sp->specifier == 'u') &&
      (len <= sp->precision ||
       (sp->precision == 0 && str_val[0] == '0' && str_val[1] == '\0')) &&
      sp->flag_precision)
    str_val = s21_addZero(str_val, &len, sp);

  int use_sign = sp->specifier == 'd' || sp->specifier == 'f';
#ifdef __APPLE__
  if (sp->specifier == 'f' && isnan(val->f)) use_sign = 0;
#endif
  if (use_sign && (sp->plus || sp->space))
    str_val = s21_addPlusOrSpace(str_val, &len, sp);

  if (len < sp->width) str_val = s21_addSpace(str_val, &len, sp);

  s21_memcpy(str + *size, str_val, len);
  *size += len;

  free(str_val);
}

int s21_sprintf(char* str, const char* format, ...) {
  va_list args;
  va_start(args, format);
  int size = 0;
  while (*format) {
    if (*format != '%') {
      str[size++] = *format;
      format++;
    } else {
      Specifier sp = {0};
      format = s21_parsingSpecifier(&sp, format);
      Value val = {0};
      if (sp.specifier == 'c') {
        val.c = (char)va_arg(args, int);
      } else if (sp.length == 'h' && sp.specifier == 'd') {
        val.ll = (short)va_arg(args, int);
      } else if (sp.length == 'l' && sp.specifier == 'd') {
        val.ll = (long long)va_arg(args, long);
      } else if (sp.specifier == 'd') {
        val.ll = (long long)va_arg(args, int);
      } else if (sp.length == 'l' && sp.specifier == 'f') {
        val.f = (long double)va_arg(args, double);
      } else if (sp.specifier == 'f') {
        val.f = (long double)va_arg(args, double);
      } else if (sp.specifier == 's') {
        val.s = va_arg(args, char*);
      } else if (sp.length == 'h' && sp.specifier == 'u') {
        val.ull = (unsigned short)va_arg(args, int);
      } else if (sp.length == 'l' && sp.specifier == 'u') {
        val.ull = (unsigned long long)va_arg(args, unsigned long);
      } else if (sp.specifier == 'u') {
        val.ull = (unsigned long long)va_arg(args, unsigned int);
      } else if (sp.specifier == '%') {
        str[size++] = '%';
      }
      s21_catVal(str, &size, &val, &sp);
    }
  }
  str[size] = '\0';
  va_end(args);
  return size;
}
