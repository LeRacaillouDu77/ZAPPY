find_package(PkgConfig REQUIRED)
pkg_check_modules(SFML REQUIRED sfml-all)

add_executable(unit_tests_gui tests.cpp ../GUI/World/world.cpp ../GUI/World/tile.cpp ../GUI/Data/player_sprite.cpp)
target_link_libraries(unit_tests_gui criterion ${SFML_LIBRARIES})
target_include_directories(unit_tests_gui PRIVATE ${SFML_INCLUDE_DIRS})
target_compile_options(unit_tests_gui PRIVATE ${SFML_CFLAGS_OTHER})

add_executable(unit_tests_server tests_server.c ../Server/Data/parse_args.c)
target_link_libraries(unit_tests_server criterion)

enable_testing()
add_test(NAME unit_tests_gui COMMAND unit_tests_gui)
add_test(NAME unit_tests_server COMMAND unit_tests_server)
