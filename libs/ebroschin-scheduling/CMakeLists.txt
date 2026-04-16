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

if(BUILD_TESTING)
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

    enable_testing()

    add_test(
        NAME claw_scheduling_stable
        COMMAND claw_scheduling_tests
            --gtest_filter=-SchedulingSystemTest.PeriodicScheduling:SchedulingSystemTest.OneShotAndPeriodicScheduling
    )

    add_test(
        NAME claw_scheduling_flaky
        COMMAND claw_scheduling_tests
            --gtest_filter=SchedulingSystemTest.PeriodicScheduling:SchedulingSystemTest.OneShotAndPeriodicScheduling
    )

    set_tests_properties(claw_scheduling_flaky PROPERTIES LABELS flaky)
endif()

target_compile_features(claw_scheduling PUBLIC cxx_std_23)