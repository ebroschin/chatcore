file(GLOB_RECURSE SRC CONFIGURE_DEPENDS
    source/*.cpp
    source/*.hpp
)

add_executable(chatcore-server
    ${SRC}
)

find_package(SQLiteCpp CONFIG REQUIRED)

target_link_libraries(chatcore-server PRIVATE
    SQLiteCpp
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