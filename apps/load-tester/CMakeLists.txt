add_executable(chatcore-load-tester
    source/main.cc
    source/application/client_tcp_system.h
    source/application/load_tester_application.cc
    source/application/load_tester_application.h
    source/application/application_system.cc
    source/application/application_system.h
    source/network/client.cc
    source/network/client.h
    source/network/root_client.cc
    source/network/root_client.h
    source/network/test_client.cc
    source/network/test_client.h
    source/network/client_report.h
    source/application/load_tester_arguments.cc
    source/application/load_tester_arguments.h
    source/application/client_rpc_system.h
)

if(WIN32)
    set(BOOST_STACKTRACE stacktrace_windbg)
else()
    set(BOOST_STACKTRACE stacktrace_backtrace)
endif()

find_package(Boost REQUIRED COMPONENTS ${BOOST_STACKTRACE})

target_link_libraries(chatcore-load-tester PRIVATE
    Boost::${BOOST_STACKTRACE}
    claw::core
    claw::utility
    claw::network
    claw::network-modules
    claw::chat-api
    claw::scheduling
    ebroschin::logging
    ebroschin::logging-modules
)

if(BUILD_TESTING)
    find_package(GTest CONFIG REQUIRED)

    add_executable(chatcore_load_tester_tests
        tests/test_load_tester_arguments.cc
        source/application/load_tester_arguments.cc
    )

    target_link_libraries(chatcore_load_tester_tests PRIVATE
        claw::utility
        ebroschin::logging
        GTest::gtest
        GTest::gtest_main
    )

    target_include_directories(chatcore_load_tester_tests PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/source
    )

    include(GoogleTest)
    gtest_discover_tests(chatcore_load_tester_tests)
endif()