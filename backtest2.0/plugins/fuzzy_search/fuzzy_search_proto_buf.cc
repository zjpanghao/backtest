//  Copyright (c) 2015-2016 The BACK Authors. All rights reserved.
//  Created on: 2016年9月1日 Author: tangtao

#include "fuzzy_search_proto_buf.h"

namespace fuzzy_search_logic {

namespace net_request {

void BTSondition::set_http_packet(base_logic::DictionaryValue *value) {
  bool r = false;
  int64 uid = 0;
  std::string token;
  std::string sonditions;
  std::string start_time;
  std::string end_time;
  r = value->GetBigInteger(L"uid", &uid);
  if (r)
    set_uid(uid);

  r = value->GetString(L"token", &token);
  if (r)
    set_token(token);

  r = value->GetString(L"sonditions", &sonditions);
  if (r)
    set_sonditions(sonditions);

  r = value->GetString(L"start_time",&start_time);
  if (r)
    set_start_time(start_time);

  r = value->GetString(L"end_time", &end_time);
  if (r)
    set_end_time(end_time);
}

}   //namespace net_request

}   //namespace fuzzy_search_logic
