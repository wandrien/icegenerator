/*
 *  meta_parser.h -- IceMetaL parser module (include)
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

#ifndef __meta_parser_h__
#define __meta_parser_h__

#ifdef HAVE_CONFIG_H
  #ifndef __main_config_h__
    #define __main_config_h__
    #include "../config.h"
  #endif
#endif

#include "circular.h"

#define MAX_LOOPS 20
#define MAX_CONSTANTS 40

#define IS_LOOP 0x4000
#define IS_ENDLOOP 0x8000
#define IS_CONSTANT 0x2000
#define RETRIEVE_DATA 0x1FFF

#define INTERNAL_BUF_SIZE 512

class cMetaDataItem
{
  int Index;
  cMetaDataItem *Next, *Loop;
public:
  cMetaDataItem();
  ~cMetaDataItem();
  void SetMeta(int idx);
  void SetLoop(int l);
  void SetEndLoop(cMetaDataItem *p);
  void SetNext(cMetaDataItem *p);
  cMetaDataItem* GetNext(bool always_next = false);
  int GetMeta();
};

class cMetaDataList
{
  char *int_buf;
  int Loops_Stack_Ptr, Constants_Idx;
  cMetaDataItem *Root, *Ptr;
  cMetaDataItem *Loops_Stack[MAX_LOOPS];
  char *Constants[MAX_CONSTANTS];
  void FillMeta(const char *key, int idx);
  bool ParseLine(const char *buf, char *key, char *value);
public:
  cMetaDataList();
  ~cMetaDataList();
  #ifdef HAVE_ID3
    void GetNextMeta(char *buf, struct ID3TableType *ID3);
  #else
    void GetNextMeta(char *buf);
  #endif
  void StartMeta();
  void LoadMeta(const char *meta);
  bool IsEmpty();
  void PurgeMeta();
};

#ifdef HAVE_ID3
  extern struct ID3TableType ID3Table[];
#endif
extern char ErrStr[];

#endif
