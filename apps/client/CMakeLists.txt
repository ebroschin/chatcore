file(GLOB_RECURSE SRC CONFIGURE_DEPENDS
        source/*.cpp
        source/*.hpp
)

add_executable(chatcore-client
        ${SRC}
)

find_package(ftxui CONFIG REQUIRED)

target_link_libraries(chatcore-client PRIVATE
    ebroschin::core
    ebroschin::utility
    ebroschin::network
    ebroschin::network-modules
    ebroschin::chatcore-api
    ebroschin::scheduling
    ebroschin::logging
    ebroschin::logging-modules
    ebroschin::commands
    ftxui::screen
    ftxui::dom
    ftxui::component
)