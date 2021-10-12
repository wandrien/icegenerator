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
  Minimal[CONFIG_IP] = false;			// FALSE = REQUIRED
  Minimal[CONFIG_PORT] = false;
  Minimal[CONFIG_MOUNT] = false;
  Minimal[CONFIG_PASSWORD] = false;
  Minimal[CONFIG_SERVER] = false;
  Minimal[CONFIG_NAME] = true;
  Minimal[CONFIG_GENRE] = true;
  Minimal[CONFIG_DESCRIPTION] = true;
  Minimal[CONFIG_URL] = true;
  Minimal[CONFIG_BITRATE] = true;
  Minimal[CONFIG_PUBLIC] = true;
  Minimal[CONFIG_DUMPFILE] = true;
  Minimal[CONFIG_MP3PATH] = false;
  Minimal[CONFIG_FORMAT] = false;
  Minimal[CONFIG_LOG] = true;
  Minimal[CONFIG_SOURCE] = false;
  Minimal[CONFIG_LOOP] = true;
  Minimal[CONFIG_RECURSIVE] = true;
  Minimal[CONFIG_SHUFFLE] = true;
  Minimal[CONFIG_METAUPDATE] = true;
  Minimal[CONFIG_DATAPORT] = true;
  Minimal[CONFIG_LOGPATH] = true;
  Minimal[CONFIG_MDFPATH] = true;
  
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
  
  for (i = ((config_key_type) CONFIG_IP); i < ((config_key_type) CONFIG_BADKEY); i++)
  {
    if (object->GetValue((config_key_type) i) != NULL)
    {
      Minimal[i] = true;
      switch ((config_key_type) i)
      {
        case CONFIG_IP:
          shout_set_host(ShoutData,object->GetValue(CONFIG_IP));
          break;
        case CONFIG_PORT:
          shout_set_port(ShoutData,atoi(object->GetValue(CONFIG_PORT)));
          break;
        case CONFIG_MOUNT:
          shout_set_mount(ShoutData,object->GetValue(CONFIG_MOUNT));
          break;
        case CONFIG_PASSWORD:
          shout_set_password(ShoutData,object->GetValue(CONFIG_PASSWORD));
          break;
        case CONFIG_SERVER:
          switch (atoi(object->GetValue(CONFIG_SERVER)))
          {
            case 2:
              shout_set_protocol(ShoutData,SHOUT_PROTOCOL_HTTP);
              break;
            case 1:
              shout_set_protocol(ShoutData,SHOUT_PROTOCOL_ICY);
              break;
            default:
              strcpy(ErrStr,"Bad server protocol");
              throw 0;
              break;
            }
            break;
        case CONFIG_NAME:
          shout_set_name(ShoutData,object->GetValue(CONFIG_NAME));
          break;
        case CONFIG_GENRE:
          shout_set_genre(ShoutData,object->GetValue(CONFIG_GENRE));
          break;
        case CONFIG_DESCRIPTION:
          shout_set_description(ShoutData,object->GetValue(CONFIG_DESCRIPTION));
          break;
        case CONFIG_URL:
          shout_set_url(ShoutData,object->GetValue(CONFIG_URL));
          break;
        case CONFIG_BITRATE:
          shout_set_audio_info(ShoutData,SHOUT_AI_BITRATE,object->GetValue(CONFIG_BITRATE));
          break;
        case CONFIG_PUBLIC:
          shout_set_public(ShoutData,atoi(object->GetValue(CONFIG_PUBLIC)));
          break;
        case CONFIG_DUMPFILE:
          shout_set_dumpfile(ShoutData,object->GetValue(CONFIG_DUMPFILE));
          break;
        case CONFIG_FORMAT:
          switch (atoi(object->GetValue(CONFIG_FORMAT)))
          {
            case 0:
              shout_set_format(ShoutData,SHOUT_FORMAT_VORBIS);
              break;
            case 1:
              shout_set_format(ShoutData,SHOUT_FORMAT_MP3);
              break;
            default:
              strcpy(ErrStr,"Bad file format");
              throw 0;
              break;
          }
          break;
        case CONFIG_SOURCE:
          shout_set_user(ShoutData,object->GetValue(CONFIG_SOURCE));
          break;
        case CONFIG_MP3PATH:
        case CONFIG_LOG:
        case CONFIG_BADKEY:
        case CONFIG_LOOP:
        case CONFIG_RECURSIVE:
        case CONFIG_SHUFFLE:
        case CONFIG_METAUPDATE:
        case CONFIG_DATAPORT:
        case CONFIG_LOGPATH:
        case CONFIG_MDFPATH: break;
      }
    }

    if (shout_get_errno(ShoutData) != SHOUTERR_SUCCESS)
    {
      strcpy(ErrStr,shout_get_error(ShoutData));
      throw 0;
    }
  }

  for (i = ((int) CONFIG_IP); (i < ((int) CONFIG_BADKEY)) && Minimal[i]; i++);

  if (i < ((int) CONFIG_BADKEY))
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
