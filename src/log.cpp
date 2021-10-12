/*
 *  log.cpp -- Log handler
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

  #ifdef HAVE_STDIO_H
    #include <stdio.h>
  #endif
	
  #ifdef HAVE_SYSLOG_H
    #include <syslog.h>
  #endif

  #ifdef HAVE_STRING_H
    #include <string.h>
  #endif

  #ifdef TIME_WITH_SYS_TIME
    #include <sys/time.h>
    #include <time.h>
  #else
    #ifdef HAVE_SYS_TIME_H
      #include <sys/time.h>
    #else
      #include <time.h>
    #endif
  #endif
#endif

#include "log.h"

cLog::cLog()
{
  LogSelected = _none;
}

cLog::cLog(cConfig *object)
{
  int_buf = new char[INTERNAL_BUF_SIZE];

  switch (LogSelected = ((log_type) atoi(object->GetValue(_log))))
  {
    case _none: break;
    case _system: openlog("Icecast Generator",LOG_PID,LOG_USER);
                  break;
    case _file: if (object->GetValue(_logpath) != NULL)
                  strcpy(int_buf,object->GetValue(_logpath));
                else
                  strcpy(int_buf,"/var/log/icegenerator.log");
                if ((LogFp = fopen(int_buf,"a")) == NULL)
                {
                  printf("Warning: cannot create /var/log/icegenerator.log\nNo log will happen\n");
                  LogSelected = _none;
                }
                break;
  }
}


void cLog::WriteLog(const char *msg, const char *data)
{
  time_t now;
  
  switch (LogSelected)
  {
    case _none: break;
    case _system: if (data == NULL)
                    syslog(LOG_INFO,"%s\n",msg);
                  else
                    syslog(LOG_INFO,"%s %s\n",msg,data); 
                  break;
    case _file: now = time(NULL);
                strcpy(int_buf,asctime(localtime(&now)));
                int_buf[strlen(int_buf)-1] = '\0';
                if (data == NULL)
                  fprintf(LogFp,"%s: %s\n",int_buf,msg);
                else
                  fprintf(LogFp,"%s: %s %s\n",int_buf,msg,data);
                fflush(LogFp);
                break;
  }
}

cLog::~cLog()
{
  switch (LogSelected)
  {
    case _none: break;
    case _system: closelog();
                  break;
    case _file: fprintf(LogFp,"\n\n");
                fclose(LogFp);
                break;
  }

  delete [] int_buf;
}
