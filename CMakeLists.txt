cmake_minimum_required(VERSION 4.0)
project(chat-server)
set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=address -fno-omit-frame-pointer")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fno-omit-frame-pointer")
    set(CMAKE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS} -fsanitize=address")
endif()

add_executable(chat-server source/main.cc
        source/application/chat_server_application.cpp
        source/application/chat_server_application.h
        source/prototyping/client_communication_system.cpp
        source/prototyping/client_communication_system.h
        source/prototyping/prototyping_system.cpp
        source/prototyping/prototyping_system.h
        source/communication/tcp_server.h
        source/communication/tcp_server_system.cpp
        source/boost_communication/boost_tcp_server.cpp
        source/boost_communication/boost_tcp_server.h
        source/persistence/persistence_system.cpp
        source/persistence/persistence_system.h
        source/persistence/persistence_store.h
        source/persistence/persistence_adapter.h
        source/chat/chat_persistence_adapter.h
        source/sqlite_persistence/sqlite_persistence_store.cpp
        source/sqlite_persistence/sqlite_persistence_store.h
        source/chat/sqlite_chat_persistence_adapter.cpp
        source/chat/sqlite_chat_persistence_adapter.h
        source/chat/chat_server_system.cpp
        source/chat/chat_server_system.h
)

find_package(Boost REQUIRED COMPONENTS stacktrace_backtrace)
find_package(claw-core CONFIG REQUIRED)
find_package(claw-utility CONFIG REQUIRED)
find_package(SQLiteCpp CONFIG REQUIRED)

target_link_libraries(chat-server PRIVATE SQLiteCpp Boost::stacktrace_backtrace claw::core)
