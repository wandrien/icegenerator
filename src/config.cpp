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

  int_buf = new char[INTERNAL_BUF_SIZE];
  
  if (fp != NULL)
  {
    char *key, *value;
    config_key_type _key;

    key = new char[64];
    value = new char[127];
  
    while (fgets(int_buf,511,fp) != NULL)
      if (!ParseLine(int_buf,key,value) && ((_key = LookupKey(key)) != CONFIG_BADKEY))
        SetValue(_key,value);

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

int cConfig::ParseLine(const char *buf, char *key, char *value)
{
  int i, idx = 0, err = 0, state = 0;

  key[0] = '\0';

  for (i = 0; (buf[i] && !err); i++)
  {
    switch (state)
    {
      case 0: if (buf[i] == '#')		// initial state
                err = 1;			// signal comment
              else if (isalpha(buf[i]))
              {
              	key[idx++] = toupper(buf[i]);	// acquire first char of first sequence
                state = 1;			// acquire first sequence state
              }
              else if (buf[i] != ' ')		// skip initial spaces
                err = -1;
              break;
      case 1: if (buf[i] == ' ')		// do not accept more chars
                state = 2;			// on space
              else if (buf[i] == '=')
              {
                key[idx] = '\0';		// on equal, close first sequence,
                idx = 0;
                state = 3;			// acquire equal state
              }
              else if (isalnum(buf[i]) || (buf[i] == '_'))
                key[idx++] = toupper(buf[i]);	// acquire next char of first sequence
              else
                err = -1;
              break;
      case 2: if (buf[i] == '=')
                state = 3;			// acquire equal state
              else if (buf[i] != ' ')
                err = -1;			// do not accept anything else but spaces
              break;
      case 3: if ((buf[i] != ' ') && !iscntrl(buf[i]))
              {
              	value[idx++] = buf[i];
                state = 4;			// acquire second sequence state
              }
              break;
      case 4: if (isalnum(buf[i]) || ispunct(buf[i]) || (buf[i] == ' '))
                value[idx++] = buf[i];		// acquire next char of second sequence
              break;
    }
  }

  if (state == 0)
    err = 1;

  value[idx] = '\0';				// close second sequence

  return err;					// signal error code
}
