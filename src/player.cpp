/*
 *  player.cpp -- Stream thread
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

  #ifdef HAVE_SYS_TYPES_H
    #include <sys/types.h>
  #endif
	
  #ifdef HAVE_SYS_STAT_H
    #include <sys/stat.h>
  #endif
	
  #ifdef HAVE_FCNTL_H
    #include <fcntl.h>
  #endif
	
  #ifdef HAVE_UNISTD_H
    #include <unistd.h>
  #endif
#endif

#include "globals.h"
#include "player.h"

void *player(void *arg)
{
  char *buf = new char[SHOUT_BUFFER_LENGTH];
  int bytes, file_id;

  while (!time_to_quit)
  {
    sem_obj->Wait(track_load);

    if (time_to_quit)
      ok_to_end = true;
    else
    {
      file_id = open(ready_obj->GetData(),O_RDONLY,0);
      if (file_id != -1)
      {
        while ((bytes = read(file_id,buf,SHOUT_BUFFER_LENGTH)))
          stream_obj->Play((unsigned char *) buf,bytes);
        close(file_id);
      }
      else
        log_obj->WriteLog("WARNING! File doesn't exists:",ready_obj->GetData());
    }
    
    sem_obj->Signal(end_track);
  }

  delete [] buf;

  return NULL;
}
