bin_PROGRAMS = server control

server_SOURCES = server.cxx VoidServerThread.cxx EdgeLoadThread.cxx Field.cxx PrimaryKey.cxx Resource.cxx ResourceMaster.cxx Universe.cxx void_util.cxx Message.cxx VoidThreadSpawner.cxx


control_SOURCES = control.cxx


control_LDADD = socket/libsocket.a

SUBDIRS = commands handles thread socket

server_LDADD = commands/libcommand.la \
	handles/libhandles.la \
	socket/libsocket.la \
	thread/libthread.la \
	-lpq 


INCLUDES = 	-I @top_srcdir@/src/commands/ \
		-I @top_srcdir@/src/commands/dock/ \
	 	-I @top_srcdir@/src/handles/ \
		-I @top_srcdir@/src/socket/ \
		-I @top_srcdir@/src/thread/ 


