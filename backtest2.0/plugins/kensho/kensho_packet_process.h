//  Copyright (c) 2015-2016 The BACK Authors. All rights reserved.
//  Created on: 2016年8月28日 Author: kerry

#ifndef BACK_KENSHO_KENSHO_PACKET_PROCESS_H_
#define BACK_KENSHO_KENSHO_PACKET_PROCESS_H_
#include "net/packet_process.h"

namespace kensho_logic {

namespace http_packet {

class PacketProcess {
 public:
  PacketProcess();
  virtual ~PacketProcess(){}
 public:
  static void PacketProcessCallBack(const int socket, base_logic::DictionaryValue* dict,
                                    back_logic::PacketHead* packet);
};

}

}
#endif
