//  Copyright (c) 2015-2016 The BACKSVC Authors. All rights reserved.
//  Created on: 2016年9月1日 Author: tangtao

#include "fuzzy_search_factory.h"
#include "basic/basic_util.h"
#include "basic/scoped_ptr.h"
#include "fuzzy_search/operator_code.h"
#include "fuzzy_search_proto_buf.h"
#include "logic/logic_comm.h"

namespace fuzzy_search_logic {

FuzzysearchFactory* FuzzysearchFactory::instance_ = NULL;

FuzzysearchFactory*
FuzzysearchFactory::GetInstance() {
  if (instance_ == NULL)
    instance_ = new FuzzysearchFactory();
  return instance_;
}

void FuzzysearchFactory::FreeInstance() {
  delete instance_;
  instance_ = NULL;
}

FuzzysearchFactory::FuzzysearchFactory() {
  Init();
}

FuzzysearchFactory::~FuzzysearchFactory() {
  if (packet_json_) {
    delete packet_json_;
    packet_json_ = NULL;
  }
  if (packet_jsonp_) {
    delete packet_jsonp_;
    packet_jsonp_ = NULL;
  }
}

void FuzzysearchFactory::Init() {
  packet_json_ = new back_logic::json_packet::PacketProcess();
  packet_jsonp_ = new back_logic::jsonp_packet::PacketProcess();
  if (0 != InitThreadrw(&m_lock_)) {
      return;
  }
}

void FuzzysearchFactory::InitParam(config::FileConfig *config) {
}

void FuzzysearchFactory::Dest() {
}

bool FuzzysearchFactory::OnBTContitionSentenceCheckSearch(
    const int socket, base_logic::DictionaryValue *dict,
    back_logic::PacketHead *packet) {
  fuzzy_search_logic::net_request::BTSondition* sondition =
      new fuzzy_search_logic::net_request::BTSondition;
  sondition->set_http_packet(dict);
  std::string sondition_content = sondition->sonditions();
  std::string decode;
  base::BasicUtil::UrlDecode(sondition_content, decode);
  FuzzySearchHandle handle;
  handle.DoSearchWork(decode);
  const BackTestSearchResult &search_result = handle.get_search_result();
  net_reply::BTSondition reply_btsondition;

  reply_btsondition.set_search_result(search_result);
  base_logic::WLockGd wlock(m_lock_);
  SendPacket(socket, &reply_btsondition, sondition->attach_field(),
             KENSHO_BT_CONTITION_SENTENCE_CHECK_SEARCH_RESPONSE, back_logic::KENSHO_TYPE);
  return true;
}

void FuzzysearchFactory::SendPacket(const int socket,
                                    back_logic::PacketHead *packet,
                                    back_logic::AttachField *attach,
                                    const int16 operator_code,
                                    const int8 type) {
  packet->set_operator_code(operator_code);
  packet->set_type(type);
  if (attach != NULL && attach->format() == "jsonp") {
    packet->attach_field()->set_callback(attach->callback());
    packet_jsonp_->PackPacket(socket, packet->packet());
  } else {
    packet_json_->PackPacket(socket, packet->packet());
  }
}

void FuzzysearchFactory::SendHeader(const int socket,
                                    back_logic::AttachField *attach,
                                    const int16 operator_code,
                                    const int8 type) {
  back_logic::PacketHead *header = new back_logic::PacketHead();
  SendPacket(socket, header, attach, operator_code, type);
  if (header) {
    delete header;
    header = NULL;
  }
}
}
