/*
 *  synch.cpp -- Thread synchronization module
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

#include "synch.h"

cSemaphore::cSemaphore()
{
  SemPtr = 0;
}

int cSemaphore::AddSem(int value)
{
  if (sem_init(&(Sems[SemPtr]),0,value) == -1)
  {
    strcpy(ErrStr,"Cannot init synch section");
    throw 0;
  }
  else
    return SemPtr++;
}

cSemaphore::~cSemaphore()
{
  int i;
  
  for (i = 0; i < SemPtr; i++)
    if (sem_destroy(&(Sems[i])) == -1)
    {
      strcpy(ErrStr,"Cannot destroy synch section");
      throw 0;
    }
}

void cSemaphore::Wait(int sem)
{
  sem_wait(&(Sems[sem]));
}

void cSemaphore::Signal(int sem)
{
  if (sem_post(&(Sems[sem])) == -1)
  {
    strcpy(ErrStr,"Error occurred during synchronization");
    throw 0;
  }
}
