cmake_minimum_required (VERSION 2.8)
project(Sieci)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread --std=c++11 -Wall")
add_executable(client client.cpp connector.cpp stream.cpp inotify.cpp filehandlernormal.cpp filehandlerremove.cpp filehandlerfolder.cpp filehandlermove.cpp filehandler.cpp)
add_executable(server server.cpp acceptor.cpp stream.cpp inotify.cpp filehandlernormal.cpp filehandlerremove.cpp filehandlerfolder.cpp filehandlermove.cpp filehandler.cpp)

install(TARGETS client RUNTIME DESTINATION bin)
install(TARGETS server RUNTIME DESTINATION bin)

