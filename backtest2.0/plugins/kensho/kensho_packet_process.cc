//  Copyright (c) 2015-2016 The BACK Authors. All rights reserved.
//  Created on: 2016年8月27日 Author: kerry
#include "kensho_packet_process.h"
#include "operator_code.h"
#include "kensho_factory.h"

namespace kensho_logic {

namespace http_packet {

PacketProcess::PacketProcess() {

}

void PacketProcess::PacketProcessCallBack(const int socket,base_logic::DictionaryValue* dict,
      back_logic::PacketHead* packet) {
  int16 operate_code = packet->operate_code();
  kensho_logic::KenshoFactory* factory =
      kensho_logic::KenshoFactory::GetInstance();

  switch (operate_code){

    case KENSHO_BT_SENTENCE_REQ: {
      factory->OnBTSentenceReq(socket,dict,packet);
      break;
    }

    case KENSHO_BT_RESULT_REQ:{
      factory->OnBTResultReq(socket, dict, packet);
      break;
    }

    case KENSHO_BT_YIELD_REQ:{
      factory->OnBTYield(socket, dict, packet);
      break;
    }
    default:
      break;
  }
}

}

}
