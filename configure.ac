AC_INIT([IceGenerator],0.5.5,administrator@tortugalabs.it)
AC_COPYRIGHT([Copyright 2004 Tortuga Labs. Inc.])
AC_CANONICAL_HOST
AC_CONFIG_SRCDIR(src/main.cpp)
AM_INIT_AUTOMAKE(icegenerator,0.5.5)
AM_CONFIG_HEADER(config.h)
AM_SANITY_CHECK
AM_MAINTAINER_MODE

MAJOR_VERSION=0
MINOR_VERSION=5
RELEASE_VERSION=5
VERSION="$MAJOR_VERSION.$MINOR_VERSION.$RELEASE_VERSION"

if test "x$prefix" = xNONE; then
  prefix=`eval echo ${ac_default_prefix}`
fi

if test "x$exec_prefix" = xNONE; then
  exec_prefix=`eval echo ${ac_default_prefix}`
fi

dnl checks for programs needed to build.
CXXFLAGS="-ansi -pedantic"

AC_PROG_INSTALL
AC_PROG_CC
AM_PROG_CC_STDC
AC_PROG_CXX
AC_LANG_CPLUSPLUS

AC_PROG_MAKE_SET
AC_PROG_LN_S

dnl debug check
AC_ARG_ENABLE(debug,
  [  --enable-debug          enable debug (default disabled)],
  [ CXXFLAGS="$CXXFLAGS -g" ], 
  [ CXXFLAGS="$CXXFLAGS -pipe -Wall -O3" ])

dnl libshout path check
AC_ARG_WITH(shout,[  --with-shout=DIR        libshout include and library files prefix path],[shoutdir=$withval],[shoutdir=""])

dnl MySQL path check
AC_ARG_WITH(mysql,[  --with-mysql            MySQL support],[mysql_en="yes"],[mysql_en=""])

dnl PostGreSQL path check
AC_ARG_WITH(pgsql,[  --with-pgsql            PostGreSQL support],[pgsql_en="yes"],[pgsql_en=""])

dnl ID3 path check
AC_ARG_WITH(id3,[  --with-id3              ID3 support],[id3_en="yes"],[id3_en=""])

dnl checking mkdir -p
AC_MSG_CHECKING([whether mkdir accepts -p])
if (mkdir -p config.tmpdir/p && mkdir -p config.tmpdir/p 2>&1) > /dev/null; then
    AC_MSG_RESULT(yes)
    MKDIR="mkdir -p"
else
    AC_MSG_RESULT(no)
    MKDIR=mkdir
fi
rm -rf config.tmpdir

dnl check for POSIX Threads library
echo -e $'\n'$'\n'POSIX Threads
ACX_PTHREAD([
  LDFLAGS="$LDFLAGS $PTHREAD_LIBS"
  CXXFLAGS="$CXXFLAGS $PTHREAD_CFLAGS"
	])
AC_CHECK_HEADERS(pthread.h)
AC_CHECK_FUNCS(pthread_create pthread_join)
dnl AC_TRY_LINK_FUNC(pthread_create,[have_pthread_funcs=yes],AC_MSG_ERROR([Could not link to libpthread. IceGenerator package uses the pthread library. Install libpthread first.]))
dnl AC_TRY_LINK_FUNC(pthread_join,[have_pthread_funcs=yes],AC_MSG_ERROR([Could not link to libpthread. IceGenerator package uses the pthread library. Install libpthread first.]))

dnl I/O functions
echo -e $'\n'$'\n'I/O functions
AC_HEADER_STDC
AC_HEADER_STAT
AC_CHECK_HEADERS(stdio.h fcntl.h)
AC_CHECK_FUNCS(open read write close printf fopen fprintf fflush fclose sprintf unlink fread fwrite,[have_io_funcs=yes],AC_MSG_ERROR([required function missing]))

dnl log functions
echo -e $'\n'$'\n'System log functions
AC_CHECK_HEADERS(syslog.h)
AC_CHECK_FUNCS(openlog syslog closelog,[have_log_funcs=yes],AC_MSG_ERROR([required function missing]))

dnl network functions
echo -e $'\n'$'\n'Network functions
AC_CHECK_HEADERS(sys/socket.h netinet/in.h arpa/inet.h)
dnl AC_CHECK_LIB(socket,socket,[LDFLAGS="$LDFLAGS -lsocket -lnsl"],,$LDFLAGS)
ETR_SOCKET_NSL
LDFLAGS="$LDFLAGS $ETR_SOCKET_LIBS"
AC_CHECK_FUNCS(socket bind listen accept connect htons htonl inet_addr,[have_net_funcs=yes],AC_MSG_ERROR([required function missing]))
TYPE_SOCKLEN_T

dnl directory functions
echo -e $'\n'$'\n'Directory functions
AC_HEADER_DIRENT
AC_CHECK_HEADERS(dirent.h)
AC_CHECK_FUNCS(opendir readdir closedir,[have_dir_funcs=yes],AC_MSG_ERROR([required function missing]))

dnl string/char functions
echo -e $'\n'$'\n'String and character functions
AC_CHECK_HEADERS(getopt.h ctype.h)
AC_CHECK_FUNCS(getopt memset strcpy atoi strtok strcat strlen toupper strcmp strncmp bzero,[have_str_funcs=yes],AC_MSG_ERROR([required function missing]))

dnl POSIX signal functions
echo -e $'\n'$'\n'POSIX signal functions
AC_CHECK_HEADERS(signal.h)
AC_CHECK_FUNCS(sigaction sigemptyset sigaddset sigprocmask kill,[have_sigs_funcs=yes],AC_MSG_ERROR([required function missing]))

dnl POSIX semaphores functions
echo -e $'\n'$'\n'POSIX semaphores functions
AC_CHECK_HEADERS(semaphore.h)
AC_CHECK_LIB(rt,sem_init,[LDFLAGS="$LDFLAGS -lrt"],,$LDFLAGS)
AC_CHECK_FUNCS(sem_init sem_destroy sem_wait sem_post,[have_sems_funcs=yes],AC_MSG_ERROR([required function missing]))

dnl Utility functions
echo -e $'\n'$'\n'Other functions
AC_HEADER_TIME
AC_STRUCT_TM
AC_CHECK_HEADERS(time.h)
AC_CHECK_FUNCS(sleep time srand rand getpid,[have_other_funcs=yes],AC_MSG_ERROR([required function missing]))

dnl check for shout library path
echo -e $'\n'$'\n'SHOUT library
if test "$shoutdir" != "" ; then
  CXXFLAGS="$CXXFLAGS -I$withval/include"
	LDFLAGS="$LDFLAGS -L$withval/lib -lshout -lvorbis"
	AC_CHECK_HEADERS($withval/include/shout/shout.h)
else
  PKG_CHECK_MODULES(SHOUT, shout >= 2.0)
  CXXFLAGS="$CXXFLAGS $SHOUT_CFLAGS"
  LDFLAGS="$LDFLAGS $SHOUT_LIBS"
  AC_CHECK_HEADERS(shout/shout.h)
fi
AC_CHECK_LIB(shout,shout_init,[have_shout_funcs=yes],AC_MSG_ERROR([Could not link to libshout. IceGenerator package uses the libshout-2.0 library. Install libshout first.]),$LDFLAGS)

dnl check for mysql library
if test "$mysql_en" != "" ; then
  echo -e $'\n'$'\n'MySQL library
  mysql_cflags=`mysql_config --cflags`
  mysql_lflags=`mysql_config --libs`
  CXXFLAGS="$CXXFLAGS $mysql_cflags"
  LDFLAGS="$LDFLAGS $mysql_lflags"
  AC_CHECK_HEADERS(mysql/mysql.h)
  AC_CHECK_LIB(mysqlclient,mysql_init,AC_DEFINE(HAVE_MYSQL,1,[MySQL is enabled]),AC_MSG_ERROR([Could not link to MySQL library.]),$LDFLAGS)
fi

dnl check for pgsql library
if test "$pgsql_en" != "" ; then
  echo -e $'\n'$'\n'PostGreSQL library
  pgsql_cflags=`pg_config --includedir`
  pgsql_lflags=`pg_config --libdir`
  CXXFLAGS="$CXXFLAGS -I$pgsql_cflags"
  LDFLAGS="$LDFLAGS -L$pgsql_lflags -lpq"
  AC_CHECK_HEADERS(libpq-fe.h)
  AC_CHECK_LIB(pq,PQconnectdb,AC_DEFINE(HAVE_PGSQL,1,[PgSQL is enabled]),AC_MSG_ERROR([Could not link to PgSQL library.]),$LDFLAGS)
fi

dnl check for id3 library
if test "$id3_en" != "" ; then
  echo -e $'\n'$'\n'ID3 library
	AC_SEARCH_LIBS(ID3Tag_Link,"id3 -lz -lstdc++",
		id3lib=yes
		AC_DEFINE(HAVE_ID3, [], [Description])
    LDFLAGS="$LDFLAGS -lid3 -lz -lstdc++",
		id3lib=no
		AC_MSG_WARN(id3lib not found - disabling id3v2 support))
dnl  AC_CHECK_HEADERS(tag.h)
fi

dnl =================================================================
dnl Always defined
dnl =================================================================

AC_DEFINE(__MT__,1,
  [Always define this, for thread safety on some platforms.])

SYSCONFDIR=`eval echo ${sysconfdir}`
AC_SUBST(SYSCONFDIR)
AC_DEFINE_UNQUOTED(DEFAULT_CONF_FILE,"`eval echo "${sysconfdir}/icegenerator.conf"`",[Default config file path])

AC_OUTPUT(Makefile src/Makefile)
