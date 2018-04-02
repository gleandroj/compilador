#include <string.h>
#include <assert.h>
#include <regex.h>

#include "log.h"
#include "helpers.h"
#include "memory.h"

char *substr(char *input, int offset, int len)
{
    assert(offset + len < (strlen(input) + 1) && len > 0);
    char *_substr = allocate_memory(sizeof(char) * len);
    strncpy(_substr, input + offset, len);
    _substr[len] = '\0';
    return _substr;
}

Booleano regex_match(char *input, char *pattern, regex_t *matchs)
{
    reg_errcode_t err;

    if ((err = regcomp(matchs, pattern, REG_EXTENDED | REG_NOSUB)) != REG_NOERROR)
    {
        log_fatal("Regex Error code: %d", err);
    }

    log_debug("Testando pattern: %s com string: %s\n", pattern, input);
 
    if ((err = regexec(matchs, input, 0, NULL, 0)) != REG_NOERROR)
    {
        log_info("Resultado: %d, No match!\n", err);
        return FALSE;
    }
    else
    {
        log_info("Resultado: %d, Match!\n", err);
        return TRUE;
    }
}
