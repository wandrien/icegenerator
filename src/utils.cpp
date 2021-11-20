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

/*******************************************************************************/

std::string escapeshellarg(const std::string &str)
{
  std::string result;

  if (str.size() == 0) {
    result = "\'\'";
    return result;
  }

  int estimated_size = 0;
  for(const char& c : str) {
    if (c == '\'')
      estimated_size += 4;
    else
      estimated_size += 1;
  }

  result.reserve(estimated_size);

  bool inside_quotes = false;
  for(const char& c : str) {
    if (c == '\'') {
      if (inside_quotes) {
        result += '\'';
        inside_quotes = false;
      }
      result += "\\\'";
    } else {
      if (!inside_quotes) {
        result += '\'';
        inside_quotes = true;
      }
      result += c;
    }
  }

  if (inside_quotes) {
    result += '\'';
    inside_quotes = false;
  }

  return result;
}

#ifdef INSIDE_CATCH

struct TestStringPair {
  const char * str;
  const char * result;
};

TEST_CASE( "escapeshellarg") {

  auto testData = GENERATE(
    TestStringPair {"", "\'\'"},
    TestStringPair {"\'", "\\\'"},
    TestStringPair {" ", "\' \'"},
    TestStringPair {"\"\"", "\'\"\"\'"},
    TestStringPair {"\'a", "\\\'\'a\'"},
    TestStringPair {"a\'", "\'a\'\\\'"},
    TestStringPair {"a b c \'\'\' 1 2 3", "\'a b c \'\\\'\\\'\\\'\' 1 2 3\'"},
    TestStringPair {"/abc/d/", "\'/abc/d/\'"}
  );

  REQUIRE(escapeshellarg(testData.str) == std::string(testData.result));
}

#endif

/*******************************************************************************/

std::string substitute_prefixed_variables(const std::string &str, const std::map<char, std::string> &variables, char prefix, bool escape_for_shell)
{
  std::string result;
  result.reserve(str.size() * 2);

  bool in_prefix = false;
  for(const char& c : str) {
    if (in_prefix) {
      if (c == prefix) {
        result += c;
      } else {
        auto search = variables.find(c);
        if (search != variables.end()) {
            if (escape_for_shell) {
              result += escapeshellarg(search->second);
            } else {
              result += search->second;
            }
        }
      }
      in_prefix = false;
    } else {
      if (c == prefix) {
        in_prefix = true;
      } else {
        result += c;
      }
    }
  }

  return result;
}

#ifdef INSIDE_CATCH

struct TestData {
  const char * str;
  bool escape_for_shell;
  const char * result;
};

TEST_CASE( "substitute_prefixed_variables") {

  auto testData = GENERATE(
    TestData {"/abc/d%% %1 %2 %3", false, "/abc/d% file file \' "},
    TestData {"/abc/d%% %1 %2 %3", true, "/abc/d% \'file\' \'file \'\\\' \'\'"},
    TestData {"", true, ""},
    TestData {"%1", true, "\'file\'"},
    TestData {"%%", true, "%"}
  );

  std::map<char, std::string> variables{
    { '1', "file"},
    { '2', "file \'"},
    { '3', ""},
  };

  REQUIRE(
    substitute_prefixed_variables(testData.str, variables, '%', testData.escape_for_shell)
    ==
    std::string(testData.result)
  );
}

#endif

/*******************************************************************************/
