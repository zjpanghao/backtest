//  Copyright (c) 2015-2016 The BACKSVC Authors. All rights reserved.
//  Created on: 2016年8月25日 Author: kerry
#include "back_infos.h"
#include "logic/logic_comm.h"
#include "basic/template.h"
#include <algorithm>
#include <set>

namespace back_logic {

StockInfo::StockInfo() {
  data_ = new Data();
}

StockInfo::StockInfo(const StockInfo& stock)
    : data_(stock.data_) {
  if (data_ != NULL) {
    data_->AddRef();
  }
}

StockInfo& StockInfo::operator =(const StockInfo& stock) {
  if (stock.data_ != NULL) {
    stock.data_->AddRef();
  }

  if (data_ != NULL) {
    data_->Release();
  }

  data_ = stock.data_;
  return (*this);
}

void StockInfo::ValueSerialization(base_logic::DictionaryValue* dict) {
  dict->GetString(L"symbol", &data_->symbol_);
  dict->GetString(L"sename", &data_->name_);
  dict->GetString(L"bord_name",&data_->bord_name_);
  dict->GetReal(L"outstanding", &data_->outstanding_);
}

BTSonditionSentence::BTSonditionSentence() {
  data_ = new Data();
}

BTSonditionSentence::BTSonditionSentence(const BTSonditionSentence& sentence)
    : data_(sentence.data_) {
  if (data_ != NULL) {
    data_->AddRef();
  }
}

BTSonditionSentence& BTSonditionSentence::operator =(
    const BTSonditionSentence& sentence) {
  if (sentence.data_ != NULL) {
    sentence.data_->AddRef();
  }

  if (data_ != NULL) {
    data_->Release();
  }

  data_ = sentence.data_;
  return (*this);
}

void BTSonditionSentence::set_param(const int32 index, const SentenceParam &param) {
  PARAM_MAP map;
  PARAM_LIST list;
  bool r = false;
  back_logic::BTSonditionParam sondition_param;
  base_logic::WLockGd lk(data_->lock_);
  r = base::MapGet<PARAMS_MAP, PARAMS_MAP::iterator, int32, PARAM_MAP>(
      data_->params_map_, index, map);
  base::MapGet<PARAMS_LIST, PARAMS_LIST::iterator, int32, PARAM_LIST>(
        data_->params_list_, index, list);
  if (!r) {  //不存在
    sondition_param.set_param(param);
    list.push_back(sondition_param);
    data_->params_list_[index] = list;
  } else {
    r = base::MapGet<PARAM_MAP, PARAM_MAP::iterator, ParamType,
        back_logic::BTSonditionParam>(map, param.value, sondition_param);
    if(!r){ //不存在
      sondition_param.set_param(param);
      list.push_back(sondition_param);
      data_->params_list_[index] = list;

    }
    sondition_param.hot_add();
  }
  map[param.value] = sondition_param;
  data_->params_map_[index] = map;

  r  = base::MapGet<PARAMS_LIST, PARAMS_LIST::iterator, int32, PARAM_LIST>(
      data_->params_list_, index, list);
  if(!r){
    list.push_back(sondition_param);
  }
  data_->params_list_[index] = list;
}

void BTSonditionSentence::sort_param(){
  base_logic::WLockGd lk(data_->lock_);
  // LOG_DEBUG2("==>id:%lld s:%s size:%d",data_->id_,
   //          data_->sentence_.c_str(),data_->params_list_.size());
  PARAMS_LIST::iterator it = data_->params_list_.begin();
  for(;it != data_->params_list_.end();it++){
    PARAM_LIST param = it->second;
    //排序
   //  LOG_DEBUG2("====>first: %d PARAM %d",it->first, param.size());
    param.sort(back_logic::BTSonditionParam::cmp);
    it->second = param;
    back_logic::BTSonditionParam p = param.front();
  }
}

void BTSonditionSentence::GetHottestParamsInfo(std::list<BTSonditionParam>& params_list) const {
  base_logic::RLockGd lk(data_->lock_);
  PARAMS_LIST::const_iterator params_iter(data_->params_list_.begin());
  for (; data_->params_list_.end() != params_iter; ++params_iter) {
    const PARAM_LIST& param_list = params_iter->second;
    params_list.push_back(param_list.front());
  }
  return;
}

void BTSonditionSentence::ValueSerialization(
    base_logic::DictionaryValue* dict) {
  int64 temp = 0;
  dict->GetBigInteger(L"id", &temp);
  data_->id_ = temp;
  dict->GetString(L"sentence", &data_->sentence_);

  temp = 0;
  dict->GetBigInteger(L"opcode", &temp);
  data_->opcode_ = temp;
  temp = 0;
  dict->GetBigInteger(L"type", &temp);
  data_->type_ = temp;

  dict->GetString(L"seword", &data_->seword_);
  dict->GetString(L"abstract", &data_->abstract_);
  dict->GetBigInteger(L"create_time", &data_->create_time_);
  dict->GetBigInteger(L"update", &data_->update_);
  int64 hot = 0;
  dict->GetBigInteger(L"hot", &hot);
  data_->hot_ = hot;
}

}
