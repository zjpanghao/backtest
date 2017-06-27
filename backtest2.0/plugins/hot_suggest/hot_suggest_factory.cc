//
// Created by harvey on 2016/8/31.
//

#include "hot_suggest_factory.h"
#include "all_suggest_handler.h"

namespace hot_suggest_logic {

HotSuggestFactory::HotSuggestFactory() {
  packet_json_ = new back_logic::json_packet::PacketProcess();
  packet_jsonp_ = new back_logic::jsonp_packet::PacketProcess();
  InitThreadrw(&lock_);
}

HotSuggestFactory::~HotSuggestFactory() {
  if (packet_json_) {
    delete packet_json_;
    packet_json_ = NULL;
  }
  if (packet_jsonp_) {
    delete packet_jsonp_;
    packet_jsonp_ = NULL;
  }
  DeinitThreadrw(lock_);
}

HotSuggestFactory*
HotSuggestFactory::GetPtr() {
  return &GetRef();
}

HotSuggestFactory&
HotSuggestFactory::GetRef() {
  static HotSuggestFactory instance;
  return instance;
}

bool HotSuggestFactory::OnRequestAllSuggest(const int socket,
                                            base_logic::DictionaryValue* dict,
                                            back_logic::PacketHead* packet) {
  net_request::AllSuggestRequest request_msg;
  request_msg.set_http_packet(dict);
  net_reply::HotSuggestResponseMsg response_msg;

  bool r = false;
  AllSuggestHandler allSuggestHandler;
  r = allSuggestHandler.OnAllSuggestRequest(request_msg, response_msg);
  if (!r) {
    LOG_ERROR2("AllSuggestHandler::OnAllSuggestRequest Failed, RequestType = %d",
               request_msg.type());
    return false;
  }
  SendPacket(socket, &response_msg, response_msg.attach_field(),
             ALL_SUGGEST_RPL, back_logic::HOT_SUGGEST_TYPE);
  return true;
}

void HotSuggestFactory::SendPacket(const int socket,
                                   back_logic::PacketHead* packet,
                                   back_logic::AttachField* attach,
                                   const int16 operator_code,
                                   const int8 type) {
  base_logic::WLockGd lk(lock_);
  packet->set_operator_code(operator_code);
  packet->set_type(type);
  packet->set_timestamp(time(NULL));
  if (attach != NULL && attach->format() == "jsonp") {
    packet->attach_field()->set_callback(attach->callback());
    packet_jsonp_->PackPacket(socket, packet->packet());
  } else {
    packet_json_->PackPacket(socket, packet->packet());
  }
}

} /*namespace hot_suggest_logic*/