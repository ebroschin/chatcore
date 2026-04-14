add_library(claw_core
    src/application.cc
    src/system_context.cc
)

add_library(claw::core ALIAS claw_core)
set_target_properties(claw_core PROPERTIES EXPORT_NAME core)

target_include_directories(claw_core PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

option(BUILD_TESTING "Build tests" OFF)
if(BUILD_TESTING)
    enable_testing()
    find_package(GTest CONFIG REQUIRED)

    add_executable(tests
            tests/test_system_context.cpp
    )

    target_link_libraries(tests PRIVATE
            claw_core
            GTest::gtest
            GTest::gtest_main
    )

    include(GoogleTest)
    gtest_discover_tests(tests)
endif()

target_compile_features(claw_core PUBLIC cxx_std_23)