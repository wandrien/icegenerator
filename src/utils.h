#ifndef UTILS_H_
#define UTILS_H_

#include <stdarg.h>

char * make_message(const char *format, ...);
char * vmake_message(const char *format, va_list ap_);

char * strdup0(const char * s);
char * m_strsignal(int sig);
char * m_strsignal2(int sig);

#endif /* UTILS_H_  */
