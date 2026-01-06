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
        source/prototyping/client_test_system.cpp
        source/prototyping/client_test_system.h
        source/prototyping/prototyping_system.h
        source/boost_communication/boost_tcp_server.cpp
        source/boost_communication/boost_tcp_server.h
        source/persistence/persistence_system.h
        source/persistence/persistence_store.h
        source/persistence/persistence_adapter.h
        source/chat/adapters/chat_persistence_adapter.h
        source/sqlite_persistence/sqlite_persistence_store.cpp
        source/sqlite_persistence/sqlite_persistence_store.h
        source/chat/adapters/sqlite_chat_persistence_adapter.cpp
        source/chat/adapters/sqlite_chat_persistence_adapter.h
        source/chat/chat_server_system.cpp
        source/chat/chat_server_system.h
        source/communication/message_handler_registry.h
        source/communication/message_handler.h
        source/prototyping/chat_input_system.cc
        source/prototyping/chat_input_system.h
        source/prototyping/ping_server_system.h
        source/communication/tcp_connection.h
        source/boost_communication/boost_tcp_connection.cc
        source/boost_communication/boost_tcp_connection.h
        source/boost_communication/boost_tcp_client.cc
        source/boost_communication/boost_tcp_client.h
        source/application/chat_tcp_system.h
        source/application/chat_persistence_system.h
        source/utility/concepts.h
        source/prototyping/fallback_message_handler.h
        source/communication/tcp_system_builder.h
        source/utility/variadic.h
        source/communication/tcp_system_concepts.h
        source/codec/test_codec.h
)

find_package(Boost REQUIRED COMPONENTS stacktrace_backtrace)
find_package(claw-core CONFIG REQUIRED)
find_package(claw-utility CONFIG REQUIRED)
find_package(SQLiteCpp CONFIG REQUIRED)

target_link_libraries(chat-server PRIVATE SQLiteCpp Boost::stacktrace_backtrace claw::core)
