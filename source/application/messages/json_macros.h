#pragma once

#include "api.h"

namespace claw::chat::api {

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(api::ChatMessage, user_id, content);
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(api::WriteChatMessage, channel_id, message);
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(api::GetChatsRequestMessage, channel_id);
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(api::GetChatsResponseMessage, channel_id, messages);
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(api::CreateChannelMessage, name);

}