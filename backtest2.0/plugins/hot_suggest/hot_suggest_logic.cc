//
// Created by harvey on 2016/8/31.
//

#include "hot_suggest/hot_suggest_logic.h"

#include "logic/logic_unit.h"
#include "logic/logic_comm.h"
#include "basic/basictypes.h"
#include "config/config.h"
#include "basic/native_library.h"
#include "hot_suggest_packet_process.h"
#include "hot_suggest/hot_suggest_engine.h"
#include "tool/token.h"

#define DEFAULT_CONFIG_PATH   "./plugins/hot_suggest/hot_suggest_config.xml"

namespace hot_suggest_logic {

HotSuggestLogic::HotSuggestLogic() {
  if (!Initialize()) {
    assert(0);
  }
}

HotSuggestLogic::~HotSuggestLogic() {
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

HotSuggestLogic*
HotSuggestLogic::GetPtr() {
  return &GetRef();
}

HotSuggestLogic&
HotSuggestLogic::GetRef() {
  static HotSuggestLogic instance;
  return instance;
}

bool HotSuggestLogic::Initialize() {
  bool r = true;
  // redis³õÊ¼»¯
  std::string path = DEFAULT_CONFIG_PATH;
  config::FileConfig* config = config::FileConfig::GetFileConfig();
  if (config == NULL) {
    return false;
  }
  r = config->LoadConfig(path);
  if (!r) {
    LOG_MSG("Load config fail!");
    return r;
  }
  token::Init(&config->redis_list_);
  LOG_MSG("redis init success");

  factory_ = HotSuggestFactory::GetPtr();
  packet_ = new back_logic::http_packet::PacketProcess();
  back_share::BSEngine* (* bs_engine)(void);
  std::string back_share_library = "./back_share/back_share.so";
  std::string back_share_func = "GetBackShareEngine";
  bs_engine = (back_share::BSEngine* (*)(void))
      back_logic::SomeUtils::GetLibraryFunction(
          back_share_library, back_share_func);
  bs_engine_ = (*bs_engine)();
  if (bs_engine_ == NULL)
    assert(0);
  bs_engine_->Init();
  hot_suggest_mgr_ = HotSuggestEngine::GetHotSuggestManager();
  hot_suggest_mgr_->Init(bs_engine_);
  return r;
}

bool HotSuggestLogic::OnHotSuggestConnect(struct server* srv, const int socket) {

  return true;
}

bool HotSuggestLogic::OnHotSuggestClose(struct server* srv, const int socket) {

  return true;
}

bool HotSuggestLogic::OnHotSuggestMessage(struct server* srv, const int socket,
                                          const void* msg, const int len) {
  if (NULL == srv || socket < 0 || NULL == msg || len < 0) {
    LOG_DEBUG2("HotSuggestLogic::OnHotSuggestMessage Msg Valid, msg = %s", msg);
    return false;
  }
  return packet_->UnpackPacket(socket, msg, len, back_logic::HOT_SUGGEST_TYPE,
                               http_packet::PacketProcess::PacketProcessCallBack);
}

bool HotSuggestLogic::OnBroadcastConnect(struct server* srv, const int socket,
                                         const void* data, const int len) {
  return true;
}

bool HotSuggestLogic::OnBroadcastClose(struct server* srv, const int socket) {
  return true;
}

bool HotSuggestLogic::OnBroadcastMessage(struct server* srv, const int socket,
                                         const void* msg, const int len) {
  return true;
}

bool HotSuggestLogic::OnIniTimer(struct server* srv) {
  if (NULL != srv) {
    srv->add_time_task(srv, "hotsuggest", UPDATE_SHARE_DATA_TIMER, 60, -1);
  }
  return true;
}

bool HotSuggestLogic::OnTimeout(struct server* srv, char* id, int opcode, int time) {
  switch (opcode) {
    case UPDATE_SHARE_DATA_TIMER:{
      hot_suggest_mgr_->OnTimeUpdateShareData();
      break;
    }
    default: {
      break;
    }
  }
  return true;
}

} /*namespace hot_suggest_logic*/
