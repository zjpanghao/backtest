//  Copyright (c) 2015-2016 The BACK Authors. All rights reserved.
//  Created on: 2016年9月1日 Author: kerry

#ifndef BACK_KENSHO_RESULT_H__
#define BACK_KENSHO_RESULT_H__

#include "logic/back_infos.h"
#include <set>
#include "kensho_basic_infos.h"
#include "kensho_db.h"
#include "kensho_proto_buf.h"
#include "back_share/back_share_engine.h"
#include <map>

namespace kensho_logic {

typedef std::map<int64, kensho_logic::KenshoCalResult> SESSION_MAP;
typedef std::map<int64, SESSION_MAP> USER_MAP;
typedef std::map<const std::string, kensho_logic::KenshoStock> STOCK_MAP;
struct StockSearchInfo{
  std::string stock_code;
  std::string date_info;
};
class BackTestKenShoResult;
class KenshoResultManager;
class KenshoResultCache {
 public:
  USER_MAP user_map_;
  STOCK_MAP stock_map_;
};

class KenshoResultManager {
 public:
  KenshoResultManager();
  virtual ~KenshoResultManager();
 public:
  void Init(back_share::BSEngine* bs_engine);
  void InitDB(kensho_logic::KenshoDB* kensho_db);
 public:
  void UpdateKenshoResult(std::list<base_logic::DictionaryValue*>& list);
  void UpdateKenshoStock();
  bool OnGetResultBySession(const int64 uid, const int64 session,const int32 pos,
                            const int32 count,
                            kensho_logic::net_reply::BTResult* result);
  bool OnGetYield(const int64 uid, const int64 session,std::string& yield);
  bool ClearSession(int64 uid) {
    base_logic::WLockGd lk(lock_);
    result_cache_->user_map_.erase(uid);
    return true;
  }
 private:
  void Init();
  void SetKenshoResult(base_logic::DictionaryValue* info);
  void SetUserKenshoResult(const int64 uid, const int64 sessionid,
                           kensho_logic::KenshoCalResult& result);

  bool ParseStockStr(int search_type, const std::string origin,
                     std::string *symbol, std::string *info);

  void GetBackTestKenShoResult(const int64 uid, const int64 sessionid,
                               BackTestKenShoResult *kensho_result);
 private:
  struct threadrw_t* lock_;
  KenshoResultCache* result_cache_;
  back_share::BSEngine* bs_engine_;
  kensho_logic::KenshoDB*  kensho_db_;
};

class KenshoBacktestAction : public back_share::BacktestAction {
  public:
   KenshoBacktestAction() {}
   explicit KenshoBacktestAction(KenshoResultManager *manager) {
     manager_ = manager;
   }
   int OnMessage(const back_share::BacktestMessage &message) {
     LOG_MSG2("recv message %d  user_id %d", message.type, message.user_id);
     if (message.type == static_cast<int>(back_share::USER_LOGIN)) {
       manager_->ClearSession(message.user_id);
       LOG_MSG2("clear session for user_id %d", message.user_id);
     }
     return 0;
   }
  private:
   KenshoResultManager *manager_;
};

class KenshoResultEngine {
 private:
  static KenshoResultManager*  result_mgr_;
  static KenshoResultEngine*   result_engine_;
  KenshoResultEngine() {}
  virtual ~KenshoResultEngine() {}
 public:
  static KenshoResultManager* GetResultManager() {
    if (result_mgr_ == NULL)
      result_mgr_ = new KenshoResultManager();
    return result_mgr_;
  }

  static KenshoResultEngine* GetResultEngine() {
    if (result_engine_ == NULL)
      result_engine_ = new KenshoResultEngine();
    return result_engine_;
  }

  static void FreeResultManager(){
    delete result_mgr_;
    result_mgr_ = NULL;
  }

  static void FreeResultEngine(){
    delete result_engine_;
    result_engine_ = NULL;
  }
};
}

#endif
