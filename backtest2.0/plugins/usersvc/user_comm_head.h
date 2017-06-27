//  Copyright (c) 2015-2015 The restful Authors. All rights reserved.
//  Created on: 2015/11/24 Author: jiaoyongqing
/*
 *  user_comm_head.h
 *
 *  Created on: 2015/10/22
 *  Author: jiaoyongqing
 */

#ifndef _HOME_RUNNER_NORTHSEA_RESTFUL_PUB_NET_USER_COMM_HEAD_H_
#define _HOME_RUNNER_NORTHSEA_RESTFUL_PUB_NET_USER_COMM_HEAD_H_

#include <string>

#include "usersvc/http_data.h"
#include "basic/basictypes.h"
#include "logic/base_values.h"
#include "logic/logic_comm.h"
#include "basic/basic_util.h"

static const int USER_PASSWORD_LACK = -1107;
static const int PLATFORM_ID_LACK   = -1105;
static const int USER_NAME_LACK     = -1106;
static const int USER_ID_LACK       = -1104;
static const int TOKEN_LACK         = -1109;

namespace netcomm_recv {
class RecvGetPlatformInfo : public RecvPacketBase {
 public:
  explicit RecvGetPlatformInfo(NetBase* m)
    :RecvPacketBase(m) {
      error_code_ = 0;
    }
};

class RecvLogin : public RecvPacketBase {
 public:
  explicit RecvLogin(NetBase* m)
  :RecvPacketBase(m) {
    init();
  }

  inline void Reset() {
    init();
  }

  inline void init() {
    bool r = false;

    r = m_->GetBigInteger(L"code", static_cast<int64*>(&code_));
    if (false == r) {
      r = m_->GetBigInteger(L"platform_id", static_cast<int64*>(&platform_id_));
      if (false == r) error_code_ = PLATFORM_ID_LACK;

      r = m_->GetString(L"user_name", &user_name_);
      if (false == r) error_code_ = USER_NAME_LACK;

      r = m_->GetString(L"password", &password_);
      if (false == r) {
        error_code_ = USER_PASSWORD_LACK;
      }
      code_ = -1;
    } else {
      r = m_->GetBigInteger(L"user_id", static_cast<int64*>(&user_id_));
      if (false == r) {
        error_code_ = USER_ID_LACK;
      }

      r = m_->GetString(L"token", &token_);
      if (false == r) {
        error_code_ = TOKEN_LACK;
      }
    }
  }

  int64 platform_id() const { return platform_id_; }
  const std::string& user_name() const { return user_name_; }
  const std::string& password() const { return password_; }
  int64 user_id() const { return user_id_; }
  std::string token() const { return token_; }
  int64 code() const { return code_; }

 private:
  int64 code_;

  int64 platform_id_;
  std::string user_name_;
  std::string password_;

  int64 user_id_;
  std::string token_;
};

}  //  namespace netcomm_recv

namespace netcomm_send {
class SendLogin:public SendPacketBase {
 public:
  SendLogin() {
    login_.reset(new NetBase());
    userinfo_login_.reset(new NetBase());
  }

  virtual ~SendLogin() {}

  NetBase* release() {
    if (userinfo_login_->size() > 0) {
      login_->Set("user_info", userinfo_login_.release());
    }
    head_->Set("result", login_.release());
    this->set_flag(0);
    this->set_msg("");
    this->set_status(1);
    return head_.release();
  }

 public:
  inline void set_userinfo_uid(const int32 uid) {
    userinfo_login_->SetBigInteger("user_id", uid);
  }

  inline void set_userinfo_name(const std::string &name) {
    userinfo_login_->SetString("user_name", name);
  }

  inline void set_userinfo_platform(const std::string &platform) {
    userinfo_login_->SetString("platform_name", platform);
  }

  inline void set_userinfo_token(const std::string &token) {
    userinfo_login_->SetString("token", token);
  }

  inline void set_userinfo_level(int64 user_level) {
    userinfo_login_->SetBigInteger("user_level", user_level);
  }

 private:
  scoped_ptr<NetBase> login_;
  scoped_ptr<NetBase> userinfo_login_;
};

  //  SendPlatformInfo
class SendPlatformInfo:public SendPacketBase {
 public:
  SendPlatformInfo() {
    comment_.reset(new base_logic::ListValue());
  }

  virtual ~SendPlatformInfo() {}

  NetBase* release() {
    if (comment_.get() != NULL) {
      head_->Set("platform_info", comment_.release());
    }
    this->set_flag(0);
    this->set_msg("");
    this->set_status(1);
    return head_.release();
  }

  inline void set_comment(base_logic::DictionaryValue* value) {
    comment_->Append(value);
  }

 private:
  scoped_ptr<base_logic::ListValue> comment_;
};

}  //  namespace netcomm_send

#endif  //  _HOME_RUNNER_NORTHSEA_RESTFUL_PUB_NET_USER_COMM_HEAD_H_
