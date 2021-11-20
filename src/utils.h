#ifndef UTILS_H_
#define UTILS_H_

#include <stdarg.h>
#include <bits/stdc++.h>
#include <string>

char * make_message(const char *format, ...);
char * vmake_message(const char *format, va_list ap_);

char * strdup0(const char * s);
char * m_strsignal(int sig);
char * m_strsignal2(int sig);

std::string substitute_prefixed_variables(
  const std::string &str,
  const std::map<char, std::string> &variables,
  char prefix='%',
  bool escape_for_shell=false);

#endif /* UTILS_H_  */
