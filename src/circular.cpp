/*
 *  circular.cpp -- Circular lists handler module
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
	
  #ifdef HAVE_SYS_TYPES_H
    #include <sys/types.h>
  #endif
	
  #ifdef HAVE_SYS_STAT_H
    #include <sys/stat.h>
  #endif
	
  #ifdef HAVE_UNISTD_H
    #include <unistd.h>
  #endif
	
  #ifdef HAVE_DIRENT_H
    #include <dirent.h>
  #endif
	
  #ifdef HAVE_CTYPE_H
    #include <ctype.h>
  #endif

  #ifdef HAVE_NDIR_H
    #include <ndir.h>
  #endif

  #ifdef HAVE_SYS_DIR_H
    #include <sys/dir.h>
  #endif

  #ifdef HAVE_SYS_NDIR_H
    #include <sys/ndir.h>
  #endif
	
  #ifdef HAVE_MYSQL
    #define NO_CLIENT_LONG_LONG
    #include <mysql.h>
  #endif
  
  #ifdef HAVE_PGSQL
    #include <libpq-fe.h>
  #endif
#endif

#include <stdio.h>

#include "circular.h"

/* ****************************************************************************** */
/* Double-linked circular list handler                                            */
/* ****************************************************************************** */

#ifdef HAVE_ID3
struct ID3TableType ID3Table[MAX_ID3_TAGS] = { { ID3FID_COMMENT, "ID3COMMENT" },
                                               { ID3FID_ALBUM, "ID3ALBUM" },
                                               { ID3FID_COMPOSER, "ID3COMPOSER" },
                                               { ID3FID_COPYRIGHT, "ID3COPYRIGHT" },
                                               { ID3FID_ENCODEDBY, "ID3ENCODEDBY" },
                                               { ID3FID_LYRICIST, "ID3LYRICIST" },
                                               { ID3FID_TITLE, "ID3TITLE" },
                                               { ID3FID_SUBTITLE, "ID3SUBTITLE" },
                                               { ID3FID_LEADARTIST, "ID3LEADARTIST" },
                                               { ID3FID_BAND, "ID3BAND" },
                                               { ID3FID_CONDUCTOR, "ID3CONDUCTOR" },
                                               { ID3FID_PUBLISHER, "ID3PUBLISHER" },
                                               { ID3FID_NETRADIOSTATION, "ID3NETRADIOSTATION" },
                                               { ID3FID_NETRADIOOWNER, "ID3NETRADIOOWNER" },
                                               { ID3FID_USERTEXT, "ID3USERTEXT" },
                                               { ID3FID_TERMSOFUSE, "ID3TERMSOFUSE" } };
#endif
                                             
cDoubleLinkedItem::cDoubleLinkedItem(const char *buf, unsigned char ofs)
{
  Data = NULL;
  Prev = Next = NULL;
  
  #ifdef HAVE_ID3
    int i;

    for (i = 0; i < MAX_ID3_TAGS; i++)
    {
      ID3[i].ID3Type = ID3Table[i].ID3Type;
      ID3[i].str = NULL;
    }
  #endif

  Data = new char[strlen(buf)+1];
  strcpy(Data,buf);
  offset = ofs;
}

cDoubleLinkedItem::~cDoubleLinkedItem()
{
  if (Data != NULL)
    delete [] Data;
  
  #ifdef HAVE_ID3
    int i;

    for (i = 0; i < MAX_ID3_TAGS; i++)
      if (ID3[i].str != NULL)
      {
        delete [] ID3[i].str;
        ID3[i].str = NULL;
      }
  #endif
}

void cDoubleLinkedItem::ConnectBefore(cDoubleLinkedItem *object)
{
  Prev = object;
}

void cDoubleLinkedItem::ConnectAfter(cDoubleLinkedItem *object)
{
  Next = object;
}

cDoubleLinkedItem * cDoubleLinkedItem::GetPrevious() const
{
  return Prev;
}

cDoubleLinkedItem * cDoubleLinkedItem::GetNext() const
{
  return Next;
}

void cDoubleLinkedItem::GetPath(char *buf)
{
  if (offset != '\0')
    strncpy(buf,Data,offset);
  buf[offset] = '\0';
}

void cDoubleLinkedItem::GetFile(char *buf)
{
  strncpy(buf,(Data+offset+1),strlen(Data)-offset);
}

char *cDoubleLinkedItem::GetData() const
{
  return Data;
}

void cDoubleLinkedItem::SetData(const char *buf, unsigned char ofs)
{
  if (Data != NULL)
    delete [] Data;
  
  Data = new char[strlen(buf)+1];
  strcpy(Data,buf);
  offset = ofs;
}

#ifdef HAVE_ID3
void cDoubleLinkedItem::LoadID3()
{
  ID3_Tag *my_tag;

  if ((Data != NULL) && (my_tag = new ID3_Tag))
  {
    ID3_Frame *my_frame;
    ID3_Field *my_field;
    int i;
    char *temp;
  
    my_tag->Link(Data);
    temp = new char[255];
    
    for (i = 0; i < MAX_ID3_TAGS; i++)
    {
      if ((my_frame = my_tag->Find(ID3[i].ID3Type)) != NULL)
      {
        if ((my_field = my_frame->GetField(ID3FN_TEXT)) != NULL)
        {
          bzero(temp,255);
          my_field->Get(temp,255);
        
          if (strlen(temp) > 0)
          {
            ID3[i].str = new char[strlen(temp)+1];
            strcpy(ID3[i].str,temp);
          }
        
          my_field->Clear();
        }
        my_frame->Clear();
      }
    }
    my_tag->Clear();
  
    delete my_tag;
  
    delete [] temp;
  }
}

char *cDoubleLinkedItem::GetID3String(int i)
{
  return ((i < MAX_ID3_TAGS)? ID3Table[i].str : NULL);
}

void cDoubleLinkedItem::GetID3Table(struct ID3TableType *ID3t)
{
  int i;
  
  for (i = 0; i < MAX_ID3_TAGS; i++)
  {
    if (ID3[i].str != NULL)
      strcpy(ID3t[i].str,ID3[i].str);
    else
      ID3t[i].str[0] = '\0';
  }
}
#endif

cCircularList::cCircularList()
{
  Ptr = NULL;
  int_buf = new char[INTERNAL_BUF_SIZE];
}

cCircularList::~cCircularList()
{
  if (Ptr != NULL)
  {
    Ptr->GetPrevious()->ConnectAfter(NULL);     // break chain
    while (Ptr != NULL)
    {
      cDoubleLinkedItem *temp = Ptr;

      StepFwd();
      delete temp;
    }
  }

  delete [] int_buf;
}

void cCircularList::Insert(cDoubleLinkedItem *object)
{
  if (Ptr == NULL)
  {
    Ptr = object;				// first element
    Ptr->ConnectAfter(Ptr);			// chain is closed in a loop
    Ptr->ConnectBefore(Ptr);
  }
  else
  {
    /* NOTE: it connects after Ptr */
    object->ConnectBefore(Ptr);			// link to upper object
    Ptr->GetNext()->ConnectBefore(object);	// lower object link
    object->ConnectAfter(Ptr->GetNext());	// link to lower object
    Ptr->ConnectAfter(object);			// upper object link
  }
}

void cCircularList::Insert(const char *buf)
{
  int i;

  cDoubleLinkedItem *item;

  for (i = strlen(buf)-1; ((i >= 0) && (buf[i] != '/')); i--);
  if (i == -1)
    i = 0;
    
  item = new cDoubleLinkedItem(buf,((unsigned char) i));
  Insert(item);
  #ifdef HAVE_ID3
    item->LoadID3();
  #endif
}

cDoubleLinkedItem * cCircularList::Remove()
{
  cDoubleLinkedItem *temp = Ptr;

  if (Ptr != NULL)
    if (Ptr->GetPrevious() == Ptr)
      Ptr = NULL;
    else
    {
      Ptr->GetPrevious()->ConnectAfter(Ptr->GetNext());		// bypass upper link
      Ptr->GetNext()->ConnectBefore(Ptr->GetPrevious());	// bypass lower link
      Ptr = Ptr->GetPrevious();					// point elsewhere
      temp->ConnectBefore(NULL);				// detach object from chain
      temp->ConnectAfter(NULL);
    }

  return temp;
}

void cCircularList::StepFwd(int steps)
{
  while (steps-- && (Ptr != NULL))
    Ptr = Ptr->GetNext();
}

void cCircularList::StepRev(int steps)
{
  while (steps-- && (Ptr != NULL))
    Ptr = Ptr->GetPrevious();
}

void cCircularList::GetPath(char *buf)
{
  Ptr->GetPath(buf);
}

void cCircularList::GetFile(char *buf)
{
  Ptr->GetFile(buf);
}

char * cCircularList::GetData() const
{
  return Ptr->GetData();
}

bool cCircularList::Empty() const
{
  return (Ptr == NULL);
}

bool cCircularList::IsMedia(const char *buf, file_type ft) const
{
  int len = strlen(buf);
  bool result = false;

  if (len > 4)
  {
    switch(ft)
    {
      case _mp3: result = ((buf[len-1] == '3') && (toupper(buf[len-2]) == 'P') && (toupper(buf[len-3]) == 'M'));
                 break;
      case _ogg: result = ((toupper(buf[len-1]) == 'G') && (toupper(buf[len-2]) == 'G') && (toupper(buf[len-3]) == 'O'));
                 break;
      case _unknown: break;
      default: break;
    }
	  result &= (buf[len-4] == '.');
  }
  
  return result;
}

bool cCircularList::FillWithPath(const char *buf, file_type ft, int recursive)
{
  bool result = false;
  char temp[255];
  struct dirent *ptr;
  DIR *fd;

  fd = opendir(buf);
  if (fd != NULL)
  {
    while ((ptr = readdir(fd)) != NULL)
      if (strcmp(ptr->d_name,".") && strcmp(ptr->d_name,".."))
      {
        struct stat s;

        strcpy(temp,buf);
        if (temp[strlen(temp)-1] != '/')
          strcat(temp,"/");
        strcat(temp,ptr->d_name);
        if (!stat(temp,&s))
        {
          if (recursive && S_ISDIR(s.st_mode))
            result |= FillWithPath(temp,ft,recursive);

          if (IsMedia(temp,ft))
          {
            result = true;
            if (Ptr != NULL)
              while (!HereFits(temp))
                StepFwd();
            Insert(temp);
          }
        }
      }
    closedir(fd);
  }

  return result;
}

bool cCircularList::GreaterThan(const char *s1, const char *s2) const
{
  while ((*s1) && (*s2) && (toupper(*s1) == toupper(*s2)))
    s1++, s2++;

  return (toupper(*s1) >= toupper(*s2));
}

bool cCircularList::HereStarts() const
{
  return ((Ptr == NULL) || GreaterThan(Ptr->GetData(),Ptr->GetNext()->GetData()));
}

bool cCircularList::HereFits(const char *buf) const
{
  return ((Ptr != NULL) && (((!GreaterThan(Ptr->GetData(),buf)) && (GreaterThan(Ptr->GetNext()->GetData(),buf))) ||
          ((!GreaterThan(Ptr->GetData(),buf)) && (!GreaterThan(Ptr->GetNext()->GetData(),buf)) && HereStarts()) ||
          ((GreaterThan(Ptr->GetData(),buf)) && (GreaterThan(Ptr->GetNext()->GetData(),buf)) && HereStarts())));
}

void cCircularList::SetStart()
{
  while (!HereStarts())
    StepFwd();
}


#ifdef HAVE_MYSQL
bool cCircularList::FillWithMySQL(const char *host, unsigned int port, const char *user, const char *passwd,
                                  const char *db, const char *table, const char *field)
{
  MYSQL sql_obj;
  MYSQL_RES *sql_query_rows;
  MYSQL_ROW row;
  bool result = false;
  struct stat s;

  if (mysql_init(&sql_obj) == NULL)
  {
    strcpy(ErrStr,"MySQL initialization error");
    throw 0;
  }		
  if (mysql_real_connect(&sql_obj,host,user,passwd,db,port,NULL,0) == NULL)
  {
    sprintf(ErrStr,"MySQL: %s",mysql_error(&sql_obj));
    throw 0;
  }
  
  sprintf(int_buf,"SELECT %s FROM %s",field,table);
  if (mysql_query(&sql_obj,int_buf))
  {
    sprintf(ErrStr,"MySQL: %s",mysql_error(&sql_obj));
    throw 0;
  }
  if ((sql_query_rows = mysql_store_result(&sql_obj)) == NULL)
  {
    sprintf(ErrStr,"MySQL: %s",mysql_error(&sql_obj));
    throw 0;
  }

  result = (mysql_num_rows(sql_query_rows) != 0);

  while ((row = mysql_fetch_row(sql_query_rows)) != NULL)
    if (!stat(((const char *) row[0]),&s))
    {
      result = true;
      Insert((const char *) row[0]);
    }
	
  mysql_free_result(sql_query_rows);
  mysql_close(&sql_obj);
  
  return result;
}
#endif

#ifdef HAVE_PGSQL
bool cCircularList::FillWithPgSQL(const char *host, unsigned int port, const char *user, const char *passwd,
                                  const char *db, const char *table, const char *field)
{
  PGconn *sql_obj;
  PGresult *sql_query_rows;
  int row_idx, sql_query_num_rows;
  bool result = false;
  struct stat s;

  if (port)
    sprintf(int_buf,"host='%s' port='%d' user='%s' password='%s' dbname='%s'",host,port,user,passwd,db);
  else
    sprintf(int_buf,"host='%s' user='%s' password='%s' dbname='%s'",host,user,passwd,db);
  
  sql_obj = PQconnectdb(int_buf);
  if (PQstatus(sql_obj) == CONNECTION_OK)
  {
    sprintf(int_buf,"SELECT %s FROM %s",field,table);
    sql_query_rows = PQexec(sql_obj,int_buf);
    sql_query_num_rows = PQntuples(sql_query_rows);
    for (row_idx = 0; row_idx < sql_query_num_rows; row_idx++)
    {
      strcpy(int_buf,PQgetvalue(sql_query_rows,row_idx,0));
      if (!stat(int_buf,&s))
      {
        result = true;
        Insert(int_buf);
      }
    }
    PQclear(sql_query_rows);
  }

  PQfinish(sql_obj);

  return result;
}
#endif

bool cCircularList::FillWithM3U(const char *m3u_filename)
{
  bool result = false;
  FILE *m3u_file;
  struct stat s;

  m3u_file = fopen(m3u_filename,"r");
  while (fgets(int_buf,127,m3u_file))
  {
    int i;

    if (int_buf[strlen(int_buf)-1] == '\n')
      int_buf[strlen(int_buf)-1] = '\0';
	
    for (i = 0; ((int_buf[i] != '\0') && (int_buf[i] == ' ')); i++);

    if (!strncmp(&(int_buf[i]),"file://",7))
      i += 7;
	
    if ((int_buf[i] != '#') && (int_buf[i] != '\0') && (!stat(&(int_buf[i]),&s)))
    {
      result = true;
      Insert(&(int_buf[i]));
    }
	
    int_buf[0] = '\0';
  }
  fclose(m3u_file);

  return result;
}

bool cCircularList::FillWithPLS(const char *pls_filename)
{
  bool result = false;
  FILE *pls_file;
  struct stat s;
	
  pls_file = fopen(pls_filename,"r");
  while (fgets(int_buf,127,pls_file))
  {
    int i;

    if (int_buf[strlen(int_buf)-1] == '\n')
      int_buf[strlen(int_buf)-1] = '\0';
	
    for (i = 0; ((int_buf[i] != '\0') && (int_buf[i] == ' ')); i++);
		
    if (!strncmp(&(int_buf[i]),"File",4))
      for (; ((int_buf[i] != '\0') && (int_buf[i] != '=')); i++);
    
    if (int_buf[i] != '\0')
      i++;
	
    if (!strncmp(&(int_buf[i]),"file://",7))
      i += 7;
	
    if ((int_buf[i] != '\0') && (!stat(&(int_buf[i]),&s)))
    {
      result = true;
      Insert(&(int_buf[i]));
    }
	
    int_buf[0] = '\0';
  }
  fclose(pls_file);

  return result;
}

void cCircularList::ParsePlayList(cConfig *object)
{
  char *parsed_strs[20], *str_ptr;
  struct stat temp_stat;
  int i = 0, strs_ind = 1;
	
  parsed_strs[0] = new char[4];
  strcpy(parsed_strs[0],strtok(object->GetValue(CONFIG_MP3PATH),":"));
  while ((parsed_strs[0][i] = toupper(parsed_strs[0][i])))
    i++;
	
  while ((str_ptr = strtok(NULL,";")) != NULL)
  {
    parsed_strs[strs_ind] = new char[strlen(str_ptr)+1];
    strcpy(parsed_strs[strs_ind++],str_ptr);
  }
	
  if (!strcmp(parsed_strs[0],"PTH"))
  {
    for (i = 1; i < strs_ind; i++)
      if (stat(parsed_strs[i],&temp_stat) != -1)
        FillWithPath(parsed_strs[i],((atoi(object->GetValue(CONFIG_FORMAT)))? _mp3 : _ogg),((object->GetValue(CONFIG_RECURSIVE) != NULL) && atoi(object->GetValue(CONFIG_RECURSIVE))));
  }
  else if (!strcmp(parsed_strs[0],"SQL"))
  {
    #ifdef HAVE_MYSQL
    if (strs_ind == 8)
      FillWithMySQL(parsed_strs[1],atoi(parsed_strs[2]),parsed_strs[3],parsed_strs[4],
                    parsed_strs[5],parsed_strs[6],parsed_strs[7]);
    #endif
  }
  else if (!strcmp(parsed_strs[0],"PQL"))
  {
    #ifdef HAVE_PGSQL
    if (strs_ind == 8)
      FillWithPgSQL(parsed_strs[1],atoi(parsed_strs[2]),parsed_strs[3],parsed_strs[4],
                    parsed_strs[5],parsed_strs[6],parsed_strs[7]);
    #endif
  }
  else if (!strcmp(parsed_strs[0],"M3U"))
    FillWithM3U(parsed_strs[1]);
  else if (!strcmp(parsed_strs[0],"PLS"))
    FillWithPLS(parsed_strs[1]);
  
  for (i = 0; i < strs_ind; i++)
    delete [] parsed_strs[i];
}

#ifdef HAVE_ID3
void cCircularList::GetID3Table(struct ID3TableType *ID3t)
{
  Ptr->GetID3Table(ID3t);
}
#endif

/* ****************************************************************************** */
