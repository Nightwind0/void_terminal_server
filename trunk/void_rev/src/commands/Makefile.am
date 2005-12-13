noinst_LTLIBRARIES = libcommand.la

SUBDIRS = dock behaviors computer

libcommand_la_LIBADD = dock/libcommanddock.la behaviors/libcommandbehaviors.la computer/libcommandcomputer.la

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
			VoidCommandSubCommands.h \
			VoidCommandCheckMail.h \
			VoidCommandCheckMail.cxx \
			VoidCommandMail.h \
			VoidCommandMail.cxx \
			VoidCommandAttack.h \
			VoidCommandAttack.cxx \
			VoidCommandClaim.cxx \
			VoidCommandClaim.h \
			VoidCommandComputer.h \
			VoidCommandComputer.cxx \
			VoidCommandDeploy.h \
			VoidCommandDeploy.cxx \
			VoidCommandReclaim.h \
			VoidCommandReclaim.cxx \
			VoidCommandScan.h \
			VoidCommandScan.cxx


INCLUDES = 	-I@top_srcdir@/src \
		-I@top_srcdir@/src/handles/ \
		-I@top_srcdir@/src/socket/ \
		-I@top_srcdir@/src/thread/ \
		-I@top_srcdir@/src/tools/ \
		-I@top_srcdir@/src/commands/dock \
		-I@top_srcdir@/src/commands/computer/ \
		-I@top_srcdir@/src/commands/behaviors
