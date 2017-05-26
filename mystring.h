#ifndef __MYSTRING_H__
#define __MYSTRING_H__

#include <inttypes.h>
#include <stdlib.h>

#define STRING_NO_MEMORY -1
#define STRING_SUCCESS    0
#define STRING_SIZE 64

typedef struct {
    char cstring[STRING_SIZE];
    int length;
} String;

int string_copy(String *to, String *from);

int string_append(String *string, char c);

int string_cmp(String  *one, String *two);

#endif