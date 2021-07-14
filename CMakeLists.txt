cmake_minimum_required(VERSION 3.5)
project(BoostFileProcess)

set(CMAKE_CXX_STANDARD 14)

include_directories(/env/platforms/x86_64/include )
link_directories(/env/platforms/x86_64/so /env/platforms/x86_64/static)

add_executable(BoostFileProcess main.cpp FlowRpcProcessor.cpp FlowRpcProcessor.h)

target_link_libraries(BoostFileProcess
        pthread
        boost_filesystem
        boost_system
        )
