#ifndef H_ASSERTION
#define H_ASSERTION

    #include "logger.h"

    #define E_ASSERT(EXPRESSION, MESSAGE)\
            if(!(EXPRESSION)) {LOG_ERROR("Assertion Failed. Expression: %s - Message: %s - File: %s, Line: %d\n", #EXPRESSION, MESSAGE, (__FILE__), __LINE__);}

#endif