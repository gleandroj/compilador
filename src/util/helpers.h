#ifndef __HELPERS_H
#define __HELPERS_H

#include <regex.h>
#include "typings.h"

char *substr(char *input, int offset, int len);

Booleano regex_match(char *input, char *pattern);

#endif