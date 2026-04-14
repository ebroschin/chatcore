add_executable(server-load-tester
    source/main.cc
    source/application/client_tcp_system.h
    source/application/load_tester_application.cc
    source/application/load_tester_application.h
    source/application/application_system.cc
    source/application/application_system.h
    source/network/client.cc
    source/network/client.h
    source/network/root_client.cc
    source/network/root_client.h
    source/network/test_client.cc
    source/network/test_client.h
    source/network/client_report.h
    source/application/load_tester_arguments.cc
    source/application/load_tester_arguments.h
    source/application/client_rpc_system.h
)

target_link_libraries(server-load-tester PRIVATE
    Boost::${BOOST_STACKTRACE}
    claw::core
    claw::utility
    claw::network
    claw::network-modules
    claw::chat-api
    claw::scheduling
)