//  Copyright (c) 2015-2016 The BACK Authors. All rights reserved.
//  Created on: 2016年8月31日 Author: kerry
#include "kensho_sentence.h"
#include "basic/template.h"
#include "basic/radom_in.h"
#include "basic/basic_util.h"
#include "glog/logging.h"
#include "logic/logic_unit.h"
#include "logic/logic_comm.h"

namespace kensho_logic {

KenshoSentenceManager* KenshoSentenceEngine::sentence_mgr_ = NULL;
KenshoSentenceEngine* KenshoSentenceEngine::sentence_engine_ = NULL;

KenshoSentenceManager::KenshoSentenceManager() {
  Init();
}

void KenshoSentenceManager::Init() {

}

KenshoSentenceManager::~KenshoSentenceManager() {

}

void KenshoSentenceManager::Init(back_share::BSEngine* bs_engine) {
  bs_engine_ = bs_engine;
  pthread_mutex_init(&lock_, NULL);
}

void KenshoSentenceManager::GetBTSentence(
    const int32 id, back_logic::BTSonditionSentence& sentence) {

}

void KenshoSentenceManager::GetBatchBTSentence(std::list<int32>& ids) {
  std::list<back_logic::BTSonditionSentence> sentences;
  bs_engine_->GetBatchListSentences(ids, sentences);
}

base_logic::Value* KenshoSentenceManager::CreateBatchBTSentence(
    base_logic::ListValue* value, base_logic::DictionaryValue* dict_value) {
  std::list<int32> ids;
  std::map<const int32, back_logic::BTSonditionSentence> sentences;
  std::string condition;
  std::string param_symbol = "x";
  bool r = false;
  for (base_logic::ListValue::iterator it = value->begin(); it != value->end();
      it++) {
    base_logic::DictionaryValue* value = (base_logic::DictionaryValue*) (*it);
    int64 temp_id;
    if (value->GetType() == value->TYPE_DICTIONARY) {
      r = value->GetBigInteger(L"id", &temp_id);
      if (r) {
        int32 id = temp_id;
        ids.push_back(id);
      }
    }
  }
  if (ids.size() <= 0)
    return NULL;
  bs_engine_->GetBatchMapSentences(ids, sentences);
  int32 n = value->GetSize();
  int32 i = 0;
  //替换参数
  for (base_logic::ListValue::iterator it = value->begin(); it != value->end();
      it++, i++) {
    base_logic::DictionaryValue* value = (base_logic::DictionaryValue*) (*it);
    //int64 param;
    base_logic::ListValue* params = NULL;
    int64 temp_id;
    back_logic::BTSonditionSentence sentence;
    //r = value->GetBigInteger(L"param",&param);
    if (value->GetType() != value->TYPE_DICTIONARY)
      continue;
    r = value->GetList(L"params", &params);
    if (!r)
      continue;
    r = value->GetBigInteger(L"id", &temp_id);
    if (!r)
      continue;
    const int32 id = temp_id;
    //查找对应语句
    r = base::MapGet<std::map<const int32, back_logic::BTSonditionSentence>,
        std::map<const int32, back_logic::BTSonditionSentence>::iterator,
        const int32, back_logic::BTSonditionSentence>(sentences, temp_id,
                                                      sentence);
    if (!r)
      continue;
    std::string t_sentence = sentence.sentence();
    bool ret = false;
    int32 index = 0;
    //遍历替换函数
    while (params->GetSize() > 0) {
      base_logic::Value* result_value;
      ret = params->Remove(0, &result_value);
      if (!ret)
        continue;
      std::string param;
      ret = result_value->GetAsString(&param);
      if (!ret)
        continue;
      LOG_MSG2("The param is %s", param.c_str());
      if (param.length() >= 2)
        param = param.substr(0, param.length() - 1);
      else
        continue;
      LOG_MSG2("tsentence0 is %s", t_sentence.c_str());
      t_sentence = back_logic::SomeUtils::StringReplace(
          t_sentence, param_symbol,
          param);
      LOG_MSG2("tsentence is %s", t_sentence.c_str());
      if(t_sentence.find("大于") == -1 || t_sentence.find("小于") == -1) {
        back_logic::SentenceParam sentence_param;
        sentence_param.value = param;
        sentence.set_param(index, sentence_param);
        bs_engine_->UpdateParam(sentence.id(), index, sentence_param);
      }
      index++;
      if (result_value) {
        delete result_value;
        result_value = NULL;
      }
    }
    sentence.hot_add();  //add
    PrepareUpdateSentenceHot(sentence);

    condition += base::BasicUtil::StringUtil::Int64ToString(sentence.id())
        + ":" + t_sentence;

    if (params) {
      delete params;
      params = NULL;
    }

    if (i < (n - 1))
      condition += "+";
  }

  dict_value->SetString(L"condition", condition);
  dict_value->SetBigInteger(L"push_time", time(NULL));
  LOG(INFO) << condition;
  return NULL;
}

int KenshoSentenceManager::PopSentenceHotForUpdate(int *id, int *hot) {
  int rc = -1;
  pthread_mutex_lock(&lock_);
  std::map<int, int>::iterator it = sentence_hot_update_.begin();
  if (it != sentence_hot_update_.end()) {
    *id = it->first;
    *hot = it->second;
    rc = 0;
    sentence_hot_update_.erase(it);
  }
  pthread_mutex_unlock(&lock_);
  return rc;

}

}
