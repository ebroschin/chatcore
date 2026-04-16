add_executable(chatcore-server
    source/main.cc
    source/application/chat_server_application.cpp
    source/application/chat_server_application.h
    source/chat/adapters/chat_persistence_adapter.h
    source/chat/adapters/sqlite_chat_persistence_adapter.cpp
    source/chat/adapters/sqlite_chat_persistence_adapter.h
    source/chat/chat_server_system.cpp
    source/chat/chat_server_system.h
    source/application/chat_tcp_system.h
    source/application/chat_persistence_system.h
    source/users/user_server_system.cc
    source/users/user_server_system.h
    source/users/adapters/user_persistence_adapter.h
    source/users/adapters/sqlite_user_persistence_adapter.cc
    source/users/adapters/sqlite_user_persistence_adapter.h
    source/chat/chat_channel_store.cc
    source/chat/chat_channel_store.h
    source/chat/chat_message_store.cc
    source/chat/chat_message_store.h
    source/chat/chat_channel_message_log.cc
    source/chat/chat_channel_message_log.h
    source/application/application_system.cc
    source/application/application_system.h
    source/application/chat_server_arguments.cc
    source/application/chat_server_arguments.h
    source/application/connection_event_handler.cc
    source/application/connection_event_handler.h
    source/users/user_store.cc
    source/users/user_store.h
)

if(WIN32)
    set(BOOST_STACKTRACE stacktrace_windbg)
else()
    set(BOOST_STACKTRACE stacktrace_backtrace)
endif()

find_package(Boost REQUIRED COMPONENTS ${BOOST_STACKTRACE})
find_package(SQLiteCpp CONFIG REQUIRED)

target_link_libraries(chatcore-server PRIVATE
    SQLiteCpp
    Boost::${BOOST_STACKTRACE}
    ebroschin::core
    ebroschin::utility
    ebroschin::network
    ebroschin::network-modules
    ebroschin::chatcore-api
    ebroschin::scheduling
    ebroschin::logging
    ebroschin::logging-modules
    ebroschin::persistence
    ebroschin::persistence-modules
)

if(BUILD_TESTING)
    find_package(GTest CONFIG REQUIRED)

    add_executable(chatcore_server_tests
        tests/test_chat_channel_message_log.cc
        tests/test_chat_channel_store.cc
        tests/test_chat_message_store.cc
        tests/test_chat_server_arguments.cc
        tests/test_user_store.cc
        tests/mock_chat_persistence_adapter.hpp
        source/users/user_store.cc
        source/chat/chat_channel_message_log.cc
        source/chat/chat_channel_store.cc
        source/chat/chat_message_store.cc
        source/application/chat_server_arguments.cc
        tests/mock_user_persistence_adapter.hpp
    )

    target_link_libraries(chatcore_server_tests PRIVATE
        ebroschin::chatcore-api
        ebroschin::core
        ebroschin::utility
        ebroschin::network
        ebroschin::network-modules
        ebroschin::logging
        GTest::gtest
        GTest::gtest_main
    )

    target_include_directories(chatcore_server_tests PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/source
    )

    include(GoogleTest)
    gtest_discover_tests(chatcore_server_tests)
endif()