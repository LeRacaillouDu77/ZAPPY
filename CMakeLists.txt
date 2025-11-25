cmake_minimum_required(VERSION 3.10)
project(Zappy)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Werror")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror -std=c++17")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

include_directories(Include)
include_directories(client)
include_directories(GUI)
include_directories(Menu)

if(APPLE)
    message(STATUS "Detected macOS - using find_package(SFML)")
    set(SFML_MIN_VERSION "2.5")
    find_package(SFML ${SFML_MIN_VERSION} COMPONENTS graphics window system network audio REQUIRED)
    include_directories(${SFML_INCLUDE_DIRS})
    link_directories(${SFML_LIBRARY_DIRS})
else()
    message(STATUS "Detected non-macOS - using pkg-config for SFML")
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(SFML REQUIRED sfml-all)
    include_directories(${SFML_INCLUDE_DIRS})
    link_directories(${SFML_LIBRARY_DIRS})
endif()

set(SERVER_DATA_SOURCES
    Server/Data/server.c
    Server/Data/run_server.c
    Server/Data/gest_clients_data.c
    Server/Data/send_data.c
    Server/Data/parse_args.c
)

set(SERVER_UPDATE_SOURCES
    Server/Update/gest_messages.c
    Server/Update/manage_zappy.c
    Server/Update/team_management.c
    Server/Update/egg_management.c
    Server/Update/manage_gui.c
    Server/Update/client_connection.c
    Server/Update/survival_system.c
    Server/Update/victory_system.c
)

set(SERVER_COMMANDS_SOURCES
    Server/Commands/handle.c
    Server/Commands/command_queue.c
    Server/Commands/verif.c

    Server/Commands/GUI/gui_commands.c
    Server/Commands/GUI/bct.c
    Server/Commands/GUI/mct.c
    Server/Commands/GUI/tna.c
    Server/Commands/GUI/msz.c
    Server/Commands/GUI/pin.c
    Server/Commands/GUI/plv.c
    Server/Commands/GUI/sgt.c
    Server/Commands/GUI/sst.c

    Server/Commands/IA/forward.c
    Server/Commands/IA/right.c
    Server/Commands/IA/left.c
    Server/Commands/IA/look.c
    Server/Commands/IA/inventory.c
    Server/Commands/IA/connect_nbr.c
    Server/Commands/IA/player_nbr.c
    Server/Commands/IA/takeobj.c
    Server/Commands/IA/setobj.c
    Server/Commands/IA/fork.c
    Server/Commands/IA/eject.c
    Server/Commands/IA/broadcast.c
    Server/Commands/IA/incantation.c
    Server/Commands/IA/verif_incant.c
)

set(SERVER_WORLD_SOURCES
    Server/World/init_world.c
    Server/World/world_utils.c
    Server/World/regenerate_ress.c
)

set(SERVER_SOURCES
    ${SERVER_DATA_SOURCES}
    ${SERVER_UPDATE_SOURCES}
    ${SERVER_COMMANDS_SOURCES}
    ${SERVER_WORLD_SOURCES}
)

set(GUI_SOURCES
    GUI/gui.cpp
    GUI/Network/network.cpp
    GUI/World/world.cpp
    GUI/Menu/Menu.cpp
    GUI/Settings/Setting.cpp
    GUI/Themes/Theme.cpp
    GUI/World/tile.cpp
    GUI/Data/player.cpp
    GUI/Data/player_sprite.cpp
    GUI/Data/egg.cpp
    GUI/Data/interaction.cpp
    GUI/render.cpp
    GUI/LoadingScreen/loading.cpp
    GUI/Menu/InGameMenu.cpp
    GUI/Menu/InGameInfoMenu.cpp
    GUI/camera.cpp
    GUI/parasite.cpp
    GUI/playerUI.cpp
    GUI/tileUI.cpp
    GUI/culling.cpp
    GUI/position_cache.cpp
    GUI/Menu/GameEndScreen.cpp
)

add_executable(zappy_server ${SERVER_SOURCES})
target_link_libraries(zappy_server m)

add_executable(zappy_gui ${GUI_SOURCES})

if(APPLE)
    target_link_libraries(zappy_gui
        sfml-graphics
        sfml-window
        sfml-system
        sfml-network
        sfml-audio
    )
else()
    target_link_libraries(zappy_gui ${SFML_LIBRARIES})
    target_compile_options(zappy_gui PRIVATE ${SFML_CFLAGS_OTHER})
endif()

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/Client/zappy_ai.py
    ${CMAKE_CURRENT_SOURCE_DIR}/zappy_ai
    COPYONLY
)

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/zappy_ai
    COMMAND chmod +x ${CMAKE_CURRENT_SOURCE_DIR}/zappy_ai
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/Client/zappy_ai.py
    COMMENT "Création de l'exécutable zappy_ai"
)

add_custom_target(zappy_ai_target ALL
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/zappy_ai
)

add_custom_target(serveur DEPENDS zappy_server)

add_custom_target(gui DEPENDS zappy_gui)

add_custom_target(ia DEPENDS zappy_ai_target)

install(TARGETS zappy_server zappy_gui DESTINATION bin)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/zappy_ai
        DESTINATION bin
        PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)

add_subdirectory(Test)
