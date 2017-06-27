//  Copyright (c) 2015-2016 The BACKTEST Authors. All rights reserved.
//  Created on: 2016年8月30日 Author: kerry

#include "logic/logic_unit.h"
#include <sstream>
#include "logic/logic_comm.h"
#include "net/packet_process.h"
#include "net/packet_process.h"

namespace back_logic {

void LogicUnit::SendFull(const int socket, const char *buf, int blen) {
  uint32_t len = blen;
  unsigned char *p_to_len = (unsigned char *)(&len);
  std::stringstream os; 
  os << p_to_len[0] << p_to_len[1] << p_to_len[2] << p_to_len[3] << buf;
  LOG_DEBUG2("length:%d %d %d %d", p_to_len[0], p_to_len[1], p_to_len[2], p_to_len[3]);
  std::string str = os.str();
  base_logic::LogicComm::SendFull(socket, str.c_str(), str.length());
}

void* SomeUtils::GetLibraryFunction(const std::string& library_name,
                                    const std::string& func_name) {
  void* engine = NULL;
  basic::libhandle handle_lancher = NULL;
  handle_lancher = basic::load_native_library(library_name.c_str());

  if (handle_lancher == NULL) {
    LOG_ERROR2("Cant't load path %s", library_name.c_str());
    return NULL;
  }

  engine = basic::get_function_pointer(handle_lancher, func_name.c_str());

  if (engine == NULL) {
    LOG_ERROR2("Can't find %s", func_name.c_str());
    return NULL;
  }
  return engine;
}
// 发送失败消息
void SendUtils::SendFailMessage(const int socket, const int error_code,
                                const std::string& error_str) {
  json_packet::PacketProcess failmsg_process;
  base_logic::DictionaryValue failmsg;
  failmsg.SetBigInteger("error_code", error_code);
  failmsg.SetString(L"error_reason", error_str);
  failmsg_process.PackPacket(socket, &failmsg);
}

void SendUtils::SendFailMessage(const int socket,
                                back_logic::PacketFail& msg) {
  json_packet::PacketProcess failmsg_process;
  failmsg_process.PackPacket(socket, msg.packet());
}

} /*namespace back_logic*/
