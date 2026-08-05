file(GLOB_RECURSE SRC CONFIGURE_DEPENDS
    source/*.cpp
    source/*.hpp
)

add_executable(chatcore-load-tester
    ${SRC}
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