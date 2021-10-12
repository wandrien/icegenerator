/*
 *  meta_parser.cpp -- IceMetaL parser module
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

#ifdef HAVE_CONFIG_H
  #ifndef __main_config_h__
    #define __main_config_h__
    #include "../config.h"
  #endif

  #ifdef HAVE_STDLIB_H
    #include <stdlib.h>
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

#include "meta_parser.h"

cMetaDataItem::cMetaDataItem()
{
  Index = 0;
  Next = Loop = NULL;
}

cMetaDataItem::~cMetaDataItem()
{
}

void cMetaDataItem::SetMeta(int idx)
{
  Index = idx;
}

void cMetaDataItem::SetLoop(int l)
{
  Index = (l | IS_LOOP);
}

void cMetaDataItem::SetEndLoop(cMetaDataItem *p)
{
  Index = ((p->Index & RETRIEVE_DATA) | IS_ENDLOOP);
  Loop = p;
}

void cMetaDataItem::SetNext(cMetaDataItem *p)
{
  Next = p;
}

cMetaDataItem *cMetaDataItem::GetNext(bool always_next)
{
  cMetaDataItem *p;

  if ((Index & IS_ENDLOOP) && !always_next)
  {
    if ((Loop->Index & RETRIEVE_DATA) == 0)
      p = Loop;
    else
    {
      if ((Index & RETRIEVE_DATA) > 0)
      {
        p = Loop;
        Index = (((Index & RETRIEVE_DATA)-1) | IS_ENDLOOP);
      }
      else
      {
        p = Next;
        Index = ((Loop->Index & RETRIEVE_DATA) | IS_ENDLOOP);
      }
    }
  }
  else
    p = Next;

  return p;  
}

int cMetaDataItem::GetMeta()
{
  return Index;
}

cMetaDataList::cMetaDataList()
{
  int i = 0;

  Loops_Stack_Ptr = -1;
  Constants_Idx = 0;
  Root = Ptr = NULL;
  for (i = 0; i < MAX_LOOPS; i++)
    Loops_Stack[i] = NULL;
  for (i = 0; i < MAX_CONSTANTS; i++)
    Constants[i] = NULL;
  
  int_buf = new char[INTERNAL_BUF_SIZE];
}

cMetaDataList::~cMetaDataList()
{
  PurgeMeta();

  delete [] int_buf;
}

bool cMetaDataList::ParseLine(const char *buf, char *key, char *value)
{
  int state = 0, idx, key_idx, value_idx;
  bool err = false;

  key_idx = value_idx = 0;

  for (idx = 0; (!err) && buf[idx] && (buf[idx] != '\n') && (state != 8); idx++)
    switch(state)
    {
      case 0: if (isalpha(buf[idx]))
              {
                key[key_idx++] = toupper(buf[idx]);
                state = 2;
              }
              else if (buf[idx] == '#')
                state = 8;
              else if (buf[idx] == ' ')
                state = 1;
              else
                err = true;
              break;
      case 1: if (isalpha(buf[idx]))
              {
                key[key_idx++] = toupper(buf[idx]);  
                state = 2;
              }
              else if (buf[idx] == '#')
                state = 8;
              else if (buf[idx] != ' ')
                err = true;
              break;
      case 2: if (isalpha(buf[idx]))
                key[key_idx++] = toupper(buf[idx]);
              else if (buf[idx] == ' ')
              {
                key[key_idx] = '\0';
                state = 3;
              }
              else
                err = true;
              break;
      case 3: if (buf[idx] != ' ')
              {
                if (!strcmp(key,"PRINT"))
                {
                  if (isalpha(buf[idx]))
                  {
                    value[value_idx++] = toupper(buf[idx]);
                    state = 4;
                  }
                  else if (buf[idx] == '"')
                    state = 5;
                  else
                    err = true;
                }
                else if (isdigit(buf[idx]) && (!strcmp(key,"LOOP")))
                {
                  value[value_idx++] = buf[idx];
                  state = 6;
                }
                else
                  err = true;
              }
              break;
      case 4: if (isalnum(buf[idx]))
                value[value_idx++] = toupper(buf[idx]);
              else if ((buf[idx] == ' ') || (buf[idx] == '\n'))
                state = 7;
              else
                err = true;
              break;
      case 5: if (buf[idx] == '"')
                state = 7;
              else
                value[value_idx++] = buf[idx];
              break;
      case 6: if (isdigit(buf[idx]))
                value[value_idx++] = buf[idx];
              else if ((buf[idx] == ' ') || (buf[idx] == '\n'))
                state = 7;
              else
                err = true;
              break;
      case 7: if ((buf[idx] != ' ') && (buf[idx] != '\n'))
                err = true;
              break;
    }

  key[key_idx] = '\0';  
  value[value_idx] = '\0';
  
  return err;
}

void cMetaDataList::FillMeta(const char *key, int idx)
{
  cMetaDataItem *temp = new cMetaDataItem;

  if (Root == NULL)
    Root = temp;
  else
    Ptr->SetNext(temp);
    
  Ptr = temp;
  
  if (!strcmp(key,"LOOP"))
  {
    temp->SetLoop(idx);
    Loops_Stack[++Loops_Stack_Ptr] = temp;
  }
  else if (!strcmp(key,"ENDLOOP"))
    temp->SetEndLoop(Loops_Stack[Loops_Stack_Ptr--]);
  else
    temp->SetMeta(idx);
}

#ifdef HAVE_ID3
void cMetaDataList::GetNextMeta(char *buf, struct ID3TableType *ID3)
{
  while ((Ptr != NULL) && (Ptr->GetMeta() & (IS_LOOP | IS_ENDLOOP)))
    Ptr = Ptr->GetNext();
  if (Ptr != NULL)
  {
    if (Ptr->GetMeta() & IS_CONSTANT)
      strcpy(buf,Constants[Ptr->GetMeta() & RETRIEVE_DATA]);
    else
      strcpy(buf,ID3[Ptr->GetMeta() & RETRIEVE_DATA].str);

    Ptr = Ptr->GetNext();
  }
  else
    buf[0] = '\0';
}
#else
void cMetaDataList::GetNextMeta(char *buf)
{
  while ((Ptr != NULL) && (Ptr->GetMeta() & (IS_LOOP | IS_ENDLOOP)))
    Ptr = Ptr->GetNext();
  if (Ptr != NULL)
  {
    strcpy(buf,Constants[Ptr->GetMeta() & RETRIEVE_DATA]);

    Ptr = Ptr->GetNext();
  }
  else
    buf[0] = '\0';
}
#endif

void cMetaDataList::StartMeta()
{
  Ptr = Root;
}

void cMetaDataList::LoadMeta(const char *meta)
{
  FILE *fp = fopen(meta,"r");

  if (fp != NULL)
  {
    char *key, *loc_value;
    int value;

    key = new char[127];
    loc_value = new char[127];
  
    while (fgets(int_buf,INTERNAL_BUF_SIZE,fp) != NULL)
    {
      if (!ParseLine(int_buf,key,loc_value) && (strlen(key) > 0))
      {
        if (!strcmp(key,"PRINT"))
        {
          bool found = false;
          int i;
      
          #ifdef HAVE_ID3        
            for (i = 0; (i < MAX_ID3_TAGS) && (!found); i++)
              found = (!strcmp(loc_value,ID3Table[i].str));
          #else
            found = false;
          #endif
          
          if (found)
            value = i-1;
          else
          { /* found is already false */
            for (i = 0; (i < Constants_Idx) && (!found); i++)
              found = (!strcmp(loc_value,Constants[i]));
            
            if (found)
              value = i | IS_CONSTANT;
            else
            {
              Constants[Constants_Idx] = new char[strlen(loc_value)+1];
              strcpy(Constants[Constants_Idx],loc_value);
              value = Constants_Idx++ | IS_CONSTANT;
            }
          }
        }
        else if (!strcmp(key,"LOOP"))
        {
          if (strlen(loc_value) > 0)
            value = atoi(loc_value);
          else
            value = 0;
        }
        else
          value = 0;
  
        FillMeta(key,value);
      }
    }

    delete [] key;
    delete [] loc_value;
  
    fclose(fp);
  }
}

bool cMetaDataList::IsEmpty()
{
  return (Root == NULL);
}

void cMetaDataList::PurgeMeta()
{
  cMetaDataItem *temp;
  int i;

  for (i = 0; i < Constants_Idx; i++)
  {
    delete [] Constants[i];
    Constants[i] = NULL;
  }
  Constants_Idx = 0;

  Ptr = Root;
  while (Ptr != NULL)
  {
    temp = Ptr;
    Ptr = Ptr->GetNext(true);
    delete temp;
  }

  Root = Ptr = NULL;
}
