//  Copyright (c) 2015-2016 The BACKTEST Authors. All rights reserved.
//  Created on: 2016.9.3 Author: kerry

#include "kensho_db.h"
#include "basic/scoped_ptr.h"
#include "config/config.h"
#include "basic/basic_util.h"
#include <mysql.h>
#include <list>

namespace kensho_logic {

KenshoDB::KenshoDB(config::FileConfig* config) {
  mysql_engine_ = base_logic::DataEngine::Create(MYSQL_TYPE);
  mysql_engine_->InitParam(config->mysql_db_list_);
}

KenshoDB::~KenshoDB() {
  if (mysql_engine_) {
    delete mysql_engine_;
    mysql_engine_ = NULL;
  }
}

bool KenshoDB::FectchStockBullish(
    std::map<const std::string, kensho_logic::KenshoStock>& map) {
  bool r = false;
  scoped_ptr<base_logic::DictionaryValue> dict(
      new base_logic::DictionaryValue());

  std::string sql;
  sql = "call proc_GetStockBullish()";
  base_logic::ListValue* listvalue;
  dict->SetString(L"sql", sql);
  r = mysql_engine_->ReadData(0, (base_logic::Value*) (dict.get()),
                              CallFecthStockBullish);
  if (!r)
    return false;
  dict->GetList(L"resultvalue", &listvalue);
  while (listvalue->GetSize()) {
    kensho_logic::KenshoStock stock;
    base_logic::Value* result_value;
    listvalue->Remove(0, &result_value);
    base_logic::DictionaryValue* dict_result_value =
        (base_logic::DictionaryValue*) (result_value);
    stock.ValueSerialization(dict_result_value);
    map[stock.symbol()] = stock;
    delete dict_result_value;
    dict_result_value = NULL;
  }
  //if (listvalue){delete listvalue; listvalue = NULL;}

  return true;
}

void KenshoDB::CallFecthStockBullish(void* param, base_logic::Value* value) {
  base_logic::DictionaryValue* dict = (base_logic::DictionaryValue*) (value);
  base_logic::ListValue* list = new base_logic::ListValue();
  base_storage::DBStorageEngine* engine =
      (base_storage::DBStorageEngine*) (param);
  MYSQL_ROW rows;
  int32 num = engine->RecordCount();
  if (num > 0) {
    while (rows = (*(MYSQL_ROW*) (engine->FetchRows())->proc)) {
      std::string str_bullish;
      double dl_bullish;
      base_logic::DictionaryValue* info_value =
          new base_logic::DictionaryValue();
      if (rows[0] != NULL)
        info_value->SetString(L"CODE", rows[0]);

      if (rows[1] != NULL) {
        str_bullish = rows[1];
        base::BasicUtil::StringUtil::StringToDouble(
            str_bullish, &dl_bullish);
        info_value->SetReal(L"changepercent", dl_bullish);
      }
      if (rows[2] != NULL) {
        str_bullish = rows[2];
        base::BasicUtil::StringUtil::StringToDouble(
            str_bullish, &dl_bullish);
        info_value->SetReal(L"trade", dl_bullish);
      }
      if (rows[3] != NULL) {
        str_bullish = rows[3];
        base::BasicUtil::StringUtil::StringToDouble(
            str_bullish, &dl_bullish);
        info_value->SetReal(L"volume", dl_bullish);
      }
      if (rows[4] != NULL) {
        str_bullish = rows[4];
        base::BasicUtil::StringUtil::StringToDouble(
            str_bullish, &dl_bullish);
        info_value->SetReal(L"amount", dl_bullish);
      }

      if (rows[5] != NULL) {
        str_bullish = rows[5];
        base::BasicUtil::StringUtil::StringToDouble(
            str_bullish,&dl_bullish);
        info_value->SetReal(L"bvps",dl_bullish);
      }

      if (rows[6] != NULL){
        str_bullish = rows[6];
        base::BasicUtil::StringUtil::StringToDouble(
            str_bullish,&dl_bullish);
        info_value->SetReal(L"totals", dl_bullish);
      }

      if (rows[7] != NULL) {
        str_bullish = rows[7];
        base::BasicUtil::StringUtil::StringToDouble(
            str_bullish,&dl_bullish);
        info_value->SetReal(L"pb", dl_bullish);
      }

      list->Append((base_logic::Value*) (info_value));
    }
  }
  dict->Set(L"resultvalue", (base_logic::Value*) (list));
}

bool KenshoDB::UpdateSentenceHot(int id, int hot) {
  char buf[128];
  snprintf(buf, sizeof(buf), "update bt_sentence set hot = %d where id = %d", 
      hot, id);
  scoped_ptr<base_logic::DictionaryValue> dict(
      new base_logic::DictionaryValue());
  std::string sql = buf;
  LOG_MSG2("%s", sql.c_str());
  dict->SetString(L"sql", sql); 
  bool r = mysql_engine_->WriteData(0, (base_logic::Value*)(dict.get()));
  return r;
}

}
