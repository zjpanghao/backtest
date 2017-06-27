//
// Created by harvey on 2016/8/31.
//

#ifndef BACKSVC_HOT_SUGGEST_PACKET_PROCESS_H
#define BACKSVC_HOT_SUGGEST_PACKET_PROCESS_H

#include "net/packet_process.h"

namespace hot_suggest_logic {
namespace http_packet {

class PacketProcess {
 public:
  static void PacketProcessCallBack(const int socket, base_logic::DictionaryValue* dict,
                                    back_logic::PacketHead* packet);
};

} /*namespace http_packet*/
} /*namespace hot_suggest_logic*/

#endif //BACKSVC_HOT_SUGGEST_PACKET_PROCESS_H
