//  Copyright (c) 2015-2015 The KID Authors. All rights reserved.
//  Created on: 2016.8.30 Author: kerry

#include "back_share_db.h"
#include "basic/scoped_ptr.h"
#include "logic/logic_comm.h"
#include <mysql/mysql.h>
#include <list>

namespace back_share {

BackShareDB::BackShareDB(config::FileConfig* config) {
  mysql_engine_ = base_logic::DataEngine::Create(MYSQL_TYPE);
  mysql_engine_->InitParam(config->mysql_db_list_);
}

BackShareDB::~BackShareDB() {
  if (mysql_engine_) {
    delete mysql_engine_;
    mysql_engine_ = NULL;
  }
}

bool BackShareDB::FetchBatchStcode(std::list<back_logic::StockInfo>* list) {
  bool r = false;
  scoped_ptr<base_logic::DictionaryValue> dict(
      new base_logic::DictionaryValue());

  std::string sql;
  sql = "call proc_GetStcodeBasic()";
  base_logic::ListValue* listvalue;
  dict->SetString(L"sql", sql);
  r = mysql_engine_->ReadData(0, (base_logic::Value*) (dict.get()),
                              CallFetchBatchStcode);
  if (!r)
    return false;
  dict->GetList(L"resultvalue", &listvalue);
  while (listvalue->GetSize()) {
    back_logic::StockInfo stock;
    base_logic::Value* result_value;
    listvalue->Remove(0, &result_value);
    base_logic::DictionaryValue* dict_result_value =
        (base_logic::DictionaryValue*) (result_value);
    stock.ValueSerialization(dict_result_value);
    list->push_back(stock);
    delete dict_result_value;
    dict_result_value = NULL;
  }

  return true;

}

bool BackShareDB::FetchBatchSentence(std::list<back_logic::BTSonditionSentence>* list) {
  bool r = false;
  scoped_ptr<base_logic::DictionaryValue> dict(
      new base_logic::DictionaryValue());

  std::string sql;
  sql = "call proc_GetSentence()";
  base_logic::ListValue* listvalue;
  dict->SetString(L"sql", sql);
  r = mysql_engine_->ReadData(0, (base_logic::Value*) (dict.get()),
                              CallFetchBatchSentence);
  if (!r)
    return false;
  dict->GetList(L"resultvalue", &listvalue);
  while (listvalue->GetSize()) {
    back_logic::BTSonditionSentence sentence;
    base_logic::Value* result_value;
    listvalue->Remove(0, &result_value);
    base_logic::DictionaryValue* dict_result_value =
        (base_logic::DictionaryValue*) (result_value);
    sentence.ValueSerialization(dict_result_value);
    list->push_back(sentence);
    delete dict_result_value;
    dict_result_value = NULL;
  }

  return true;
}

bool BackShareDB::FetchBatchSentenceParams(std::map<int, std::map<int, back_logic::SentenceParam> >* hot_params) {
  bool r = false;
  scoped_ptr<base_logic::DictionaryValue> dict(
      new base_logic::DictionaryValue());

  std::string sql;
  sql = "call proc_GetSentenceParam()";
  base_logic::ListValue* listvalue;
  dict->SetString(L"sql", sql);
  r = mysql_engine_->ReadData(0, (base_logic::Value*) (dict.get()),
                              CallFetchBatchSentenceParams);
  if (!r)
    return false;
  dict->GetList(L"resultvalue", &listvalue);
  int64  sentence_id = -1;
  int64  index;
  std::string  value;
  std::map<int, back_logic::SentenceParam> params; 
  while (listvalue->GetSize()) {
    base_logic::Value* result_value;
    listvalue->Remove(0, &result_value);
    base_logic::DictionaryValue* dict_result_value =
        (base_logic::DictionaryValue*) (result_value);
    int64 new_sentence_id;
    dict_result_value->GetBigInteger(L"sentence_id", &new_sentence_id);
    dict_result_value->GetBigInteger(L"index", &index);
    if (new_sentence_id != sentence_id) {
      if (sentence_id != -1)
        (*hot_params)[sentence_id] = params;
      params.clear();
      sentence_id = new_sentence_id;
    }
    dict_result_value->GetString(L"value", &value);
    back_logic::SentenceParam param;
    // in order to use  dic  getstring for  number
    if (value.length() > 1)                      
      param.value = value.substr(0, value.length() - 1);
    params[index] = param;
    delete dict_result_value;
    dict_result_value = NULL;
  }
  (*hot_params)[sentence_id] = params;
  return true;
}

bool BackShareDB::FetchBatchEvent(std::list<back_logic::BTSonditionSentence>* list) {
  bool r = false;
  scoped_ptr<base_logic::DictionaryValue> dict(
      new base_logic::DictionaryValue());

  std::string sql;
  sql = "call proc_GetEvent()";
  base_logic::ListValue* listvalue;
  dict->SetString(L"sql", sql);
  r = mysql_engine_->ReadData(0, (base_logic::Value*) (dict.get()),
                              CallFetchBatchEvent);
  if (!r)
    return false;
  dict->GetList(L"resultvalue", &listvalue);
  while (listvalue->GetSize()) {
    back_logic::BTSonditionSentence sentence;
    base_logic::Value* result_value;
    listvalue->Remove(0, &result_value);
    base_logic::DictionaryValue* dict_result_value =
        (base_logic::DictionaryValue*) (result_value);
    sentence.ValueSerialization(dict_result_value);
    list->push_back(sentence);
    delete dict_result_value;
    dict_result_value = NULL;
  }

  return true;
}

void BackShareDB::CallFetchBatchStcode(void* param, base_logic::Value* value) {
  base_logic::DictionaryValue* dict = (base_logic::DictionaryValue*) (value);
  base_logic::ListValue* list = new base_logic::ListValue();
  base_storage::DBStorageEngine* engine =
      (base_storage::DBStorageEngine*) (param);
  MYSQL_ROW rows;
  int32 num = engine->RecordCount();
  if (num > 0) {
    while (rows = (*(MYSQL_ROW*) (engine->FetchRows())->proc)) {
      base_logic::DictionaryValue* info_value =
          new base_logic::DictionaryValue();
      if (rows[0] != NULL)
        info_value->SetString(L"symbol", rows[0]);
      if (rows[1] != NULL)
        info_value->SetString(L"sename", rows[1]);
      if (rows[2] != NULL)
        info_value->SetString(L"bord_name", rows[2]);
      if (rows[3] != NULL)
        info_value->SetReal(L"outstanding", atof(rows[3]));

      list->Append((base_logic::Value*) (info_value));
    }
  }
  dict->Set(L"resultvalue", (base_logic::Value*) (list));
}

void BackShareDB::CallFetchBatchSentenceParams(void* param, base_logic::Value* value) {
  base_logic::DictionaryValue* dict = (base_logic::DictionaryValue*) (value);
  base_logic::ListValue* list = new base_logic::ListValue();
  base_storage::DBStorageEngine* engine =
      (base_storage::DBStorageEngine*) (param);
  MYSQL_ROW rows;
  int32 num = engine->RecordCount();
  if (num > 0) {
    while (rows = (*(MYSQL_ROW*) (engine->FetchRows())->proc)) {
      base_logic::DictionaryValue* info_value =
          new base_logic::DictionaryValue();
      if (rows[0] != NULL)
        info_value->SetBigInteger(L"sentence_id", atoll(rows[0]));
      if (rows[1] != NULL)
        info_value->SetBigInteger(L"index", atoll(rows[1]));
      if (rows[2] != NULL) {
        std::string param_value(rows[2]);
        param_value.append("s");
        info_value->SetString(L"value", param_value);
      }
      list->Append((base_logic::Value*) (info_value));
    }
  }
  dict->Set(L"resultvalue", (base_logic::Value*) (list));
}

void BackShareDB::CallFetchBatchSentence(void* param, base_logic::Value* value) {
  base_logic::DictionaryValue* dict = (base_logic::DictionaryValue*) (value);
  base_logic::ListValue* list = new base_logic::ListValue();
  base_storage::DBStorageEngine* engine =
      (base_storage::DBStorageEngine*) (param);
  MYSQL_ROW rows;
  int32 num = engine->RecordCount();
  if (num > 0) {
    while (rows = (*(MYSQL_ROW*) (engine->FetchRows())->proc)) {
      base_logic::DictionaryValue* info_value =
          new base_logic::DictionaryValue();
      if (rows[0] != NULL)
        info_value->SetBigInteger(L"id", atoll(rows[0]));
      if (rows[1] != NULL)
        info_value->SetString(L"sentence", rows[1]);
      if (rows[2] != NULL)
        info_value->SetBigInteger(L"opcode", atoll(rows[2]));
      if (rows[3] != NULL)
        info_value->SetBigInteger(L"type", atoll(rows[3]));
      if (rows[4] != NULL) {
        info_value->SetString(L"seword", rows[4]);
      }
      if (rows[5] != NULL) {
        info_value->SetBigInteger(L"hot", atoll(rows[5]));
      }
      list->Append((base_logic::Value*) (info_value));
    }
  }
  dict->Set(L"resultvalue", (base_logic::Value*) (list));
}

void BackShareDB::CallFetchBatchEvent(void* param, base_logic::Value* value) {
  base_logic::DictionaryValue* dict = (base_logic::DictionaryValue*) (value);
  base_logic::ListValue* list = new base_logic::ListValue();
  base_storage::DBStorageEngine* engine =
      (base_storage::DBStorageEngine*) (param);
  MYSQL_ROW rows;
  int32 num = engine->RecordCount();
  if (num > 0) {
    while (rows = (*(MYSQL_ROW*) (engine->FetchRows())->proc)) {
      base_logic::DictionaryValue* info_value =
          new base_logic::DictionaryValue();
      if (rows[0] != NULL)
        info_value->SetBigInteger(L"id", atoll(rows[0]));
      if (rows[1] != NULL)
        info_value->SetString(L"sentence", rows[1]);
      if (rows[2] != NULL)
        info_value->SetBigInteger(L"opcode", atoll(rows[2]));
      if (rows[3] != NULL)
        info_value->SetBigInteger(L"type", atoll(rows[3]));
      if (rows[4] != NULL) {
        info_value->SetString(L"seword", rows[4]);
      }
      if (rows[5] != NULL) {
        info_value->SetBigInteger(L"create_time", atoll(rows[5]));
      }
      if (rows[6] != NULL) {
        info_value->SetBigInteger(L"update", atoll(rows[6]));
      }
      if (rows[7] != NULL) {
        info_value->SetString(L"abstract", rows[7]);
      }
      
      list->Append((base_logic::Value*) (info_value));
    }
  }
  dict->Set(L"resultvalue", (base_logic::Value*) (list));
}

bool BackShareDB::SaveHotParams(int sentence_id, int index, const back_logic::SentenceParam &param) {
  char buf[1024];
  snprintf(buf, sizeof(buf), "insert into bt_sentence_param (sentenceId, paramIndex, paramValue) values(%d, %d, %s) on duplicate key update paramValue = %s", 
      sentence_id, index, param.value.c_str(), param.value.c_str());
  scoped_ptr<base_logic::DictionaryValue> dict(
      new base_logic::DictionaryValue());
  std::string sql = buf;
  LOG_MSG2("%s", sql.c_str());
  dict->SetString(L"sql", sql); 
  bool r = mysql_engine_->WriteData(0, (base_logic::Value*)(dict.get()));
  return r;
}

}
