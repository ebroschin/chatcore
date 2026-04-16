add_library(claw_network INTERFACE
        include/claw/network/tcp/tcp_system.h
        include/claw/network/tcp/tcp_connection.h
        include/claw/network/commons.h
        include/claw/network/tcp/tcp_system_concepts.h
        include/claw/network/tcp/tcp_system_builder.h
        include/claw/network/tcp/tcp_message_processor.h
        include/claw/network/tcp/tcp_system_connection_facade.h
        include/claw/network/rpc/commons.h
        include/claw/network/rpc/rpc_call_builder.h
        include/claw/network/rpc/rpc_system.h
        include/claw/network/tcp/tcp_connection_event_handler.h
        include/claw/network/tcp/tcp_connector.h
)

add_library(claw::network ALIAS claw_network)
set_target_properties(claw_network PROPERTIES EXPORT_NAME network)

target_include_directories(claw_network INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(claw_network INTERFACE
    claw::utility
    claw::core
)

if(BUILD_TESTING)
    find_package(GTest CONFIG REQUIRED)

    add_executable(claw_network_tests
            tests/test_tcp_system.cc
    )

    target_link_libraries(claw_network_tests PRIVATE
            claw_network
            claw::utility
            claw::core
            GTest::gtest
            GTest::gtest_main
    )

    include(GoogleTest)
    gtest_discover_tests(claw_network_tests)
endif()

target_compile_features(claw_network INTERFACE cxx_std_23)