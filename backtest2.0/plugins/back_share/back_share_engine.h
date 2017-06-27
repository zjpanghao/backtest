//  Copyright (c) 2015-2016 The BACK Authors. All rights reserved.
//  Created on: 2016年8月30日 Author: kerry
#ifndef BACK_BACK_SHARE_ENGINE_H_
#define BACK_BACK_SHARE_ENGINE_H_

#include <map>
#include <list>
#include "back_share_db.h"
#include "thread/base_thread_handler.h"
#include "thread/base_thread_lock.h"
#include "config/config.h"
#include "logic/back_infos.h"

namespace back_share {

typedef std::map<std::string, back_logic::StockInfo> STOCK_MAP;
typedef std::list<back_logic::StockInfo> STOCK_LIST;
typedef std::map<int32, back_logic::BTSonditionSentence> SENTENCE_MAP;
typedef std::list<back_logic::BTSonditionSentence> SENTENCE_LIST;

enum UserMessage {
  USER_LOGIN = 1,
};
struct BacktestMessage {
  int type;
  int user_id;
};
class BacktestAction {
 public:
  virtual int OnMessage(const BacktestMessage &message);
};

class BSEngine {
 public:
  virtual void Init() = 0;

  virtual bool SetStock(const std::string& symbol,
                        back_logic::StockInfo* stock) = 0;

  virtual bool GetStock(const std::string& symbol,
                        back_logic::StockInfo* stock) = 0;

  virtual void GetBatchListStock(std::list<std::string>& symbol_list,
                       std::list<back_logic::StockInfo>& stocks) = 0;
  virtual void GetBatchMapStock(STOCK_MAP *map) = 0;

  virtual bool SetSentence(const int32 id,
                           back_logic::BTSonditionSentence* sentence) = 0;

  virtual void GetBatchListSentences(std::list<int32>& ids,
           std::list<back_logic::BTSonditionSentence>& sentences) = 0;

  virtual void GetBatchMapSentences(std::list<int32>& ids,
             std::map<const int32,back_logic::BTSonditionSentence>& sentences) = 0;

  virtual void SortSentenceByHot() = 0;

  virtual void UpdateParam(int sentence_id, int index, back_logic::SentenceParam param) = 0;

  virtual void SortParamByHot()= 0;

  virtual void GetAllSentencesList(std::list<back_logic::BTSonditionSentence>& sentences) = 0;
  
  virtual void GetAllRondomSentenceList(SENTENCE_LIST& sentences) = 0;
  virtual void UpdateShareData() = 0;
  virtual void UpdateShareLongPeriodData() = 0;
  virtual bool GetSentence(const int32 id,
                           back_logic::BTSonditionSentence* sentence) = 0;
  virtual void RegisterAction(int message_type, BacktestAction *action) = 0;
  virtual void OnMessage(const BacktestMessage &message) = 0;
};

class BSEngineImpl : public BSEngine {
 public:

  void Init();

  bool SetStock(const std::string& symbol, back_logic::StockInfo* stock);

  bool GetStock(const std::string& symbol, back_logic::StockInfo* stock);

  bool SetSentence(const int32 id,
                   back_logic::BTSonditionSentence* sentence);

  bool GetSentence(const int32 id,
                   back_logic::BTSonditionSentence* sentence);

  void GetBatchListStock(std::list<std::string>& symbol_list,
                         std::list<back_logic::StockInfo>& stocks);
  void GetBatchMapStock(STOCK_MAP *map);

  void GetBatchListSentences(std::list<int32>& ids,
         std::list<back_logic::BTSonditionSentence>& sentences);

  void GetBatchMapSentences(std::list<int32>& ids,
              std::map<const int32,back_logic::BTSonditionSentence>& sentences);

  void SortSentenceByHot();

  void SortParamByHot();

  virtual void GetAllSentencesList(std::list<back_logic::BTSonditionSentence>& sentences);
  
  virtual void GetAllRondomSentenceList(SENTENCE_LIST& sentences);

  virtual void UpdateShareData();

  virtual void UpdateShareLongPeriodData();
  virtual void RegisterAction(int message_type, BacktestAction *action);
  virtual void OnMessage(const BacktestMessage &message);

  void UpdateParam(int sentence_id, int index, back_logic::SentenceParam param);
};


class BackShareCache {
 public:
  STOCK_MAP secondary_stock_map_;
  STOCK_LIST secondary_stock_list_;
  SENTENCE_MAP sentence_map_;
  SENTENCE_LIST sentence_list_;
  SENTENCE_LIST random_sentence_list_;
};

__attribute__((visibility("default")))
class BackShareManager {
 public:
  BackShareManager();
  virtual ~BackShareManager();
 public:
  bool SetStock(const std::string& symbol, back_logic::StockInfo* stock);

  bool GetStock(const std::string& symbol, back_logic::StockInfo* stock);

  bool SetSentence(const int32 id, back_logic::BTSonditionSentence* sentence);

  bool GetSentence(const int32 id, back_logic::BTSonditionSentence* sentence);

  bool SetSentenceNoLock(const int32 id, back_logic::BTSonditionSentence* sentence);

  bool GetSentenceNoLock(const int32 id, back_logic::BTSonditionSentence* sentence);
  bool SetStockNoLock(const std::string& symbol,
                      back_logic::StockInfo* stock);
  void GetBatchListStock(std::list<std::string>& symbol_list,
                          std::list<back_logic::StockInfo>& stocks);
  void GetBatchMapStock(STOCK_MAP *map);

  void GetBatchListSentences(std::list<int32>& ids,
             std::list<back_logic::BTSonditionSentence>& sentences);

  void GetBatchMapSentences(std::list<int32>& ids,
                std::map<const int32,back_logic::BTSonditionSentence>& sentences);

  void SortSentenceByHot();

  void SortParamByHot();

  void GetAllSentencesList(std::list<back_logic::BTSonditionSentence>& sentences);
  void GetAllRondomSentenceList(SENTENCE_LIST& sentences);
  void UpdateShareData();

  void UpdateShareLongPeriodData();
  void RegisterAction(int message_type, BacktestAction *action) {
    std::map<int, std::list<BacktestAction*> >::iterator it = action_map_.find(message_type);
    if (it == action_map_.end()) {
      std::list<BacktestAction*> action_list;
      action_list.push_back(action);
      action_map_.insert(std::make_pair(message_type, action_list));
    } else {
      std::list<BacktestAction*> &action_list = it->second;
      action_list.push_back(action);
    }
  }

  int OnMessage(const BacktestMessage &message) {
    std::map<int, std::list<BacktestAction*> >::iterator mit = action_map_.find(message.type);
    if (mit == action_map_.end())
      return -1;
    std::list<BacktestAction*>::iterator it = mit->second.begin();
    while (it != mit->second.end()) {
      (*it)->OnMessage(message);
      it++;
    }
    return 0;
  }
  void SaveHotParams();

 private:
  void Init();

  void UpdateStock();
  void UpdateSentence();
  void UpdateHotParams();
  void ClearEvents(std::list<back_logic::BTSonditionSentence> *list);
  void ClearEvents(SENTENCE_MAP  *map);

 public:
  BackShareCache* GetFindCache() {return this->share_cache_;}
  void UpdateParam(int sentence_id, int index, back_logic::SentenceParam param);
 private:
  struct threadrw_t*    lock_;
  BackShareCache*       share_cache_;
  back_share::BackShareDB*  back_share_db_;
  std::map<int, std::list<BacktestAction*> > action_map_;
};

class BackShareEngine {
 private:
  static BackShareManager*      backshare_mgr_;
  static BackShareEngine*       backshare_engine_;

  BackShareEngine() {}
  virtual ~BackShareEngine() {}
 public:
  __attribute__((visibility("default")))
   static BackShareManager* GetBackShareManager() {
      if (backshare_mgr_ == NULL)
        backshare_mgr_ = new BackShareManager();
      return backshare_mgr_;
  }

  static BackShareEngine* GetBackShareEngine() {
      if (backshare_engine_ == NULL)
        backshare_engine_ = new BackShareEngine();

      return backshare_engine_;
  }
};
}


#ifdef __cplusplus
extern "C" {
#endif
back_share::BSEngine *GetBackShareEngine(void);
#ifdef __cplusplus
}
#endif

#endif
