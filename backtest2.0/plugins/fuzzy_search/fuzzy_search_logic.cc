//  Copyright (c) 2015-2016 The BACKSVC Authors. All rights reserved.
//  Created on: 2016年9月1日 Author: tangtao

#include "fuzzy_search/fuzzy_search_logic.h"
#include "logic/logic_comm.h"
#include "logic/logic_unit.h"
#include "basic/basictypes.h"
#include "config/config.h"
#include "basic/native_library.h"
#include "back_share/back_share_engine.h"
#include "fuzzy_search/test/fuzzy_search_test.h"
#include "tool/token.h"

#define DEFAULT_CONFIG_PATH  "./plugins/fuzzy_search/fuzzy_search_config.xml"

namespace fuzzy_search_logic {

FuzzysearchLogic *FuzzysearchLogic::instance_ = NULL;

FuzzysearchLogic::FuzzysearchLogic() {
  if (!Init())
    assert(0);
}

FuzzysearchLogic::~FuzzysearchLogic() {
  if (factory_) {
    delete factory_;
    factory_ = NULL;
  }
  if (packet_) {
    delete packet_;
    packet_ = NULL;
  }
  token::Release();
}

bool FuzzysearchLogic::Init() {
  bool r = true;

  // redis初始化
  std::string path = DEFAULT_CONFIG_PATH;
  config::FileConfig* config = config::FileConfig::GetFileConfig();
  if (config == NULL) {
    return false;
  }
  r = config->LoadConfig(path);
  if (false == r) {
    LOG_MSG("FuzzysearchLogic::Load config fail!");
    return false;
  }
  token::Init(&config->redis_list_);
  LOG_MSG("redis init success");

  back_share::BSEngine *(*bs_engine)(void);
  packet_ = new back_logic::http_packet::PacketProcess();
  factory_ = fuzzy_search_logic::FuzzysearchFactory::GetInstance();

  GetBackShareEngine()->Init();
  // fuzzy_search_test();
  return r;
}

FuzzysearchLogic* FuzzysearchLogic::GetInstance() {
  if (instance_ == NULL)
    instance_ = new FuzzysearchLogic();
  return instance_;
}

void FuzzysearchLogic::FreeInstance() {
  delete instance_;
  instance_ = NULL;
}

bool FuzzysearchLogic::OnFuzzysearchConnect(struct server *srv,
                                            const int socket) {
  return true;
}

bool FuzzysearchLogic::OnFuzzysearchMessage(struct server *srv,
                                            const int socket, const void *msg,
                                            const int len) {
  bool r = false;
  if (srv == NULL || socket < 0 || msg == NULL || len < 0)
    return false;

  r = packet_->UnpackPacket(
      socket, msg, len, back_logic::SEARCH_TYPE,
      fuzzy_search_logic::http_packet::PacketProcess::PacketProcessCallBack);
  return true;
}

bool FuzzysearchLogic::OnFuzzysearchClose(struct server *srv,
                                          const int socket) {
  return true;
}

bool FuzzysearchLogic::OnIniTimer(struct server *srv) {
  if (srv->add_time_task != NULL) {
    //srv->add_time_task(srv, "vip", TIME_UPDATE_ACRTICLE, 100, -1);
    return true;
  }
  return true;
}

bool FuzzysearchLogic::OnTimeout(struct server *srv, char *id, int opcode,
                                 int time) {
  return true;
}

}   //namespace fuzzy_search_logic
