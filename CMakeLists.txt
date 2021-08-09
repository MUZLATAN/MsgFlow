cmake_minimum_required(VERSION 3.5)
project(MsgFlow)

set(CMAKE_CXX_STANDARD 14)

include_directories(/env/platforms/x86_64/include )
link_directories(/env/platforms/x86_64/so /env/platforms/x86_64/static)

add_executable(MsgFlow main.cpp MsgFlow.cpp MsgFlow.h)

target_link_libraries(BoostFileProcess
        pthread
        boost_filesystem
        boost_system
        )
