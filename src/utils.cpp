#include "utils.h"
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

char * vmake_message(const char *format, va_list ap_)
{
  int size = 0;
  char *msg = NULL;
  va_list ap;

  va_copy(ap, ap_);
  size = vsnprintf(msg, size, format, ap);
  va_end(ap);

  if (size < 0)
    return NULL;

  size++; /* for '\0' */
  msg = (char *) malloc(size);
  if (msg == NULL)
    return NULL;

  va_copy(ap, ap_);
  size = vsnprintf(msg, size, format, ap);
  va_end(ap);

  if (size < 0) {
    free(msg);
    return NULL;
  }

  return msg;
}

char * make_message(const char *format, ...)
{
  va_list ap;
  va_start(ap, format);
  char * msg = vmake_message(format, ap);
  va_end(ap);
  return msg;
}

char * strdup0(const char * s)
{
  if (s)
    return strdup(s);
  return NULL;
}

char * m_strsignal(int sig)
{
  return strdup0(strsignal(sig));
}

char * m_strsignal2(int sig)
{
  char * signal_name = m_strsignal(sig);
  char * signal_name2 = NULL;
  if (signal_name) {
    signal_name2 = make_message("%s(%d)", signal_name, sig);
  } else {
    signal_name2 = make_message("%d", sig);
  }
  free(signal_name);
  return signal_name2;
}

