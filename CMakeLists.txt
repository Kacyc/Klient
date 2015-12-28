project(Sieci)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread --std=c++11 -Wall")
add_executable(Sieci main.cpp socket.cpp inotify.cpp)

install(TARGETS Sieci RUNTIME DESTINATION bin)
