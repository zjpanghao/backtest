//  Copyright (c) 2015-2015 The restful Authors. All rights reserved.
//  Created on: 2015/11/24 Author: jiaoyongqing

#include "usersvc/user_logic.h"

#include <string>

#include "usersvc/user_comm_head.h"
#include "net/comm_head.h"
#include "logic/logic_comm.h"
#include "logic/logic_unit.h"
#include "logic/base_values.h"
#include "usersvc/get_platform_info.h"
#include "usersvc/login.h"
#include "usersvc/db_comm.h"
#include "tool/token.h"
#include "back_share/back_share_engine.h"

namespace usersvc_logic {
Userlogic* Userlogic::instance_ = NULL;

Userlogic::Userlogic() {
  if (!init()) {
    assert(0);
  }
}

Userlogic::~Userlogic() {
  token::Release();
  db::DbSql::Dest();
}

bool Userlogic::init() {
  bool r = false;
  std::string path = DEFAULT_CONFIG_PATH;
  config::FileConfig* config = config::FileConfig::GetFileConfig();
  if (config == NULL) {
    return false;
  }
  r = config->LoadConfig(path);

  LOG_DEBUG2("user  size: %d", config->redis_list_.size());
  db::DbSql::Init(&config->mysql_db_list_);
  token::Init(&config->redis_list_);
  bs_engine_ = GetBackShareEngine();
  bs_engine_->Init();

  return true;
}

bool Userlogic::OnUserConnect(struct server *srv, \
                                const int socket) {
  return true;
}

bool Userlogic::OnUserMessage(struct server *srv, \
                                const int socket, \
                                 const void *msg, \
                                   const int len) {
  const char* packet_stream = reinterpret_cast<const char*>(msg);
  std::string http_str(packet_stream, len);
	std::string error_str;
	
  int error_code = 0;
  scoped_ptr<base_logic::ValueSerializer> serializer(\
    base_logic::ValueSerializer::Create(base_logic::IMPL_HTTP, &http_str));

  NetBase* value = reinterpret_cast<NetBase*>\
    (serializer.get()->Deserialize(&error_code, &error_str));
  if (value == NULL) {
    int fail_errcode = back_logic::PACKET_ILLEGAL;  // 包不符合要求
    std::string fail_errstr = "packet not legal";
    back_logic::SendUtils::SendFailMessage(socket, fail_errcode, fail_errstr);
    return true;
  }

  scoped_ptr<RecvPacketBase> packet(\
    new RecvPacketBase(value));

  int32 type = packet->GetType();
  switch (type) {
    case back_logic::USER_LOGIN_TYPE:
      LOG_MSG2("onusermessage: %s", packet_stream);
      LOG_DEBUG2("message %s", "ddddd");
      OnLogin(srv, socket, value);
      break;

    case back_logic::GET_PLATFORM_TYPE:
      LOG_MSG2("onusermessage: %s", packet_stream);
      OnGetPlatformInfo(srv, socket, value);
      break;
    default:  //  end
      return false;
  }

  return true;
}

bool Userlogic::OnUserClose(struct server *srv, \
                              const int socket) {
  return true;
}



bool Userlogic::OnBroadcastConnect(struct server *srv, \
                                     const int socket, \
                                      const void *msg, \
                                        const int len) {
  return true;
}

bool Userlogic::OnBroadcastMessage(struct server *srv, \
                                     const int socket, \
                                      const void *msg, \
                                        const int len) {
  return true;
}

bool Userlogic::OnBroadcastClose(struct server *srv, \
                                   const int socket) {
  return true;
}

bool Userlogic::OnIniTimer(struct server *srv) {
    return true;
}



bool Userlogic::OnTimeout(struct server *srv, \
                                    char *id, \
                                  int opcode, \
                                    int time) {
  return false;
}

bool Userlogic::OnGetPlatformInfo(struct server *srv, \
                                    const int socket, \
                                    NetBase* netbase, \
                                     const void* msg, \
                                       const int len) {
  netcomm_recv::RecvGetPlatformInfo platform(netbase);
  std::string jsonp_call = platform.GetJsonp();
  int json_type = jsonp_call == "" ? 0 : 3;
  int error_code = platform.GetResult();
  if (error_code != 0) {
    int fail_errcode = error_code;
    std::string fail_errstr = "参数错误";
    back_logic::SendUtils::SendFailMessage(socket, fail_errcode, fail_errstr);
    return false;
  }

  get_platform_info::GetPlatFormInfo get_platform_info(jsonp_call, socket);
  get_platform_info.GetData();
  error_code = get_platform_info.get_error_info();
  if (error_code != 0) {
    int fail_errcode = error_code;
    std::string fail_errstr = "获取数据错误";
    back_logic::SendUtils::SendFailMessage(socket, fail_errcode, fail_errstr);
    return false;
  }
  get_platform_info.SendMessage();
  return true;
}


bool Userlogic::OnLogin(struct server *srv, \
                          const int socket, \
                          NetBase* netbase, \
                           const void* msg, \
                             const int len) {
  netcomm_recv::RecvLogin recv_login(netbase);
  int error_code = recv_login.GetResult();
  if (error_code != 0) {
    int fail_errcode = error_code; 
    std::string fail_errstr = "参数错误";
    back_logic::SendUtils::SendFailMessage(socket, fail_errcode, fail_errstr);
    return false;
  }

  int code = recv_login.code();
  login::Login login;
  if (code == -1) {
    int platform_id = recv_login.platform_id();
    std::string user_name = recv_login.user_name();
    std::string password  = recv_login.password();
    login.InitLoginByPwd(platform_id, user_name, password, socket, code);
  } else if (code == 1) {
  	int64 user_id = recv_login.user_id();
  	std::string token = recv_login.token();
  	login.InitLoginByToken(user_id, token, socket, code);
  }

  login.GetData();
  error_code = login.get_error_info();
  if (error_code != 0) {
    int fail_errcode = error_code; 
    std::string fail_errstr = "获取数据错误";
    back_logic::SendUtils::SendFailMessage(socket, fail_errcode, fail_errstr);
    return false;
  }
  if (code == -1) {
    back_share::BacktestMessage message;
    message.type = 1;
    message.user_id = login.user_id();
    LOG_MSG2("The user_id is %d", message.user_id);
    bs_engine_->OnMessage(message);
  }
  login.SendMessage();

  return true;
}

}  //  namespace usersvc_logic

