bin_PROGRAMS = voidserver voidcontrol

voidserver_SOURCES = server.cxx VoidServerThread.cxx EdgeLoadThread.cxx Field.cxx PrimaryKey.cxx Resource.cxx ResourceMaster.cxx Universe.cxx void_util.cxx Message.cxx VoidThreadSpawner.cxx EdgeLoadThread.h  PrimaryKey.h Universe.h void_util.h Field.h Resource.h VoidServerThread.h Message.h   ResourceMaster.h  VoidThreadSpawner.h



voidcontrol_SOURCES = control.cxx


voidcontrol_LDADD = socket/libsocket.la

SUBDIRS = commands handles thread socket

voidserver_LDADD = commands/libcommand.la \
	handles/libhandles.la \
	socket/libsocket.la \
	thread/libthread.la \
	-lpq 


INCLUDES = 	-I @top_srcdir@/src/commands/ \
		-I @top_srcdir@/src/commands/dock/ \
	 	-I @top_srcdir@/src/handles/ \
		-I @top_srcdir@/src/socket/ \
		-I @top_srcdir@/src/thread/ 


