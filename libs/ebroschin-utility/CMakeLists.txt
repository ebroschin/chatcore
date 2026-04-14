add_library(claw_utility
        src/task_thread.cc
        include/claw/utility/variadic.h
        include/claw/utility/concepts.h
        include/claw/utility/registry.h
        include/claw/utility/resource.h
        include/claw/utility/signal.h
        src/signal_subscription.cc
        include/claw/utility/signal_subscription.h
        include/claw/utility/arguments.h
        src/arguments.cc
)

add_library(claw::utility ALIAS claw_utility)

set_target_properties(claw_utility PROPERTIES EXPORT_NAME utility)

target_include_directories(claw_utility PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_compile_features(claw_utility PUBLIC cxx_std_23)

option(BUILD_TESTING "Build tests" OFF)
if(BUILD_TESTING)
    enable_testing()
    find_package(GTest CONFIG REQUIRED)

    add_executable(tests
            tests/test_task_thread.cc
    )

    target_link_libraries(tests PRIVATE
            claw_utility
            GTest::gtest
            GTest::gtest_main
    )

    include(GoogleTest)
    gtest_discover_tests(tests)
endif()