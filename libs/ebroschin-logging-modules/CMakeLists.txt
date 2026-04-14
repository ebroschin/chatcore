add_library(ebroschin_logging_modules
    include/ebroschin/logging-modules/spdlog/spdlog-logger.hpp
    src/spdlog/spdlog-logger.cpp
)

add_library(ebroschin::logging-modules ALIAS ebroschin_logging_modules)
set_target_properties(ebroschin_logging_modules PROPERTIES EXPORT_NAME logging-modules)

target_include_directories(ebroschin_logging_modules PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

find_package(spdlog CONFIG REQUIRED)

target_link_libraries(ebroschin_logging_modules PUBLIC
    ebroschin::logging
    spdlog::spdlog
)

target_compile_features(ebroschin_logging_modules PUBLIC cxx_std_23)