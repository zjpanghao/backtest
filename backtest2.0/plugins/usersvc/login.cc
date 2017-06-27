//  Copyright (c) 2015-2015 The restful Authors. All rights reserved.
//  Created on: 2015/11/24 Author: jiaoyongqing

#include "usersvc/login.h"

#include <string>
#include <sstream>

#include "usersvc/login_logic_unit.h"
#include "usersvc/db_comm.h"
#include "usersvc/user_comm_head.h"
#include "tea/tea.h"
#include "tool/token.h"
#include "usersvc/redis_link.h"

static const int USER_PASSWORD_ERROR    = -1108;

namespace login {
bool Login::GetData() {
  Login::DataInMySql(0, 0);
  return true;
}

void Login::NumToChar(void *d, size_t l, std::string &token) {
  std::stringstream os;
  char *p = reinterpret_cast<char *>(d);
  int temp;
  for (int i = 0; i < l; ++i) {
    temp = p[i];
    os << temp << ",";
  }
  token = os.str();
}

std::string Login::GetToken(int64 user_id, std::string &token) {
  std::stringstream os;
  std::string cur_token;
  std::string temp;
  os.str("");
  os << user_id;
  os << ",";
  os << time(NULL);
  cur_token = os.str();

  LOG_DEBUG2("\n\norigin token: %s\n\n", cur_token.c_str());

  int len = ((cur_token.length() - 1) / 8 + 1) * 8;
  char *in = reinterpret_cast<char*>(malloc(len));
  memset(in, 0, len);
  strncpy(in, cur_token.c_str(), cur_token.length());

  struct tea_data td;
  td.d = reinterpret_cast<void *>(in);
  td.l = len;

  StrEn(&td);
  NumToChar(td.d, td.l, token);
  free(in);
  return token;
}

std::string Login::GetToken(base_storage::DictionaryStorageEngine *const redis) {
  std::string token_saved = "0";

  if (NULL == redis) {
    LOG_MSG2("User redis null %x", redis);
	return token_saved;
  }
  std::string table_name = "user_token";
  std::string key_name = "0";
  std::stringstream ss;
  ss << user_id_;
  ss >> key_name;
  ss.str("");
  ss.clear();

  char * val = NULL;
  size_t val_len = 0;
  redis->GetHashElement(table_name.c_str(), key_name.c_str(), \
	  key_name.length(), &val, &val_len);
  if (NULL != val) {
	token_saved = val;
	free(val);
  }
  return token_saved;
}

bool Login::LoginByPwd() {
  db::DbSql sql;
  sql.GetUserInfo(platform_id_, user_name_);
  error_info_ = sql.get_error_info();
  if (0 != error_info_) {
	return false;
  }

  MYSQL_ROW rows = sql.NextRecord();
  if (password_ != rows[2]) {
	error_info_ = USER_PASSWORD_ERROR;
	return false;
  }

  user_id_ = atoll(rows[0]);
  user_level_ = atoll(rows[3]);

  platform_name_ = sql.GetPlatformName(platform_id_);
  error_info_ = sql.get_error_info();
  if (0 != error_info_) {
	return false;
  }

  return true;
}

bool Login::LoginByToken() {
  AutoEngine auto_engine;
  base_storage::DictionaryStorageEngine *const redis_engine = auto_engine.GetRedisEngine();
  if (!redis_engine) {
    LOG_MSG2("%x", "Get token error");
    return false;
  }

  std::string token_saved = GetToken(redis_engine);
  if (token_ != token_saved) {
    LOG_DEBUG2("token被替换,token_saved:%s, cur token:%s", token_saved.c_str(), token_.c_str());
    return false;
  }

  if (!token::CheckToken(user_id_, token_)) {
      LOG_DEBUG("token失效或者token格式不合法");
      return false;
  }

  return true;
}

bool Login::DataInMySql(time_t start_time, time_t end_time) {
  if (code_ == -1) {
    return LoginByPwd();
  } else {
    return LoginByToken();
  }
  return true;
}

void Login::SendMessage() {
  std::string token="";
  GetToken(user_id_,token);

  token::SaveUserToken(user_id_, token);

  netcomm_send::SendLogin qlogin;
  qlogin.set_userinfo_token(token);
  qlogin.set_userinfo_uid(user_id_);
  if (code_ == -1) {
    qlogin.set_userinfo_name(user_name_);
    qlogin.set_userinfo_platform(platform_name_);
    qlogin.set_userinfo_level(user_level_);
  }
  send_message(socket_, &qlogin);
}

}  //  namespace login
