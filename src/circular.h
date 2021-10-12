/*
 *  circular.h -- Circular lists handler module (include)
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

#ifndef __circular_h__
#define __circular_h__

#ifdef HAVE_CONFIG_H
  #ifndef __main_config_h__
    #define __main_config_h__
    #include "../config.h"
  #endif

  #ifdef HAVE_ID3
    #include <id3/tag.h>
    
    struct ID3TableType
    {
      ID3_FrameID ID3Type;
      char *str;
    };
  #endif
#endif

#include "config.h"

#define MAX_ID3_TAGS 16
#define INTERNAL_BUF_SIZE 512
  
enum file_type { _mp3, _ogg, _unknown };
enum pls_type { _dir, _sql, _pls, _bad_pls };

class cDoubleLinkedItem
{
  cDoubleLinkedItem *Prev;
  char *Data;
  unsigned char offset;
  cDoubleLinkedItem *Next;
  #ifdef HAVE_ID3
    struct ID3TableType ID3[MAX_ID3_TAGS];
  #endif
public:
  cDoubleLinkedItem(const char *buf, unsigned char ofs = '\0');
  ~cDoubleLinkedItem();
  void ConnectBefore(cDoubleLinkedItem *object);
  void ConnectAfter(cDoubleLinkedItem *object);
  cDoubleLinkedItem *GetPrevious() const;
  cDoubleLinkedItem *GetNext() const;
  void GetPath(char *buf);
  void GetFile(char *buf);
  char *GetData() const;
  void SetData(const char *buf, unsigned char ofs = '\0');
  #ifdef HAVE_ID3
    void LoadID3();
    char *GetID3String(int i);
    void GetID3Table(struct ID3TableType *ID3t);
  #endif
};

class cCircularList
{
  char *int_buf;
  cDoubleLinkedItem *Ptr;
  bool IsMedia(const char *buf, file_type ft) const;
  bool HereStarts() const;
  bool HereFits(const char *buf) const;
  bool GreaterThan(const char *s1, const char *s2) const;
  bool FillWithPath(const char *buf, file_type ft, int recursive);
  #ifdef HAVE_MYSQL
  bool FillWithMySQL(const char *host, unsigned int port, const char *user, const char *passwd,
                     const char *db, const char *table, const char *field);
  #endif
  #ifdef HAVE_PGSQL
  bool FillWithPgSQL(const char *host, unsigned int port, const char *user, const char *passwd,
                     const char *db, const char *table, const char *field);
  #endif
  bool FillWithM3U(const char *m3u_filename);
  bool FillWithPLS(const char *pls_filename);
  pls_type Pls_Selected;
public:
  cCircularList();
  ~cCircularList();
  // void Insert(const char *buf, const char *buf_more);
  void Insert(const char *buf);
  void Insert(cDoubleLinkedItem *object);
  cDoubleLinkedItem *Remove();
  void StepFwd(int steps = 1);
  void StepRev(int steps = 1);
  void GetPath(char *buf);
  void GetFile(char *buf);
  char *GetData() const;
  bool Empty() const;
  void SetStart();
  void ParsePlayList(cConfig *object);
  #ifdef HAVE_ID3
    void GetID3Table(struct ID3TableType *ID3t);
  #endif
};

extern char ErrStr[];

#endif
