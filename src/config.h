/*
 *  config.h -- Configuration file parser module (include)
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

#ifndef __config_h__
#define __config_h__

#define MAX_KEYS 23

enum key_type { _ip = 0, _port, _mount, _password, _server, _name, _genre,
               _description, _url, _bitrate, _public, _dumpfile, _mp3path,
               _format, _log, _source, _loop, _recurse, _shuffle, _metaupdate,
							 _dataport, _logpath, _mdfpath, _badkey };

#define INTERNAL_BUF_SIZE 512
                       
class cConfig
{
  char *Table[MAX_KEYS];
  char *int_buf;
  int ParseLine(const char *buf, char *key, char *value);
  key_type LookupKey(const char *key) const;
public:
  cConfig(const char *path);
  ~cConfig();
  char *GetValue(key_type key) const;
  void SetValue(key_type key, const char *value);
};

extern char ErrStr[];

#endif
