add_library(claw_chat_api INTERFACE
        include/claw/chat/api.h
)

add_library(claw::chat-api ALIAS claw_chat_api)
set_target_properties(claw_chat_api PROPERTIES EXPORT_NAME chat-api)

target_include_directories(claw_chat_api INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_compile_features(claw_chat_api INTERFACE cxx_std_23)