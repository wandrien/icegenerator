/*
 *  streamer.h -- Interface for libshout module (include)
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

#ifndef __streamer_h__
#define __streamer_h__

#include <shout/shout.h>
#include "config.h"

class cStreamer
{
  shout_t *ShoutData;
  shout_metadata_t *MetaData;
  bool Minimal[MAX_KEYS];
  bool Connected;
public:
  cStreamer();
  ~cStreamer();
  void SetData(cConfig *object);
  void Connect();
  void Disconnect();
  void UpdateMetaData(const char *buf);
  void Play(unsigned char *buf, int bytes);
};

extern char ErrStr[];

#endif
