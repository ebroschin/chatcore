find_package(Boost REQUIRED COMPONENTS ${BOOST_STACKTRACE})

add_library(claw_network_modules
    include/claw/network-modules/codecs/json_network_codec.h
    include/claw/network-modules/connectors/boost_async/boost_tcp_acceptor.h
    include/claw/network-modules/connectors/boost_async/boost_tcp_resolver.h
    include/claw/network-modules/connectors/boost_async/boost_tcp_connection.h
    include/claw/network-modules/message_handler/direct_message_handler.h
    include/claw/network-modules/message_handler/observable_message_handler.h
    include/claw/network-modules/rpc_timeout_handler/scheduler_rpc_timeout_handler.h
    src/connectors/boost_async/boost_tcp_acceptor.cc
    src/connectors/boost_async/boost_tcp_resolver.cc
    src/connectors/boost_async/boost_tcp_connection.cc
    src/rpc_timeout_handler/scheduler_rpc_timeout_handler.cc
)

add_library(claw::network-modules ALIAS claw_network_modules)
set_target_properties(claw_network_modules PROPERTIES EXPORT_NAME network-modules)

target_include_directories(claw_network_modules PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(claw_network_modules
    PUBLIC
        claw::utility
        claw::network
        claw::scheduling
        Boost::${BOOST_STACKTRACE}
)

option(BUILD_TESTING "Build tests" OFF)
if(BUILD_TESTING)
    enable_testing()
    find_package(GTest CONFIG REQUIRED)

    add_executable(tests
            tests/test_json_codec.cc
    )

    target_link_libraries(tests PRIVATE
            claw_network_modules
            claw::network
            claw::utility
            claw::core
            GTest::gtest
            GTest::gtest_main
    )

    include(GoogleTest)
    gtest_discover_tests(tests)
endif()

target_compile_features(claw_network_modules INTERFACE cxx_std_23)