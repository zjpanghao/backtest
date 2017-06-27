//  Copyright (c) 2015-2016 The BACKTEST Authors. All rights reserved.
//  Created on: 2016.8.30 Author: kerry

#ifndef BACK_SHARE_DB_H_
#define BACK_SHARE_DB_H_
#include "logic/back_infos.h"
#include "storage/data_engine.h"

namespace back_share {

class BackShareDB {
 public:
  BackShareDB(config::FileConfig* config);
  virtual ~BackShareDB();

 public:
  bool FetchBatchStcode(std::list<back_logic::StockInfo>* list);

  bool FetchBatchSentence(std::list<back_logic::BTSonditionSentence>* list);

  bool FetchBatchSentenceParams(std::map<int, std::map<int, back_logic::SentenceParam> >* hot_params);

  bool FetchBatchEvent(std::list<back_logic::BTSonditionSentence>* list);
  
  bool SaveHotParams(int sentence_id, int index, const back_logic::SentenceParam &param);

 public:
  static void CallFetchBatchStcode(void* param, base_logic::Value* value);

  static void CallFetchBatchSentence(void* param, base_logic::Value* value);

  static void CallFetchBatchSentenceParams(void* param, base_logic::Value* value);

  static void CallFetchBatchEvent(void* param, base_logic::Value* value);

 private:
  base_logic::DataEngine* mysql_engine_;
};
}
#endif
