/*
 *  config.cpp -- configuration file parser module
 *
 *  Copyright (C) 2004 Tony Sin(x) '76 <administrator@tortugalabs.it>
 *  All rights reserved.
 *
 */

/*
 *			 GNU GENERAL PUBLIC LICENSE
 *			    Version 2, June 1991
 *
 *  Copyright (C) 1989, 1991 Free Software Foundation, Inc.
 *                           675 Mass Ave, Cambridge, MA 02139, USA
 *  Everyone is permitted to copy and distribute verbatim copies
 *  of this license document, but changing it is not allowed.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.

 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.

 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

/* ****************************************************************************** */
/* Configuration file parser							  */
/* ****************************************************************************** */

#ifdef HAVE_CONFIG_H
  #ifndef __main_config_h__
    #define __main_config_h__
    #include "../config.h"
  #endif

  #ifdef HAVE_STRING_H
    #include <string.h>
  #endif
	
  #ifdef HAVE_STDIO_H
    #include <stdio.h>
  #endif
	
  #ifdef HAVE_CTYPE_H
    #include <ctype.h>
  #endif
	
#endif

#include <stdlib.h>
#include "config.h"

static const char * keys[MAX_KEYS] = {
    #define CONFIG_KEY_FIRST(key) #key,
    #define CONFIG_KEY(key) #key,
    #define CONFIG_KEY_LAST(key) #key
    #include "config_keys.h"
    #undef CONFIG_KEY_FIRST
    #undef CONFIG_KEY
    #undef CONFIG_KEY_LAST
};

cConfig::cConfig(const char *path)
{
  int i;
  FILE *fp = fopen(path,"r");

  for (i = ((int) CONFIG_IP); i < ((int) CONFIG_BADKEY); Table[i++] = NULL);

  int_buf = new char[CONFIG_MAX_LINE_SIZE];

  if (fp != NULL)
  {
    char *key, *value;
    config_key_type config_key;

    key = new char[CONFIG_MAX_LINE_SIZE];
    value = new char[CONFIG_MAX_LINE_SIZE];

    while (fgets(int_buf, CONFIG_MAX_LINE_SIZE - 1,  fp) != NULL) {
      if (ParseLine(int_buf, key, CONFIG_MAX_LINE_SIZE, value, CONFIG_MAX_LINE_SIZE) != 0)
        continue;
      config_key = LookupKey(key);
      if (config_key == CONFIG_BADKEY)
        continue;
      SetValue(config_key, value);
    }

    fclose(fp);

    delete [] key;
    delete [] value;
  }
  else
  {
    strcpy(ErrStr,"Configuration file does not exist");
    throw 0;
  }
}

cConfig::~cConfig()
{
  int i;

  for (i = ((int) CONFIG_IP); i < ((int) CONFIG_BADKEY); i++)
    if (Table[i] != NULL)
      delete [] Table[i];
    
  delete [] int_buf;
}

config_key_type cConfig::LookupKey(const char *key) const
{
  int result;

  for (result = ((int) CONFIG_IP); (result < ((int) CONFIG_BADKEY)) && (strcmp(key,keys[result])); result++);

  if (result == CONFIG_BADKEY)
  {
    strcpy(ErrStr,"Invalid configuration parameter in config file");
    throw 0;
  }
  else
    return ((config_key_type) result);
}

void cConfig::SetValue(config_key_type key, const char *value)
{
  if ((value != NULL) && (strlen(value) > 0))
  {
    if (Table[key] != NULL)
    {
      delete [] Table[key];
      Table[key] = NULL;
    }
  
    if (Table[key] == NULL)
      Table[key] = new char[strlen(value)+1];
    strcpy(Table[key],value);
  }
}

char * cConfig::GetValue(config_key_type key) const
{
  return Table[key];
}

int cConfig::GetIntValue(config_key_type key, int default_value) const
{
  char * value = GetValue(key);
  if (value)
    return atoi(value);
  return default_value;
}

int cConfig::ParseLine(const char *buf, char *key, ssize_t key_size, char *value, ssize_t value_size)
{
  int i, err = 0, state = 0;
  int key_idx = 0;
  int value_idx = 0;

  if (!key || !value || key_size < 1|| value_size < 1)
    return -1;

  key[0] = '\0';
  value[0] = '\0';

  for (i = 0; (buf[i] && !err); i++)
  {
    if (key_idx >= key_size - 1 || value_idx >= value_size - 1) {
       err = -1;
       break;
    }

    char c = buf[i];

    switch (state)
    {
      case 0: // initial state
              if (c == '#') { // signal comment
                err = 1;
              }
              else if (isalpha(c)) { // first char of the KEY
                key[key_idx++] = toupper(c);
                state = 1;
              }
              else if (c != ' ') { // skip initial spaces
                err = -1;
              }
              break;
      case 1: // read the KEY
              if (c == ' ') { // end of the KEY
                state = 2;
              }
              else if (c == '=') { // end of the KEY and "=", so skip state 2
                state = 3;
              }
              else if (isalnum(c) || (c == '_')) {
                key[key_idx++] = toupper(c);
              }
              else {
                err = -1;
              }
              break;
      case 2: // read "="
              // do not accept anything else but spaces and single "="
              if (c == '=')
                state = 3;
              else if (c != ' ')
                err = -1;
              break;
      case 3: // read spaces after "="
              if ((c != ' ') && !iscntrl(c))
              {
                value[value_idx++] = c;
                state = 4;
              }
              break;
      case 4: // read the VALUE
              if (c != '\n' && c != '\r')
                value[value_idx++] = c;
              break;
    }
  }

  if (state == 0)
    err = 1;

  key[key_idx] = '\0';
  value[value_idx] = '\0';

  return err;
}
