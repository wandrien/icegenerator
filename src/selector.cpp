/*
 *  selector.cpp -- Track selector thread
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
#endif

#include "globals.h"
#include "selector.h"

void *selector(void *arg)
{
  int i;
  bool first_time = true;
  char *int_buf;
  
  sig_obj->Block();

  srand((int) time(NULL));

  int_buf = new char[INTERNAL_BUF_SIZE];

  while (!ok_to_end)
  {
    i = (((conf_obj->GetValue(_shuffle) != NULL) && (!atoi(conf_obj->GetValue(_shuffle))))? 1 : (rand() % 256));

    sem_obj->Wait(data_mutex);

    ready_obj->StepFwd(i);
    changed = true;
    if (first_time)
    {
      sem_obj->Signal(start_meta);
      first_time = false;
    }
    sem_obj->Signal(data_mutex);
  
    ready_obj->GetFile(int_buf);
    log_obj->WriteLog("Now playing",int_buf);

    sem_obj->Signal(track_load);

    sem_obj->Wait(end_track);

    played_obj->Insert(ready_obj->Remove());

    if (time_to_quit)
    {
      ok_to_end = true;
      sem_obj->Signal(track_load);
    }
    else
      if (ready_obj->Empty())
        if ((conf_obj->GetValue(_loop) == NULL) || atoi(conf_obj->GetValue(_loop)))
        {
          cCircularList *temp;

          temp = ready_obj;
          ready_obj = played_obj;
          played_obj = temp;

          log_obj->WriteLog("Rotating queues and looping again...");
        }
        else
        {
          time_to_quit = true;
          sem_obj->Signal(track_load);
        }
  }

  delete [] int_buf;

  return NULL;
}
