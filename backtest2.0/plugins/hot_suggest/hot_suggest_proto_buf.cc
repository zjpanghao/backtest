//
// Created by harvey on 2016/8/31.
//

#include "hot_suggest_proto_buf.h"
#include "logic/logic_comm.h"

namespace hot_suggest_logic {
namespace net_request {

void HotSuggestRequestMsg::set_http_packet(base_logic::DictionaryValue* value) {
  bool r = false;

  int64 uid = 0;
  r = value->GetBigInteger(L"uid", &uid);
  if (r) {
    set_uid(uid);
  }

  std::string token;
  r = value->GetString(L"token", &token);
  if (r) {
    set_token(token);
  }

  int64 flag = 0;
  r = value->GetBigInteger(L"flag", &flag);
  if (r) {
    LOG_DEBUG2("flag = %d", flag);
    set_flag(flag);
  }

  int64 pos = 0;
  r = value->GetBigInteger(L"pos", &pos);
  if (r) {
    set_pos(pos);
  }

  int64 count = 0;
  r = value->GetBigInteger(L"count", &count);
  if (r) {
    LOG_DEBUG2("count = %d", count);
    set_count(count);
  }

}

} /*namespace net_request*/

} /*namespace hot_suggest_logic*/