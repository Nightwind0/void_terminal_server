noinst_LTLIBRARIES = libcommand.la

SUBDIRS = dock

libcommand_la_LIBADD = dock/libcommanddock.la

libcommand_la_SOURCES =  VoidCommand.cxx \
			VoidCommandDisplay.cxx \
			VoidCommandHail.cxx \
			VoidCommandMove.cxx \
			VoidCommandPort.cxx \
			VoidCommandStatus.cxx \
			VoidCommandTransmit.cxx \
			VoidCommandWho.cxx \
			VoidCommandSubCommands.cxx \
			VoidCommandDock.cxx \
			VoidCommandBeam.cxx \
			VoidCommandPurchaseShipItem.cxx \
			VoidCommandTow.cxx \
			VoidCommandBeam.h \
			VoidCommandMove.h \
		        VoidCommandTow.h \
			VoidCommandDisplay.h \
			VoidCommandPort.h \
	                VoidCommandTransmit.h \
			VoidCommandDock.h \
			VoidCommandPurchaseShipItem.h \
			VoidCommandWho.h \
			VoidCommand.h \
		        VoidCommandStatus.h \
			VoidCommandHail.h \
			VoidCommandSubCommands.h



INCLUDES = 	-I@top_srcdir@/src \
		-I@top_srcdir@/src/handles/ \
		-I@top_srcdir@/src/socket/ \
		-I@top_srcdir@/src/thread/ \
		-I@top_srcdir@/src/commands/dock
