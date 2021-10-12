/*
 *  data_streamer.cpp -- Metadata handler thread
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

  #ifdef HAVE_STRING_H
    #include <string.h>
  #endif

  #ifdef HAVE_UNISTD_H
    #include <unistd.h>
  #endif
#endif

#include "globals.h"
#include "data_streamer.h"
#include "meta_parser.h"

void *data_streamer(void *arg)
{
  sig_obj->Block();
  
  if ((conf_obj->GetValue(_metaupdate) == NULL) || (atoi(conf_obj->GetValue(_metaupdate)) > 0))
  {
    cMetaDataList *global = NULL, *directory = NULL, *file = NULL, *p = NULL;
    bool loc_changed = false;
    #ifdef HAVE_ID3
      struct ID3TableType *loc_ID3t;
    #endif
    int i;
    char *curr_path = NULL, *new_path = NULL, *temp_buf = NULL, *file_buf = NULL;
    
    curr_path = new char[INTERNAL_BUF_SIZE];
    new_path = new char[INTERNAL_BUF_SIZE];
    temp_buf = new char[INTERNAL_BUF_SIZE];
    file_buf = new char[INTERNAL_BUF_SIZE];
    
    bzero(temp_buf,INTERNAL_BUF_SIZE);
    bzero(curr_path,INTERNAL_BUF_SIZE);
    bzero(new_path,INTERNAL_BUF_SIZE);
    bzero(file_buf,INTERNAL_BUF_SIZE);
    
    #ifdef HAVE_ID3
      loc_ID3t = new ID3TableType[MAX_ID3_TAGS];
      for (i = 0; i < MAX_ID3_TAGS; i++)
      {
        loc_ID3t[i].ID3Type = ID3Table[i].ID3Type;
        loc_ID3t[i].str = new char[INTERNAL_BUF_SIZE];
        bzero(loc_ID3t[i].str,INTERNAL_BUF_SIZE);
      }
    #endif

    if (conf_obj->GetValue(_mdfpath) != NULL)
    {
      global = new cMetaDataList;
      global->LoadMeta(conf_obj->GetValue(_mdfpath));
      global->StartMeta();
    }
  
    directory = new cMetaDataList;
    file = new cMetaDataList;

    sem_obj->Wait(start_meta);
  
    while (!ok_to_end)
    {
      sem_obj->Wait(data_mutex);

      if (loc_changed = changed)
      {
        ready_obj->GetPath(new_path);
        strcpy(temp_buf,ready_obj->GetData());
        ready_obj->GetFile(file_buf);
        changed = false;
      
        #ifdef HAVE_ID3
          ready_obj->GetID3Table(loc_ID3t);
        #endif
      }
      sem_obj->Signal(data_mutex);

      if (loc_changed)
      {
        int len;
      
        file->PurgeMeta();
        file_buf[strlen(file_buf)-4] = '\0';
        
        len = strlen(temp_buf);
        temp_buf[len-1] = 'f';
        temp_buf[len-2] = 'd';
        temp_buf[len-3] = 'm';
        file->LoadMeta(temp_buf);
        file->StartMeta();
        
        if (file->IsEmpty() && strcmp(curr_path,new_path))
        {
          directory->PurgeMeta();
          strcat(new_path,"/default.mdf");
          directory->LoadMeta(new_path);
          directory->StartMeta();
        }
        strcpy(curr_path,new_path);

        if (!file->IsEmpty())
          p = file;
        else if (!directory->IsEmpty())
          p = directory;
        else if ((global != NULL) && !global->IsEmpty())
          p = global;
        else
          p = NULL;
      }

      if (p != NULL)
        #ifdef HAVE_ID3
          p->GetNextMeta(temp_buf,loc_ID3t);
        #else
          p->GetNextMeta(temp_buf);
        #endif
      else
        strcpy(temp_buf,file_buf);

      if (strlen(temp_buf) > 0)
        stream_obj->UpdateMetaData(temp_buf);
      sleep(((conf_obj->GetValue(_metaupdate) != NULL)? atoi(conf_obj->GetValue(_metaupdate)) : 5));
    }

    
    if (file != NULL)
      delete file;
    if (directory != NULL)
      delete directory;
    if (global != NULL)
      delete global;
    
    #ifdef HAVE_ID3
      for (i = 0; i < MAX_ID3_TAGS; i++)
        delete [] loc_ID3t[i].str;
    
      delete [] loc_ID3t;
    #endif

    delete [] curr_path;
    delete [] new_path;
    delete [] temp_buf;
    delete [] file_buf;
  }

  return NULL;
}
