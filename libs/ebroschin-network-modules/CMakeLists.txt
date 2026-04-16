find_package(Boost REQUIRED COMPONENTS ${BOOST_STACKTRACE})

add_library(ebroschin_network_modules
    include/ebroschin/network-modules/codecs/json_network_codec.h
    include/ebroschin/network-modules/connectors/boost_async/boost_tcp_acceptor.h
    include/ebroschin/network-modules/connectors/boost_async/boost_tcp_resolver.h
    include/ebroschin/network-modules/connectors/boost_async/boost_tcp_connection.h
    include/ebroschin/network-modules/message_handler/direct_message_handler.h
    include/ebroschin/network-modules/message_handler/observable_message_handler.h
    include/ebroschin/network-modules/rpc_timeout_handler/scheduler_rpc_timeout_handler.h
    src/connectors/boost_async/boost_tcp_acceptor.cc
    src/connectors/boost_async/boost_tcp_resolver.cc
    src/connectors/boost_async/boost_tcp_connection.cc
    src/rpc_timeout_handler/scheduler_rpc_timeout_handler.cc
)

add_library(ebroschin::network-modules ALIAS ebroschin_network_modules)
set_target_properties(ebroschin_network_modules PROPERTIES EXPORT_NAME network-modules)

target_include_directories(ebroschin_network_modules PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(ebroschin_network_modules PUBLIC
    ebroschin::utility
    ebroschin::network
    ebroschin::scheduling
    Boost::${BOOST_STACKTRACE}
)

if(BUILD_TESTING)
    find_package(GTest CONFIG REQUIRED)

    add_executable(ebroschin_network_modules_tests
        tests/test_json_codec.cc
    )

    target_link_libraries(ebroschin_network_modules_tests PRIVATE
            ebroschin_network_modules
            ebroschin::network
            ebroschin::utility
            ebroschin::core
            GTest::gtest
            GTest::gtest_main
    )

    include(GoogleTest)
    gtest_discover_tests(ebroschin_network_modules_tests)
endif()

target_compile_features(ebroschin_network_modules INTERFACE cxx_std_23)