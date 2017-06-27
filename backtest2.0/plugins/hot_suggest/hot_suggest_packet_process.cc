//
// Created by harvey on 2016/8/31.
//

#include "hot_suggest_packet_process.h"
#include "hot_suggest_factory.h"
#include "operator_code.h"

namespace hot_suggest_logic {
namespace http_packet {

void PacketProcess::PacketProcessCallBack(const int socket,
                                          base_logic::DictionaryValue* dict,
                                          back_logic::PacketHead* packet) {
  int16 opcode = packet->operate_code();
  hot_suggest_logic::HotSuggestFactory* factory =
      hot_suggest_logic::HotSuggestFactory::GetPtr();
  switch (opcode) {
    case ALL_SUGGEST_REQ: {
      factory->OnRequestAllSuggest(socket, dict, packet);
      break;
    }
    default: {
      break;
    }
  }
}

} /*namespace http_packet*/
} /*namespace hot_suggest_logic*/