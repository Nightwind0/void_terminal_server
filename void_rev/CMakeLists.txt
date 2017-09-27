cmake_minimum_required(VERSION 3.0) # I had problems if I only specified "VERSION 3".
project(void_server)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CONFIGURATION_TYPES "Release;Debug" CACHE STRING "" FORCE)


# Options to correctly link the standard C++ lib on Mac.
if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin") # This is the correct way to detect Mac OS X operating system -- see http://www.openguru.com/2009/04/cmake-detecting-platformoperating.html
    set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY "libc++")
    if(${CMAKE_CXX_COMPILER_ID} MATCHES "Clang") # GCC ("GNU") probably would require a different option
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
    endif()
endif()

# External packages
set(PostgreSQL_ADDITIONAL_VERSIONS "9.5.8")
find_package(PostgreSQL REQUIRED)

INCLUDE_DIRECTORIES(${PostgreSQL_INCLUDE_DIRS} src src/commands src/thread src/commands/computer src/commands/dock src/commands/behavior src/tools src/handles src/thread src/socket src/commands/behaviors/)

# Executables

set(server_HEADERS
  src/EdgeLoadThread.h
  src/Event.h
  src/Field.h
  src/Message.h
  src/PrimaryKey.h
  src/Resource.h
  src/ResourceMaster.h
  src/Universe.h
  src/VoidServerThread.h
  src/VoidThreadSpawner.h
  src/void_util.h
  src/commands/VoidCommandAttack.h
  src/commands/VoidCommandClaim.h
  src/commands/VoidCommandDisplay.h
  src/commands/VoidCommandHail.h
  src/commands/VoidCommandPort.h
  src/commands/VoidCommandScan.h
  src/commands/VoidCommandTow.h
  src/commands/VoidCommandBeam.h
  src/commands/VoidCommandComputer.h
  src/commands/VoidCommandDock.h
  src/commands/VoidCommandMail.h
  src/commands/VoidCommandPurchaseShipItem.h
  src/commands/VoidCommandStatus.h
  src/commands/VoidCommandTransmit.h
  src/commands/VoidCommandCheckMail.h
  src/commands/VoidCommandDeploy.h
  src/commands/VoidCommand.h
  src/commands/VoidCommandMove.h
  src/commands/VoidCommandReclaim.h
  src/commands/VoidCommandSubCommands.h
  src/commands/VoidCommandWho.h
  src/commands/behaviors/Behavior.h
  src/commands/behaviors/EvaluateShipBehavior.h
  src/commands/behaviors/ShipListBehavior.h
  src/commands/computer/VoidCommandComputerCloak.h
  src/commands/computer/VoidCommandComputerPlayers.h
  src/commands/computer/VoidCommandComputerShipList.h
  src/commands/dock/VoidCommandDockArmory.h
  src/commands/dock/VoidCommandDockArmoryMissiles.h
  src/commands/dock/VoidCommandDockArmorySentinels.h
  src/commands/dock/VoidCommandDockArmoryShields.h
  src/commands/dock/VoidCommandDockShipYardBuy.h
  src/commands/dock/VoidCommandDockShipYard.h
  src/commands/dock/VoidCommandDockShipYardHolds.h
  src/commands/dock/VoidCommandDockShipYardList.h
  src/commands/dock/VoidCommandDockShipYardSell.h
  src/commands/dock/VoidCommandDockShipYardSpec.h
  src/handles/LoginHandle.h
  src/handles/OutpostHandle.h
  src/handles/PlayerHandle.h
  src/handles/SerialObject.h
  src/handles/ShipHandle.h
  src/handles/ShipTypeHandle.h
  src/socket/SocketException.h
  src/socket/Socket.h
  src/thread/Condition.h
  src/thread/Mutex.h
  src/thread/PThreadException.h
  src/thread/Semaphore.h
  src/thread/thread.h
  src/tools/CombatTools.h
  src/tools/CommTools.h
  src/tools/SentinelTools.h
  src/tools/ShipTools.h
  src/tools/ToolSet.h
)

set(server_SOURCES 
  src/EdgeLoadThread.cxx
  src/Event.cxx
  src/Field.cxx
  src/Message.cxx
  src/PrimaryKey.cxx
  src/Resource.cxx
  src/ResourceMaster.cxx
  src/Universe.cxx
  src/VoidServerThread.cxx
  src/VoidThreadSpawner.cxx
  src/void_util.cxx
  src/commands/VoidCommandAttack.cxx
  src/commands/VoidCommandClaim.cxx
  src/commands/VoidCommandDisplay.cxx
  src/commands/VoidCommandHail.cxx
  src/commands/VoidCommandPort.cxx
  src/commands/VoidCommandScan.cxx
  src/commands/VoidCommandTow.cxx
  src/commands/VoidCommandBeam.cxx
  src/commands/VoidCommandComputer.cxx
  src/commands/VoidCommandDock.cxx
  src/commands/VoidCommandMail.cxx
  src/commands/VoidCommandPurchaseShipItem.cxx
  src/commands/VoidCommandStatus.cxx
  src/commands/VoidCommandTransmit.cxx
  src/commands/VoidCommandCheckMail.cxx
  src/commands/VoidCommandDeploy.cxx
  src/commands/VoidCommand.cxx
  src/commands/VoidCommandMove.cxx
  src/commands/VoidCommandReclaim.cxx
  src/commands/VoidCommandSubCommands.cxx
  src/commands/VoidCommandWho.cxx
  src/commands/behaviors/EvaluateShipBehavior.cxx
  src/commands/behaviors/ShipListBehavior.cxx
  src/commands/computer/VoidCommandComputerCloak.cxx
  src/commands/computer/VoidCommandComputerPlayers.cxx
  src/commands/computer/VoidCommandComputerShipList.cxx
  src/commands/dock/VoidCommandDockArmory.cxx
  src/commands/dock/VoidCommandDockArmoryMissiles.cxx
  src/commands/dock/VoidCommandDockArmorySentinels.cxx
  src/commands/dock/VoidCommandDockArmoryShields.cxx
  src/commands/dock/VoidCommandDockShipYardBuy.cxx
  src/commands/dock/VoidCommandDockShipYard.cxx
  src/commands/dock/VoidCommandDockShipYardHolds.cxx
  src/commands/dock/VoidCommandDockShipYardList.cxx
  src/commands/dock/VoidCommandDockShipYardSell.cxx
  src/commands/dock/VoidCommandDockShipYardSpec.cxx
  src/handles/LoginHandle.cxx
  src/handles/OutpostHandle.cxx
  src/handles/PlayerHandle.cxx
  src/handles/SerialObject.cxx
  src/handles/ShipHandle.cxx
  src/handles/ShipTypeHandle.cxx
  src/socket/Socket.cxx
  src/thread/Condition.cxx
  src/thread/Mutex.cxx
  src/thread/Semaphore.cxx
  src/thread/Thread.cxx
  src/tools/CombatTools.cxx
  src/tools/CommTools.cxx
  src/tools/SentinelTools.cxx
  src/tools/ShipTools.cxx
  src/tools/ToolSet.cxx
  src/server.cxx
)

add_executable(server ${server_HEADERS} ${server_SOURCES})

set_property(TARGET server PROPERTY CXX_STANDARD 14)


target_include_directories(
    server PUBLIC
    src
)

target_link_libraries(server PUBLIC ${PostgreSQL_LIBRARIES} pthread)

