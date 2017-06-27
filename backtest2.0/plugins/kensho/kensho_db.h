//  Copyright (c) 2015-2016 The BACKTEST Authors. All rights reserved.
//  Created on: 2016.8.30 Author: kerry

#ifndef BACK_KENSHO_DB_H_
#define BACK_KENSHO_DB_H_
#include "kensho_basic_infos.h"
#include "storage/data_engine.h"

namespace kensho_logic {

class KenshoDB {
 public:
  KenshoDB(config::FileConfig* config);
  virtual ~KenshoDB();
 public:
  bool FectchStockBullish(std::map<const std::string, kensho_logic::KenshoStock>& map);
  bool UpdateSentenceHot(int id, int hot);

 private:
  static void CallFecthStockBullish(void* param, base_logic::Value* value);
 private:
  base_logic::DataEngine* mysql_engine_;
};
}
#endif
