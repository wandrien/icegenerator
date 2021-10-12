/*
 *  main.cpp -- Simple daemon to source an icecast server with direct
 *              mp3/ogg streaming, no resampling.
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
 
/* Version history:
   0.1 - First release
   0.3 - Libshout 2.0 support
         Strong configuration parses
         Support for Icecast2.0/Shoutcast
         Bug fixes
   0.5 - Libshout 1.3.x no longer supported
         Full POSIX implementation
         Multiple playlist directories
         Support for OGG Vorbis
         Command line interface
         Multiple log configuration
         Threads (no more detached processes)
         All "external errors" are catched
         Clean exit, all system resources are released
         FreeBSD/NetBSD/Linux 2.x/MacOS platform supported
 0.5.1 - Code cleaner
         Added SOURCE option
         Added LOOP option
         Added SHUFFLE option
         Added METAUPDATE option
         FIXED: Serious bug in SERVER option
         FIXED: try/catch mechanism didn't work properly
         FIXED: possible error in parsing configuration
         FIXED: wrong path bug
         FIXED: opendir possibly crash (thanks to DIeMONd)
         FIXED: multiple directories existing check
         Songs are sorted by filename on SHUFFLE = 0
         Now IceG is a GNU standard package, hope that multiple platforms work now
         Added MP3/OGG recursive directory search option (thx to DIeMONd again for suggesting me)
         MP3/OGG file extension search is now case insensitive
         Update time for metadata information is configurable, even metadata information presence too
 0.5.2 - Added telnet minimal interface
         Added MySQL support
         Code cleaner (again)
         FIXED: heading '/' in log file path
         FIXED: massive bugfix in configure script
 0.5.3 - FIXED: Missing declaration on Mac OS X
         FIXED: Alpha signal.h compilation error
         FIXED: removed POSIX definition, cause BSD conflict
 0.5.4 - FIXED: Incorrect socket definitions on MacOSX
         FIXED: Segfault occurs if log object is NULL when error
         FIXED: --with-shout parameter (-lshout -lvorbis missing)
         Added PostGreSQL support
         Added .M3U playlist support
         Added .PLS playlist support
         Removed basename function cause is not POSIX compliant
         Log file is now path configurable
         Added more info into README file
 0.5.5 - FIXED: Serious bug in configuration file parser module
         FIXED: Don't try to open non-existing files during stream
         FIXED: Race condition on meta streamer <-> selector launch
         FIXED: Memory leaks
         Added ID3 support
         Added ICEMETAL language support
         Default icegenerator.conf path is now SYSCONFDIR-fixed
*/

#ifdef HAVE_CONFIG_H
  #ifndef __main_config_h__
    #define __main_config_h__
    #include "../config.h"
  #endif

  #ifndef __GNU_LIBRARY__
    #define __GNU_LIBRARY__
  #endif

  #ifdef HAVE_GETOPT_H
    #include <getopt.h>
  #endif

  #ifdef HAVE_STDIO_H
    #include <stdio.h>
  #endif
 
  #ifdef HAVE_STDLIB_H
    #include <stdlib.h>
  #endif

  #ifdef HAVE_PTHREAD_H
    #include <pthread.h>
  #endif

  #ifdef HAVE_SYS_TYPES_H
    #include <sys/types.h>
  #endif

  #ifdef HAVE_SYS_STAT_H
    #include <sys/stat.h>
  #endif
  
  #ifdef HAVE_FCNTL_H
    #include <fcntl.h>
  #endif

  #ifdef HAVE_CTYPE_H
    #include <ctype.h>
  #endif
	
  #ifdef HAVE_STRING_H
    #include <string.h>
  #endif

  #ifdef HAVE_UNISTD_H
    #include <unistd.h>
  #endif
  
  #define APPNAME PACKAGE_STRING
#else
  #define APPNAME "IceGenerator"
#endif

#include "log.h"
#include "config.h"
#include "sgnl_handler.h"
#include "streamer.h"
#include "circular.h"
#include "synch.h"
#include "main.h"

#define HELP_MSG_LINES 8

const char *help_msg[HELP_MSG_LINES] = { APPNAME,
                                         NULL,
                                         "Usage:",
                                         "\ticegenerator [-h] [-d] [-f filename]",
                                         NULL,
                                         "\t-h\t\tDisplay this help message",
                                         "\t-f filename\tSpecify configuration file",
                                         "\t-d\t\tRun in foreground - no daemon" };
cLog *log_obj = NULL;
cSignal *sig_obj = NULL;
cConfig *conf_obj = NULL;
cStreamer *stream_obj = NULL;
cCircularList *ready_obj = NULL, *played_obj = NULL;
cSemaphore *sem_obj = NULL;
int track_load, end_track, data_mutex, start_meta;
volatile bool time_to_quit = false;
bool ok_to_end = false, changed = false;
char ErrStr[127] = "Unknown error";

void signal_termination_proc(const int sig)
{
  if (!time_to_quit)
    log_obj->WriteLog("Wait for all child process to terminate...");
  time_to_quit = true;
}

void clean_objs()
{
  if (sem_obj != NULL)
    delete sem_obj;

  if (played_obj != NULL)
    delete played_obj;
  
  if (ready_obj != NULL)  
    delete ready_obj;
  
  if (stream_obj != NULL)
    delete stream_obj;
  
  if (conf_obj != NULL)
    delete conf_obj;
  
  if (sig_obj != NULL)
    delete sig_obj;
  
  if (log_obj != NULL)
    delete log_obj;
}

int main(int argc, char **argv)
{
  int i, c;
  bool is_daemon = true;
  pthread_t player_tid, selector_tid, data_streamer_tid, data_server_tid;

  try
  {
    /* Parse command line parameters and reads configuration file */
    if (argc > 1)
      while ((c = getopt (argc, argv, "hdf:")) != -1)
        switch (c)
        {
      	  case 'h': for (i = 0; i < HELP_MSG_LINES; i++)
      	            {
      	              if (help_msg[i] != NULL)
      	                printf("%s",help_msg[i]);
      	              printf("\n");
      	            }
      	            printf("\n");
      	            exit(0);
      	            break;
      	  case 'f': conf_obj = new cConfig(optarg);
      	                       break;
      	  case 'd': is_daemon = false;
      	            break;
      	  case ':': printf("Invalid parameter\n");
      	            exit(1);
      	            break;
        }
    
    if (conf_obj == NULL)
      conf_obj = new cConfig(DEFAULT_CONF_FILE);

    /* Init log file */
    log_obj = new cLog(conf_obj);
    
    /* Select streaming type and scans directories */
    ready_obj = new cCircularList;
    ready_obj->ParsePlayList(conf_obj);
    if (ready_obj->Empty())
    {
      strcpy(ErrStr,"No files found");
      throw 0;
    }
    else
    {
      played_obj = new cCircularList;
      ready_obj->SetStart();
    }

    /* Opens connection to stream server */
    stream_obj = new cStreamer;
    stream_obj->SetData(conf_obj);
    stream_obj->Connect();

    log_obj->WriteLog("Connected to stream server");
    
    /* Init synchronization module */
    sem_obj = new cSemaphore;
    data_mutex = sem_obj->AddSem(1);
    track_load = sem_obj->AddSem(0);
    end_track = sem_obj->AddSem(0);
    start_meta = sem_obj->AddSem(0);

    /* Launches as a daemon */
    if (is_daemon)
    {
      fclose(stdin);
      fclose(stdout);
      fclose(stderr);
      if (fork())
        exit(0);
      setsid();
      if (fork())
        exit(0);
      chdir("/");
      umask(0);
      log_obj->WriteLog("Going to daemon land...");
    }

    /* Install signal handler */
    sig_obj = new cSignal();
    sig_obj->AddHandler(SIGHUP,signal_termination_proc);
    sig_obj->AddHandler(SIGTERM,signal_termination_proc);
    sig_obj->AddHandler(SIGINT,signal_termination_proc);
    sig_obj->AddHandler(SIGQUIT,signal_termination_proc);
    sig_obj->AddHandler(SIGABRT,signal_termination_proc);

    /* Generates threads */
    pthread_create(&player_tid,NULL,player,NULL);
    pthread_create(&data_streamer_tid,NULL,data_streamer,NULL);
    pthread_create(&selector_tid,NULL,selector,NULL);
    if (conf_obj->GetValue(_dataport) != NULL)
      pthread_create(&data_server_tid,NULL,data_server,NULL);

    /* Doesn't care about SIGS */
    sig_obj->Block();

    /* Waits for childen to die */
    pthread_join(player_tid,NULL);
    pthread_join(data_streamer_tid,NULL);
    pthread_join(selector_tid,NULL);
	
    if (conf_obj->GetValue(_dataport) != NULL)
    {
      close_data_server();
      pthread_join(data_server_tid,NULL);
    }

    log_obj->WriteLog("Have a nice day");

    /* Time to go out */
    stream_obj->Disconnect();
    clean_objs();
  }
  catch (...)
  {
    /* Something went wrong ... */
    if (is_daemon && (log_obj != NULL))
      log_obj->WriteLog(ErrStr);
    else
      printf("%s\n\n",ErrStr);

    clean_objs();
  }

  return 0;
}
