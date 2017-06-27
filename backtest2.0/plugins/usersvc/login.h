//  Copyright (c) 2015-2015 The restful Authors. All rights reserved.
//  Created on: 2015/11/24 Author: jiaoyongqing

#ifndef _HOME_TEST_WOOKONG_NORTHSEA_RESTFUL_PLUGINS_USERSVC_LOGIN_H_
#define _HOME_TEST_WOOKONG_NORTHSEA_RESTFUL_PLUGINS_USERSVC_LOGIN_H_

#include <string>

#include "usersvc/base.h"
#include "basic/basictypes.h"
#include "storage/storage.h"

namespace login {
class Login : public base::Base{
 public:
	Login() \
	  : platform_id_(-1), \
	  user_id_(-1), \
	  user_level_(-1), \
	  socket_(-1) {}

	void InitLoginByPwd(int platform_id, const std::string &user_name, const std::string &password, int socket, int64 code) {
	  platform_id_  = platform_id;
	  user_name_    = user_name;
	  password_     = password;
	  socket_       = socket;
	  code_         = code;
	}

	void InitLoginByToken(int64 user_id, const std::string &token, int socket, int64 code) {
	  user_id_      = user_id;
	  token_        = token;
	  socket_       = socket;
	  code_ = code;
	}

  virtual ~Login() {}
  //  对外接口
 public:
  bool GetData();
  void SendMessage();
  int user_id() const {
    return user_id_;
  }

 private:
  bool LoginByPwd();
  bool LoginByToken();

  bool DataInMySql(time_t start_time, time_t end_time);

  bool DataInRedis(time_t start_time, time_t end_time) { return true; }

  bool DataBetweenRedisAndMySql(time_t start_time, \
                                  time_t end_time, \
                               time_t middle_time) { return true; }

  void NumToChar(void *d, size_t l, std::string &token);
  std::string GetToken(int64 user_id, std::string &token);
  std::string GetToken(base_storage::DictionaryStorageEngine * const redis);

 private:
  int platform_id_;
  std::string user_name_;
  std::string password_;
  std::string platform_name_;
  int64 user_id_;
  std::string token_;
  int user_level_;
  int socket_;
  int64 code_;
};

}  // namespace login

#endif  //  _HOME_TEST_WOOKONG_NORTHSEA_RESTFUL_PLUGINS_USERSVC_LOGIN_H_
