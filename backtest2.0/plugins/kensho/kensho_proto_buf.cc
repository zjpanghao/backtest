//  Copyright (c) 2015-2016 The BACK Authors. All rights reserved.
//  Created on: 2016年8月28日 Author: kerry

#include "kensho_proto_buf.h"
#include "basic/basic_util.h"

namespace kensho_logic {

namespace net_request {

void BTSondition::set_http_packet(base_logic::DictionaryValue* value) {
  bool r = false;
  int64 uid = 0;
  int64 pos = 0;
  int64 count = 0;
  int64 base_sessionid = 0;
  std::string token;
  std::string sonditions;
  std::string start_time;
  std::string end_time;
  r = value->GetBigInteger(L"uid", &uid);
  if (r)
    set_uid(uid);
  r = value->GetBigInteger(L"base_sessionid", &base_sessionid);
  if (r)
    set_base_sessionid(base_sessionid);
  r = value->GetString(L"token", &token);
  if (r)
    set_token(token);

  std::string url_sonditions;
  r = value->GetString(L"sonditions", &url_sonditions);
  //urlcode
  if (r) {
    base::BasicUtil::UrlDecode(url_sonditions, sonditions);
    set_sonditions(sonditions);
  }

  r = value->GetString(L"start_time", &start_time);
  if (r)
    set_start_time(start_time);

  r = value->GetString(L"end_time", &end_time);
  if (r)
    set_end_time(end_time);
}

void BTResult::set_http_packet(base_logic::DictionaryValue* value) {
  bool r = false;
  int64 uid = 0;
  std::string token;
  int64 session_id = 0;
  int64 pos = 0;
  int64 count = 0;
  r = value->GetBigInteger(L"uid", &uid);
  if (r)
    set_uid(uid);

  r = value->GetString(L"token", &token);
  if (r)
    set_token(token);

  r = value->GetBigInteger(L"sessionid", &session_id);
  if (r)
    set_session_id(session_id);

  r = value->GetBigInteger(L"pos", &pos);
  if (r)
    set_pos(pos);

  r = value->GetBigInteger(L"count", &count);
  if (r)
    set_count(count);

}


void BTKenshoYield::set_http_packet(base_logic::DictionaryValue* value) {
  bool r = false;
  int64 uid = 0;
  std::string token;
  int64 session_id = 0;
  int64 pos = 0;
  int64 count = 0;
  r = value->GetBigInteger(L"uid", &uid);
  if (r)
    set_uid(uid);

  r = value->GetString(L"token", &token);
  if (r)
    set_token(token);

  r = value->GetBigInteger(L"sessionid", &session_id);
  if (r)
    set_session_id(session_id);

}

}

}
