/*
 *  streamer.cpp -- Interface for libshout module
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
	
  #ifdef HAVE_STDIO_H
    #include <stdio.h>
  #endif
#endif

#include "streamer.h"
#include "config.h"

cStreamer::cStreamer()
{
  shout_init();

  if ((ShoutData = shout_new()) == NULL)
  {
    strcpy(ErrStr,"Not enough memory to init Icecast connection");
    throw 0;
  }
  if ((MetaData = shout_metadata_new()) == NULL)
  {
    strcpy(ErrStr,"Not enough memory to init Icecast connection");
    throw 0;
  }
  Minimal[_ip] = false;			// FALSE = REQUIRED
  Minimal[_port] = false;
  Minimal[_mount] = false;
  Minimal[_password] = false;
  Minimal[_server] = false;
  Minimal[_name] = true;
  Minimal[_genre] = true;
  Minimal[_description] = true;
  Minimal[_url] = true;
  Minimal[_bitrate] = true;
  Minimal[_public] = true;
  Minimal[_dumpfile] = true;
  Minimal[_mp3path] = false;
  Minimal[_format] = false;
  Minimal[_log] = true;
  Minimal[_source] = false;
  Minimal[_loop] = true;
  Minimal[_recurse] = true;
  Minimal[_shuffle] = true;
  Minimal[_metaupdate] = true;
  Minimal[_dataport] = true;
  Minimal[_logpath] = true;
  Minimal[_mdfpath] = true;
  
  Connected = false;
}

cStreamer::~cStreamer()
{
  if (Connected)
    Disconnect();
  shout_metadata_free(MetaData);
  shout_free(ShoutData);
  shout_shutdown();
}

void cStreamer::SetData(cConfig *object)
{
  int i;
  
  for (i = ((key_type) _ip); i < ((key_type) _badkey); i++)
  {
    if (object->GetValue((key_type) i) != NULL)
    {
      Minimal[i] = true;
      switch ((key_type) i)
      {
        case _ip: shout_set_host(ShoutData,object->GetValue(_ip));
                  break;
        case _port: shout_set_port(ShoutData,atoi(object->GetValue(_port)));
                    break;
        case _mount: shout_set_mount(ShoutData,object->GetValue(_mount));
                     break;
        case _password: shout_set_password(ShoutData,object->GetValue(_password));
                        break;
        case _server: switch (atoi(object->GetValue(_server)))
                      {
                        case 2: shout_set_protocol(ShoutData,SHOUT_PROTOCOL_HTTP);
                                break;
                        case 1: shout_set_protocol(ShoutData,SHOUT_PROTOCOL_ICY);
                                break;
                        default: strcpy(ErrStr,"Bad server protocol");
                                 throw 0;
                                 break;
                      }
                      break;
        case _name: shout_set_name(ShoutData,object->GetValue(_name));
                    break;
        case _genre: shout_set_genre(ShoutData,object->GetValue(_genre));
                     break;
        case _description: shout_set_description(ShoutData,object->GetValue(_description));
                           break;
        case _url: shout_set_url(ShoutData,object->GetValue(_url));
                   break;
        case _bitrate: shout_set_audio_info(ShoutData,SHOUT_AI_BITRATE,object->GetValue(_bitrate));
                       break;
        case _public: shout_set_public(ShoutData,atoi(object->GetValue(_public)));
                      break;
        case _dumpfile: shout_set_dumpfile(ShoutData,object->GetValue(_dumpfile));
                        break;
        case _format: switch (atoi(object->GetValue(_format)))
                      {
                        case 0: shout_set_format(ShoutData,SHOUT_FORMAT_VORBIS);
                                break;
                        case 1: shout_set_format(ShoutData,SHOUT_FORMAT_MP3);
                                break;
		                    default: strcpy(ErrStr,"Bad file format");
                                 throw 0;
                                 break;
                      }
                      break;
        case _source: shout_set_user(ShoutData,object->GetValue(_source));
                      break;
        case _mp3path:
        case _log:
        case _badkey:
        case _loop: 
        case _recurse: 
        case _shuffle: 
        case _metaupdate: 
        case _dataport: 
        case _logpath: 
        case _mdfpath: break;
      }
    }

    if (shout_get_errno(ShoutData) != SHOUTERR_SUCCESS)
    {
      strcpy(ErrStr,shout_get_error(ShoutData));
      throw 0;
    }
  }

  for (i = ((int) _ip); (i < ((int) _badkey)) && Minimal[i]; i++);

  if (i < ((int) _badkey))
  {
    strcpy(ErrStr,"Insufficient parameters to run Icegenerator");
    throw 0;
  }
}

void cStreamer::Connect()
{
  if (shout_open(ShoutData) != SHOUTERR_SUCCESS)
  {
    strcpy(ErrStr,"Cannot connect to stream server.\n");
    throw 0;
  }
  else
    Connected = true;
}

void cStreamer::Disconnect()
{
  shout_close(ShoutData);
  if (shout_get_errno(ShoutData) != SHOUTERR_SUCCESS)
  {
    strcpy(ErrStr,shout_get_error(ShoutData));
    throw 0;
  }
  Connected = false;
}

void cStreamer::UpdateMetaData(const char *buf)
{
  shout_metadata_add(MetaData,"song",buf);
  if (shout_get_errno(ShoutData) != SHOUTERR_SUCCESS)
  {
    strcpy(ErrStr,shout_get_error(ShoutData));
    throw 0;
  }
  shout_set_metadata(ShoutData,MetaData);
  if (shout_get_errno(ShoutData) != SHOUTERR_SUCCESS)
  {
    strcpy(ErrStr,shout_get_error(ShoutData));
    throw 0;
  }
}

void cStreamer::Play(unsigned char *buf, int bytes)
{
  shout_send(ShoutData,buf,bytes);
  if (shout_get_errno(ShoutData) != SHOUTERR_SUCCESS)
  {
    strcpy(ErrStr,shout_get_error(ShoutData));
    throw 0;
  }
  shout_sync(ShoutData);
}
