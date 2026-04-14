add_library(claw_scheduling
    src/scheduling_system.cc
    include/claw/scheduling/commons.h
)

target_link_libraries(claw_scheduling PRIVATE
    claw::core
)

add_library(claw::scheduling ALIAS claw_scheduling)
set_target_properties(claw_scheduling PROPERTIES EXPORT_NAME scheduling)
target_include_directories(claw_scheduling PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

option(BUILD_TESTING "Build tests" OFF)
if(BUILD_TESTING)
    enable_testing()
    find_package(GTest CONFIG REQUIRED)

    add_executable(claw_scheduling_tests
            tests/test_scheduling_system.cpp
    )

    target_link_libraries(claw_scheduling_tests PRIVATE
            claw::core
            claw_scheduling
            GTest::gtest
            GTest::gtest_main
    )

    include(GoogleTest)
    gtest_discover_tests(claw_scheduling_tests)
endif()

target_compile_features(claw_scheduling PUBLIC cxx_std_23)