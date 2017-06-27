//  Copyright (c) 2015-2016 The BACKSVC Authors. All rights reserved.
//  Created on: 2016年8月25日 Author: kerry
#include "kensho/kensho_logic.h"
#include "glog/logging.h"
#include "kensho/kensho_config.h"
#include "signal.h"
#include "logic/logic_unit.h"
#include "basic/basictypes.h"
#include "baseconfig/base_config.h"
#include "config/config.h"
#include "basic/native_library.h"
#include "market_info/stock_price.h"
#include "tool/token.h"
#include "kensho_result.h"

#define DEFAULT_CONFIG_PATH  "./plugins/kenshosvc/kensho_config.xml"
#define KENSHO_CONFIG_PATH  "./plugins/kenshosvc/kensho.config"

namespace kensho_logic {

KenshoLogic* KenshoLogic::instance_ = NULL;

// 用于接收键盘ctrl c信号， 仅用作退出程序 
static void backtest_exit(int num) {
  if (num == SIGINT) {
    LOG_MSG("user exit");
    exit(1);
  }
}

KenshoLogic::KenshoLogic() {
  if (!Init())
    assert(0);
}

KenshoLogic::~KenshoLogic() {
  if (factory_){delete factory_; factory_ = NULL;}
  if (packet_) {delete packet_; packet_= NULL;}
  token::Release();
}

bool KenshoLogic::Init() {
  signal(SIGINT, backtest_exit);
  bool r = true;
  // redis初始化
  std::string path = DEFAULT_CONFIG_PATH;
  config::FileConfig* config = config::FileConfig::GetFileConfig();
  if (config == NULL)
    return false;
  r = config->LoadConfig(path);
  if (!r) {
    LOG_MSG("Load config fail!");
    return r;
  }
  plugin_pub::BaseConfig ini_config;
  if (!ini_config.LoadFile(KENSHO_CONFIG_PATH)) {    
    LOG_ERROR2("加载配置文件 [%s]失败", KENSHO_CONFIG_PATH);
    return false;
  }
  std::string yield_url = 
      ini_config.GetParam<std::string>("yield", "url");
  GetKenshoConfig().set_yield_url(yield_url);
  LOG(INFO) << "yield url :" <<  yield_url;
  token::Init(&config->redis_list_);
  LOG_MSG("redis init success");
  stock_price::SingleStockPrice::GetInstance()->Init(true);

  signal(SIGINT, backtest_exit);
  back_share::BSEngine* (*bs_engine)(void);
  packet_ = new back_logic::http_packet::PacketProcess();
  factory_ = kensho_logic::KenshoFactory::GetInstance();

  std::string back_share_library = "./back_share/back_share.so";
  std::string back_share_func = "GetBackShareEngine";

  bs_engine = (back_share::BSEngine* (*)(void))
  back_logic::SomeUtils::GetLibraryFunction(
  back_share_library, back_share_func);

  bs_engine_  = (*bs_engine)();
  if (bs_engine_ == NULL)
    assert(0);

  bs_engine_->Init();
  factory_->InitBS(bs_engine_);
  factory_->InitParam(config);
  back_share::BacktestAction *action = new KenshoBacktestAction(factory_->kensho_result());
  bs_engine_->RegisterAction(1, action);
  return r;
}

KenshoLogic*
KenshoLogic::GetInstance() {
  if (instance_ == NULL)
    instance_ = new KenshoLogic();
  return instance_;
}

void KenshoLogic::FreeInstance() {
  delete instance_;
  instance_ = NULL;
}

bool KenshoLogic::OnKenshoConnect(struct server *srv, const int socket) {
  return true;
}

bool KenshoLogic::OnKenshoMessage(struct server *srv, const int socket,
                                  const void *msg, const int len) {
  bool r = false;
  if (srv == NULL || socket < 0 || msg == NULL
              || len < 0)
    return false;
  r = packet_->UnpackPacket(socket, msg, len, back_logic::KENSHO_TYPE,
                            kensho_logic::http_packet::PacketProcess::PacketProcessCallBack);
  return true;
}

bool KenshoLogic::OnKenshoClose(struct server *srv, const int socket) {
  return true;
}

bool KenshoLogic::OnIniTimer(struct server *srv) {
  if (srv->add_time_task != NULL) {
    srv->add_time_task(srv, "kensho", TIME_KENSHO_RESULT, 1, -1);
    srv->add_time_task(srv, "kensho", TIME_KENSHO_RESULT1, 1, -1);
    srv->add_time_task(srv, "kensho", TIME_KENSHO_RESULT2, 1, -1);
    srv->add_time_task(srv, "kensho", TIME_KENSHO_RESULT3, 1, -1);
    srv->add_time_task(srv, "kensho", TIME_KENSHO_RESULT4, 1, -1);
    srv->add_time_task(srv, "kensho", TIME_KENSHO_RESULT5, 1, -1);
    srv->add_time_task(srv, "kensho", TIME_UPDATE_BACKSHARE,60,-1);
    srv->add_time_task(srv, "kensho", TIME_UPDATE_STOCK, 60, -1);
    srv->add_time_task(srv, "kensho", TIME_UPDATE_SENTENCE_HOT, 10, -1);
    srv->add_time_task(srv, "kensho", TIME_UPDATE_BACKSHARE_LONG_PERIOD, 60, -1);
    return true;
  }
  return true;
}

bool KenshoLogic::OnTimeout(struct server *srv, char *id, int opcode,
                            int time) {
  factory_->TimeEvent(opcode,time);
  return true;
}

}
