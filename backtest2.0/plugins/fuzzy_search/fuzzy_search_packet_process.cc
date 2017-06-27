//  Copyright (c) 2015-2016 The BACK Authors. All rights reserved.
//  Created on: 2016年9月1日 Author: tangtao

#include "fuzzy_search_packet_process.h"
#include "logic/logic_comm.h"
#include "operator_code.h"
#include "fuzzy_search_factory.h"

namespace fuzzy_search_logic {

namespace http_packet {

PacketProcess::PacketProcess() {

}

void PacketProcess::PacketProcessCallBack(const int socket, base_logic::DictionaryValue* dict,
      back_logic::PacketHead* packet) {
  int16 operate_code = packet->operate_code();
  LOG_MSG2("GET %d", operate_code);
  fuzzy_search_logic::FuzzysearchFactory* factory =
      fuzzy_search_logic::FuzzysearchFactory::GetInstance();

  switch (operate_code){
    case KENSHO_BT_CONTITION_SENTENCE_CHECK_SEARCH: {
      factory->OnBTContitionSentenceCheckSearch(socket, dict, packet);
      break;
    }
    default:
      break;
  }
}

}

}
