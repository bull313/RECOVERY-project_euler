#ifndef _UTIL_H_
#define _UTIL_H_

#define STR_EQUAL(str1, str2) ( (strcmp(str1, str2) == 0) ? TRUE : FALSE )
#define ARR_LEN(arr) ( sizeof(arr) / sizeof(*arr) )
#define BOOL(condition) ( (condition) ? TRUE : FALSE )
#define DEBUG_OPEN printf("\n======== Debug ========\n\n");
#define DEBUG_CLOSE printf("\n=======================\n\n");
#define MAX(v1, v2) ( (v2 > v1) ? v2 : v1 )

#endif
