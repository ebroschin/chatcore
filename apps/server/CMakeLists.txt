add_executable(chatcore-server
    source/main.cpp
    source/application/chat_server_application.cpp
    source/application/chat_server_application.hpp
    source/chat/adapters/chat_persistence_adapter.hpp
    source/chat/adapters/sqlite_chat_persistence_adapter.cpp
    source/chat/adapters/sqlite_chat_persistence_adapter.hpp
    source/chat/chat_server_system.cpp
    source/chat/chat_server_system.hpp
    source/application/chat_tcp_system.hpp
    source/application/chat_persistence_system.hpp
    source/users/user_server_system.cpp
    source/users/user_server_system.hpp
    source/users/adapters/user_persistence_adapter.hpp
    source/users/adapters/sqlite_user_persistence_adapter.cpp
    source/users/adapters/sqlite_user_persistence_adapter.hpp
    source/chat/chat_channel_store.cpp
    source/chat/chat_channel_store.hpp
    source/chat/chat_message_store.cpp
    source/chat/chat_message_store.hpp
    source/chat/chat_channel_message_log.cpp
    source/chat/chat_channel_message_log.hpp
    source/application/application_system.cpp
    source/application/application_system.hpp
    source/application/chat_server_arguments.cpp
    source/application/chat_server_arguments.hpp
    source/application/connection_event_handler.cpp
    source/application/connection_event_handler.hpp
    source/users/user_store.cpp
    source/users/user_store.hpp
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
        tests/test_chat_channel_message_log.cpp
        tests/test_chat_channel_store.cpp
        tests/test_chat_message_store.cpp
        tests/test_chat_server_arguments.cpp
        tests/test_user_store.cpp
        tests/mock_chat_persistence_adapter.hpp
        source/users/user_store.cpp
        source/chat/chat_channel_message_log.cpp
        source/chat/chat_channel_store.cpp
        source/chat/chat_message_store.cpp
        source/application/chat_server_arguments.cpp
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