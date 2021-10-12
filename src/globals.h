/*
 *  globals.h -- Global data define
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

#ifndef __globals_h__
#define __globals_h__

#include "streamer.h"
#include "log.h"
#include "synch.h"
#include "circular.h"
#include "config.h"
#include "sgnl_handler.h"

extern cLog *log_obj;
extern cSignal *sig_obj;
extern cConfig *conf_obj;
extern cStreamer *stream_obj;
extern cCircularList *ready_obj, *played_obj;
extern cSemaphore *sem_obj;
extern int track_load, end_track, data_mutex, start_meta;
extern volatile bool time_to_quit;
extern bool ok_to_end, changed;

#endif
