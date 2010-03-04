/**
 * Standard include file and asm implementation.
 *
 * Copyright (C) 2007-2008, Bernhard Kauer <kauer@tudos.org>
 *
 * This file is part of Florence2.
 *
 * Florence2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * Florence2 is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License version 2 for more details.
 */

#pragma once

static inline void *
memcpy(void *dst, const void *src, unsigned long count) {
  void *res = dst;
  if (count & 3) {
      unsigned long x = count & 3;
      asm volatile ("1: movsb; loop 1b;" : "+D"(dst), "+S"(src), "+c" (x) : : "memory");
  }
  count /= 4;
  asm volatile ("rep movsl" : "+D"(dst), "+S"(src), "+c" (count) : : "memory");
  return res;
}


static inline int
memcmp(const void *dst, const void *src, long count) {
  return __builtin_memcmp(dst, src, count);
}


static inline unsigned long
strlen(const char *src) {
  return __builtin_strlen(src);
}


static inline void *
memmove(void *dst, const void *src, long count) {

  char *d = reinterpret_cast<char *>(dst);
  const char *s = reinterpret_cast<const char *>(src);
  if (d > s)
    {
      d+=count-1;
      s+=count-1;
      asm volatile ("std; rep movsb; cld;" : "+D"(d), "+S"(s), "+c"(count) : : "memory");
    }
  else
    memcpy(d, s, count);
  return dst;
}


static inline void *
memset(void *dst, char n, long count) {
  void *res = dst;
  unsigned long x = count & 3;
  if (x) asm volatile ("1: stosb; loop 1b;" : "+D"(dst), "+c" (x) : "a"(n) : "memory");

  unsigned value = n & 0xff;
  value = (value << 24) | (value << 16) | (value << 8) | value;
  count /= 4;
  asm volatile ("rep stosl" : "+D"(dst), "+c"(count) : "a"(value)  : "memory");
  return res;
}


static inline unsigned long
strnlen(const char *src, unsigned long maxlen) {

  unsigned long count = maxlen;
  unsigned char ch = 0;
  asm volatile ("repne scasb; setz %0;" : "+a"(ch), "+D"(src), "+c"(count));
  if (ch) count--;
  return maxlen - count;
}


static inline char *
strcpy(char *dst, const char *src) {
  char *res = dst;
  asm volatile ("1: lodsb; test %%al, %%al; stosb; jnz 1b;  " : "+D"(dst), "+S"(src) : : "eax", "memory", "cc");
  return res;
}


static inline char *
strstr(char *haystack, const char *needle) {

  int index;
  do {
    for (index=0; needle[index] == haystack[index] && needle[index]; index++)
      ;
    if (!needle[index])
      return haystack;
    haystack += index ? index : 1;
  } while (*haystack);
  return 0;

}


static inline unsigned long
strtoul(char *nptr, char **endptr, int base) {

  unsigned long res = 0;
  if (*nptr == '0' && *(nptr+1) == 'x')
    {
      nptr += 2;
      base = base ? base : 16;
    }
  else if (*nptr == '0')
    base = base ? base : 8;
  else
    base = base ? base : 10;

  while (*nptr)
    {
      long val = *nptr - '0';
      if (val > 9)
	val = val - 'a' + '0' + 10;
      if (val < 0 || val > base)
	break;
      res = res*base + val;
      nptr++;
    }
  if (endptr) *endptr = nptr;
  return res;
}


static inline const char *
strchr(const char *s, int c) {

  while (*s)
    if (c == *s)
      return s;
    else s++;
  return 0;
}


static inline char *
strsep(char **stringp, const char *delim) {

  if (!stringp || !*stringp)  return 0;
  char *res = *stringp;
  char *s = res;
  while (*s)
    {
      if (strchr(delim, *s))
	{
	  *s = 0;
	  *stringp = s+1;
	  break;
	}
      s++;
    }
  if (res == *stringp)
    *stringp = 0;
  return res;

}


static inline int
strcmp(const char *dst, const char *src) {
  return memcmp(dst, src, strlen(dst));
}