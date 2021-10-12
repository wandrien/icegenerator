/*
 *  sgnl_handler.cpp -- Signal handler module
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
#endif

#include "sgnl_handler.h"

cSignal::cSignal()
{
  SigPtr = 0;
}

cSignal::~cSignal()
{
  while (SigPtr--)
    if (sigaction(SigsData[SigPtr].Sig,&(SigsData[SigPtr].OldHandler),NULL) == -1)
    {
      strcpy(ErrStr,"Cannot remove signal handlers");
      throw 0;
    }
}

void cSignal::AddHandler(int sig, void (*handler)(int))
{
  SigsData[SigPtr].Sig = sig;
  SigsData[SigPtr].SigHandler.sa_handler = handler;
  if (sigemptyset(&(SigsData[SigPtr].SigHandler.sa_mask)) == -1)
  {
    strcpy(ErrStr,"Error in signal handler installer");
    throw 0;
  }
  SigsData[SigPtr].SigHandler.sa_flags = 0;
  if (sigaction(SigsData[SigPtr].Sig,&(SigsData[SigPtr].SigHandler),&(SigsData[SigPtr].OldHandler)) == -1)
  {
    strcpy(ErrStr,"Cannot insert handler into signal queue");
    throw 0;
  }
  else
    SigPtr++;
}

void cSignal::Block()
{
  int i;
  sigset_t block_sigs;
  const char *err_str = "Error in signal handler block";

  if (sigemptyset(&block_sigs) == -1)
  {
    strcpy(ErrStr,err_str);
    throw 0;
  }
  for (i = 0; i < SigPtr; i++)
    if (sigaddset (&block_sigs, SigsData[i].Sig) == -1)
    {
      strcpy(ErrStr,err_str);
      throw 0;
    }
  if (sigprocmask (SIG_BLOCK, &block_sigs, NULL) == -1)
  {
    strcpy(ErrStr,err_str);
    throw 0;
  }
}
