//  Copyright (c) 2015-2016 The back Authors. All rights reserved.
//  Created on: 2016年8月29日 Author: kerry

#ifndef BACK_NET_PACKET_PROCESS_H__
#define BACK_NET_PACKET_PROCESS_H__

#include "proto_buf.h"
#include "basic/basictypes.h"
#include "logic/base_values.h"
#include "tool/token.h"

namespace back_logic {

namespace http_packet {

class PacketProcess {
 public:
  PacketProcess();
  virtual ~PacketProcess();
 public:
  bool UnpackPacket(
      const int socket,
      const void *msg,
      const int len,
      const int8 type,
      void (*packet_process)(const int socket, base_logic::DictionaryValue*,
                             back_logic::PacketHead*));
  bool TokenCheck(base_logic::DictionaryValue* dict);
};

}

namespace json_packet {
class PacketProcess {
 public:
  PacketProcess();
  virtual ~PacketProcess();
 public:
  void PackPacket(const int socket, base_logic::DictionaryValue* value);
};
}

namespace jsonp_packet {
class PacketProcess {
 public:
  PacketProcess();
  virtual ~PacketProcess();
 public:
  void PackPacket(const int socket, base_logic::DictionaryValue* value);
};

}
}

#endif
