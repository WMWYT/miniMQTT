cmake_minimum_required(VERSION 3.22.1)

project(miniMQTT)

find_package(OpenSSL REQUIRED)

add_subdirectory(mqtt)
add_subdirectory(net)
add_subdirectory(config)
add_subdirectory(utils)
add_subdirectory(log)
add_subdirectory(control)

add_executable(miniMQTT miniMQTT.c)
add_executable(test test/test.c test/test_event.c)

target_link_libraries(miniMQTT mqtt net config iniparser log)
target_link_libraries(test mqtt log)