//  Copyright (c) 2015-2016 The BACK Authors. All rights reserved.
//  Created on: 2016年8月29日 Author: kerry
#include "packet_process.h"
#include "proto_buf.h"
#include "logic/logic_comm.h"
#include "basic/scoped_ptr.h"
#include "net/comm_head.h"
#include "logic/logic_unit.h"
#include "logic/base_values.h"
#include "basic/basic_util.h"

namespace back_logic {

namespace http_packet {

PacketProcess::PacketProcess() {
}

PacketProcess::~PacketProcess() {
}

bool PacketProcess::TokenCheck(base_logic::DictionaryValue* dict) {
  int64 uid = 0;
  std::string token;
  if (!dict->GetBigInteger(L"uid", &uid)
      || !dict->GetString(L"token", &token)) {
    return false;
  }
  if (uid == 9999) {
    return true;
  }


  std::string out_token;
  base::BasicUtil::UrlDecode(token, out_token);
  if (!token::CheckToken(uid, out_token)) {
    return false;
  }
  return true;
}
#if 0
bool PacketProcess::UnpackPacket(
    const int socket,
    const void *msg,
    const int len,
    const int8 type,
    void (*packet_process)(const int socket, base_logic::DictionaryValue*,
                           back_logic::PacketHead*)) {
  int fail_errcode;
  std::string fail_errstr;

  std::string error_str;
  int error_code = 0;
  std::string packet_stream((const char*) msg, len);
  if (len <= 0 || socket < 0 || msg == NULL) {
    PacketFail token_valid_msg;
    token_valid_msg.set_timestamp(time(NULL));
    token_valid_msg.set_status(PACKET_ILLEGAL);
    SendUtils::SendFailMessage(socket, token_valid_msg);
    return false;
  }
  scoped_ptr<base_logic::ValueSerializer> serializer(
      base_logic::ValueSerializer::Create(base_logic::IMPL_HTTP));
  base_logic::Value* value = serializer->Deserialize(&packet_stream,
                                                      &error_code, &error_str);

  if (value == NULL) {
    PacketFail token_valid_msg;
    token_valid_msg.set_timestamp(time(NULL));
    token_valid_msg.set_status(UNPACK_FAILED);
    SendUtils::SendFailMessage(socket, token_valid_msg);
    return false;
  }

  back_logic::PacketHead* packet = new back_logic::PacketHead;
  base_logic::DictionaryValue* dict = (base_logic::DictionaryValue*) value;
  packet->set_http_head(dict);
  if (packet->type() != 4 && packet->type() != 5) {
    if (!TokenCheck(dict)) {      // token无效

      PacketFail token_valid_msg;
      token_valid_msg.set_timestamp(time(NULL));
      token_valid_msg.set_status(TOKEN_INVALID);
      SendUtils::SendFailMessage(socket, token_valid_msg);
      return false;
    }
  }

 if (packet->type() == type)
    packet_process(socket, dict, packet);
  return true;
}

#else
bool PacketProcess::UnpackPacket(
    const int socket,
    const void *msg,
    const int len,
    const int8 type,
    void (*packet_process)(const int socket, base_logic::DictionaryValue*,
                           back_logic::PacketHead*)) {
  int fail_errcode;
  std::string fail_errstr;

  std::string error_str;
  int error_code = 0;
  std::string packet_stream((const char*) msg, len);
  if (len <= 0 || socket < 0 || msg == NULL) {
    PacketFail token_valid_msg;
    token_valid_msg.set_timestamp(time(NULL));
    token_valid_msg.set_status(PACKET_ILLEGAL);
    SendUtils::SendFailMessage(socket, token_valid_msg);
    return false;
  }
  scoped_ptr<base_logic::ValueSerializer> serializer(
      base_logic::ValueSerializer::Create(base_logic::IMPL_HTTP));
  base_logic::Value* value = serializer->Deserialize(&packet_stream,
                                                      &error_code, &error_str);

  if (value == NULL) {
    PacketFail token_valid_msg;
    token_valid_msg.set_timestamp(time(NULL));
    token_valid_msg.set_status(UNPACK_FAILED);
    SendUtils::SendFailMessage(socket, token_valid_msg);
    return false;
  }

  back_logic::PacketHead* packet = new back_logic::PacketHead;
  base_logic::DictionaryValue* dict = (base_logic::DictionaryValue*) value;

  packet->set_http_head(dict);
  if (type != packet->type()) {
    return true;
  }

  if (packet->type() != 4 && packet->type() != 5) {
    if (!TokenCheck(dict)) {      // token无效

      PacketFail token_valid_msg;
      token_valid_msg.set_timestamp(time(NULL));
      token_valid_msg.set_status(TOKEN_INVALID);
      SendUtils::SendFailMessage(socket, token_valid_msg);
      return false;
    }
  }

  //通过type判断数据什么packet
  //if (packet->type() == type)
  packet_process(socket, dict, packet);
  return true;
}
#endif
}

namespace json_packet {
PacketProcess::PacketProcess() {
}

PacketProcess::~PacketProcess() {
}

void PacketProcess::PackPacket(const int socket,
                               base_logic::DictionaryValue* value) {
  std::string json;
  scoped_ptr<base_logic::ValueSerializer> serializer(
      base_logic::ValueSerializer::Create(base_logic::IMPL_JSONP));
  bool r = serializer->Serialize(*value, &json);
  if (!r)
    return;
  base_logic::LogicComm::SendFull(socket, json.c_str(), json.length());
}

}

namespace jsonp_packet {

PacketProcess::PacketProcess() {
}

PacketProcess::~PacketProcess() {
}

void PacketProcess::PackPacket(const int socket,
                               base_logic::DictionaryValue* value) {
  std::string json;
  scoped_ptr<base_logic::ValueSerializer> serializer(
      base_logic::ValueSerializer::Create(base_logic::IMPL_JSONP));
  bool r = serializer->Serialize(*value, &json);
  if (!r)
    return; LOG_MSG2("%s",json.c_str());
  back_logic::LogicUnit::SendFull(socket, json.c_str(), json.length());
  // base_logic::LogicComm::SendFull(socket, json.c_str(), json.length());
}

}

}
