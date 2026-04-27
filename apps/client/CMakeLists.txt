add_executable(chatcore-client
    source/main.cpp
    source/application/chat_client_application.cpp
    source/application/chat_client_application.hpp
    source/application/application_system.cpp
    source/application/application_system.hpp
    source/ui/ui_system.cpp
    source/ui/ui_system.hpp
    source/commands/connect_command.cpp
    source/commands/connect_command.hpp
    source/commands/add_user_command.cpp
    source/commands/add_user_command.hpp
    source/session/session_system.cpp
    source/session/session_system.hpp
    source/commands/client_commands_system.hpp
    source/commands/login_command.cpp
    source/model/model_system.cpp
    source/model/model_system.hpp
    source/commands/join_channel_command.cpp
    source/commands/join_channel_command.hpp
    source/application/client_tcp_system.hpp
    source/application/client_rpc_system.hpp
    source/commands/get_channels_command.cpp
    source/commands/get_channels_command.hpp
    source/commands/add_channel_command.hpp
    source/commands/add_channel_command.cpp
    source/session/connection_event_handler.cpp
    source/session/connection_event_handler.hpp
    source/commands/logout_command.hpp
    source/commands/logout_command.cpp
    source/commands/exit_command.hpp
    source/commands/exit_command.cpp
    source/commands/help_command.cpp
    source/commands/help_command.hpp
    source/ui/ftxui_logger.cpp
    source/ui/ftxui_logger.hpp
)

if(WIN32)
    set(BOOST_STACKTRACE stacktrace_windbg)
else()
    set(BOOST_STACKTRACE stacktrace_backtrace)
endif()

find_package(Boost REQUIRED COMPONENTS ${BOOST_STACKTRACE})
find_package(ftxui CONFIG REQUIRED)

target_link_libraries(chatcore-client PRIVATE
    Boost::${BOOST_STACKTRACE}
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