add_executable(chatcore-client
    source/main.cc
    source/application/chat_client_application.cc
    source/application/chat_client_application.h
    source/application/application_system.cc
    source/application/application_system.h
    source/ui/ui_system.cc
    source/ui/ui_system.h
    source/commands/commands_system.h
    source/commands/connect_command.cc
    source/commands/connect_command.h
    source/commands/add_user_command.cc
    source/commands/add_user_command.h
    source/session/session_system.cc
    source/session/session_system.h
    source/commands/client_commands_system.h
    source/commands/login_command.cc
    source/model/model_system.cc
    source/model/model_system.h
    source/commands/join_channel_command.cc
    source/commands/join_channel_command.h
    source/application/client_tcp_system.h
    source/application/client_rpc_system.h
    source/commands/get_channels_command.cc
    source/commands/get_channels_command.h
    source/commands/add_channel_command.h
    source/commands/add_channel_command.cc
    source/session/connection_event_handler.cc
    source/session/connection_event_handler.h
    source/commands/logout_command.h
    source/commands/logout_command.cc
    source/commands/exit_command.h
    source/commands/exit_command.cc
    source/commands/help_command.cc
    source/commands/help_command.h
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
    claw::core
    claw::utility
    claw::network
    claw::network-modules
    claw::chat-api
    claw::scheduling
    ebroschin::logging
    ebroschin::logging-modules
    ftxui::screen
    ftxui::dom
    ftxui::component
)