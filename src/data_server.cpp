/*
 *  data_server.cpp -- Telnet interface thread
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

  #ifdef HAVE_CTYPE_H
    #include <ctype.h>
  #endif
	
  #ifdef HAVE_STRING_H
    #include <string.h>
  #endif
	
  #ifndef _BSD_SOCKLEN_T_
    #define _BSD_SOCKLEN_T_
  #endif

  #ifdef HAVE_SYS_TYPES_H
    #include <sys/types.h>
  #endif
	
  #ifdef HAVE_SYS_SOCKET_H
    #include <sys/socket.h>
  #endif
	
  #ifdef HAVE_NETINET_IN_H
    #include <netinet/in.h>
  #endif
	
  #ifdef HAVE_ARPA_INET_H
    #include <arpa/inet.h>
  #endif
	
  #ifdef HAVE_UNISTD_H
    #include <unistd.h>
  #endif

#endif

#include "globals.h"
#include "data_server.h"

void *data_server(void *arg)
{
  int my_sock, ext_sock;
  socklen_t len;
  struct sockaddr_in from, sin;
  char *int_buf;
  
  sig_obj->Block();
  
  int_buf = new char[INTERNAL_BUF_SIZE];
	
  if (conf_obj->GetValue(_dataport) != NULL)
  {
    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_port = htons(atoi(conf_obj->GetValue(_dataport)));
    if (!sin.sin_port)
      sin.sin_port = 8796;
    sin.sin_addr.s_addr = htonl(INADDR_ANY);
    if ((my_sock	= socket(AF_INET, SOCK_STREAM, 0)) == -1)
      log_obj->WriteLog("Socket error. Telnet interface not available");
    else if (bind(my_sock,(struct sockaddr *) &sin, sizeof sin) == -1)
      log_obj->WriteLog("Can't bind to specified port");
    else
    {
      if (listen(my_sock, 1) == -1)
        log_obj->WriteLog("Cannot listen on specified port");
      else
        while (!ok_to_end)
        {
          int buf_len, i;
          bool dot_found = false;

          len = sizeof from;
          ext_sock = accept(my_sock, (struct sockaddr *) &from, &len);
          
          if (!ok_to_end)
          {
            log_obj->WriteLog("Connected to client ",inet_ntoa(from.sin_addr));

            buf_len = read(ext_sock,int_buf,INTERNAL_BUF_SIZE);
            int_buf[buf_len+1] = '\0';
            for (i = 0; i < buf_len; i++)
              if (int_buf[i] == '.')
              {
                int_buf[i] = '\0';
                dot_found = true;
              }
              else
                int_buf[i] = toupper(int_buf[i]);
					
            if ((buf_len > 0) && dot_found)
              if (!strcmp(int_buf,"TRACK"))
              {
                sem_obj->Wait(data_mutex);
                ready_obj->GetFile(int_buf);
                sem_obj->Signal(data_mutex);

                int_buf[strlen(int_buf)-4] = '\0';
                log_obj->WriteLog("Received TRACK command, releasing information...");
              }
              else if (!strcmp(int_buf,"QUIT"))
              {
                signal_termination_proc(0);
                strcpy(int_buf,"OK");
              }
              else if (!strcmp(int_buf,"LOOP"))
              {
                if (conf_obj->GetValue(_loop) != NULL)
                  strcpy(int_buf,conf_obj->GetValue(_loop));
                else
                  strcpy(int_buf,"1");
                log_obj->WriteLog("Received LOOP command, releasing information...");
              }
              else if (!strcmp(int_buf,"LOOPON"))
              {
                conf_obj->SetValue(_loop,"1");
                log_obj->WriteLog("LOOP enabled");
                strcpy(int_buf,"OK");
              }
              else if (!strcmp(int_buf,"LOOPOFF"))
              {
                conf_obj->SetValue(_loop,"0");
                log_obj->WriteLog("LOOP disabled");
                strcpy(int_buf,"OK");
              }
              else if (!strcmp(int_buf,"SHUFFLE"))
              {
                if (conf_obj->GetValue(_shuffle) != NULL)
                  strcpy(int_buf,conf_obj->GetValue(_shuffle));
                else
                  strcpy(int_buf,"1");
                log_obj->WriteLog("Received SHUFFLE command, releasing information...");
              }
              else if (!strcmp(int_buf,"SHUFFLEON"))
              {
                conf_obj->SetValue(_shuffle,"1");
                log_obj->WriteLog("SHUFFLE enabled");
                strcpy(int_buf,"OK");
              }
              else if (!strcmp(int_buf,"SHUFFLEOFF"))
              {
                conf_obj->SetValue(_shuffle,"0");
                log_obj->WriteLog("SHUFFLE disabled");
                strcpy(int_buf,"OK");
              }
              else
                dot_found = false;

            if (!dot_found)
              strcpy(int_buf,"Unknown command");

            write(ext_sock,int_buf,strlen(int_buf)+1);
          }

          close(ext_sock);
        }
    }

    close(my_sock);
  }

  return NULL;
}

void close_data_server()
{
  struct sockaddr_in sin;
  int s = socket(AF_INET, SOCK_STREAM, 0);

  memset(&sin, 0, sizeof(sin));
  sin.sin_family = AF_INET;
  sin.sin_port = htons(atoi(conf_obj->GetValue(_dataport)));
  if (!sin.sin_port)
    sin.sin_port = 8796;
  sin.sin_addr.s_addr = inet_addr("127.0.0.1");

  connect(s, (struct sockaddr *)&sin,sizeof sin);

  close(s);
}
