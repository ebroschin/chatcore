add_executable(chatcore-load-tester
    source/main.cpp
    source/application/client_tcp_system.hpp
    source/application/load_tester_application.cpp
    source/application/load_tester_application.hpp
    source/application/application_system.cpp
    source/application/application_system.hpp
    source/testing/client.cpp
    source/testing/client.hpp
    source/testing/root_client.cpp
    source/testing/root_client.hpp
    source/testing/test_client.cpp
    source/testing/test_client.hpp
    source/testing/client_report.hpp
    source/application/load_tester_arguments.cpp
    source/application/load_tester_arguments.hpp
    source/application/client_rpc_system.hpp
)

target_link_libraries(chatcore-load-tester PRIVATE
    ebroschin::core
    ebroschin::utility
    ebroschin::network
    ebroschin::network-modules
    ebroschin::chatcore-api
    ebroschin::scheduling
    ebroschin::logging
    ebroschin::logging-modules
)

if(BUILD_TESTING)
    find_package(GTest CONFIG REQUIRED)

    add_executable(chatcore_load_tester_tests
        tests/test_load_tester_arguments.cpp
        tests/test_load_tester_reporting.cpp
        source/application/load_tester_arguments.cpp
    )

    target_link_libraries(chatcore_load_tester_tests PRIVATE
        ebroschin::utility
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