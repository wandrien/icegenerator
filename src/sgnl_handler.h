/*
 *  sgnl_handler.h -- Signal handler module (include)
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

#ifndef __sgnl_handler_h__
#define __sgnl_handler_h__

#ifdef HAVE_CONFIG_H
  #ifndef __main_config_h__
    #define __main_config_h__
    #include "../config.h"
  #endif

  #ifndef __USE_POSIX
    #define __USE_POSIX
  #endif
  
  #ifdef HAVE_SIGNAL_H
    #include <signal.h>
  #endif
#endif

#define MAX_SIGNALS 16


class cSignal
{
  struct
  {
    int Sig;
    struct sigaction SigHandler, OldHandler;
  } SigsData[MAX_SIGNALS];
  int SigPtr;
public:
  cSignal();
  ~cSignal();
  void AddHandler(int sig, void (*handler)(int));
  void Block();
};

extern char ErrStr[];

#endif
