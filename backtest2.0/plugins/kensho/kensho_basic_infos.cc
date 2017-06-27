//  Copyright (c) 2015-2016 The BACKSVC Authors. All rights reserved.
//  Created on: 2016年9月2日 Author: kerry

#include "kensho_basic_infos.h"
#include "glog/logging.h"
#include "http/http_api.h"
#include "basic/template.h"
#include "kensho/kensho_config.h"

namespace kensho_logic {

typedef std::list<KenshoStock> KENSHO_LIST;
typedef std::map<int64, KENSHO_LIST> KENSHO_MAP;

KenshoStock::KenshoStock() {
  data_ = new Data();
}

KenshoStock::KenshoStock(const KenshoStock& stock)
    : data_(stock.data_) {
  if (data_ != NULL) {
    data_->AddRef();
  }
}

KenshoStock& KenshoStock::operator =(const KenshoStock& stock) {
  if (stock.data_ != NULL) {
    stock.data_->AddRef();
  }

  if (data_ != NULL) {
    data_->Release();
  }
  data_ = stock.data_;
  return (*this);
}

void KenshoStock::ValueSerialization(base_logic::DictionaryValue* dict) {
  dict->GetString(L"CODE", &data_->symbol_);
  dict->GetReal(L"changepercent", &data_->changepercent_);
  dict->GetReal(L"trade", &data_->trade_);
  dict->GetReal(L"volume", &data_->volume_);
  dict->GetReal(L"amount", &data_->amount_);
  dict->GetReal(L"bvps", &data_->bvps_);
  dict->GetReal(L"pb", &data_->pb_);
  dict->GetReal(L"totals", &data_->totals_);
}

KenshoCalResult::KenshoCalResult() {
  data_ = new Data();
}

KenshoCalResult::KenshoCalResult(const KenshoCalResult& result)
    : data_(result.data_) {
  if (data_ != NULL) {
    data_->AddRef();
  }
}

KenshoCalResult& KenshoCalResult::operator =(const KenshoCalResult& result) {
  if (result.data_ != NULL) {
    result.data_->AddRef();
  }

  if (data_ != NULL) {
    data_->Release();
  }
  data_ = result.data_;
  return (*this);
}

void KenshoCalResult::create_yield() {
  // std::string url =
  //    "http://61.147.114.67/cgi-bin/wookong_yields/wookong_yields_runner.fcgi";
  std::string url = kensho_logic::GetKenshoConfig().yield_url(); 
  //计算总值
  std::string stock_info;
  std::string start_date = "," + data_->start_time_;
  std::string end_date = "," + data_->end_time_;
  if (data_->stock_list_.size() <=0 )
    return;
  std::list<KenshoStock>::iterator it = data_->stock_list_.begin();
  double allstock_value = 0.0 + 0.000001;
  for (; it != data_->stock_list_.end(); it++) {
    allstock_value += (*it).outstanding();
  }
  
  it = data_->stock_list_.begin();
  int32 index = data_->stock_list_.size();
  for (; it != data_->stock_list_.end(); it++) {
    double ratio = (*it).outstanding() / allstock_value;
    char buf[32];
    sprintf(buf, "%.6f", ratio);
    stock_info += (*it).symbol();
    stock_info += "," + std::string(buf);
    index--;
    if (index > 0)
      stock_info += ",";
  }
  std::string result;
  base_logic::DictionaryValue* dict = new base_logic::DictionaryValue;
  dict->SetString(L"name", "CustomWeightObserver");
  dict->SetString(L"stocks_info", stock_info);
  dict->SetString(L"start_date", start_date);
  dict->SetString(L"end_date", end_date);
  base_http::HttpAPI::RequestPostMethod(url, dict, result);
  data_->yield_ = result;
  if (dict) {
    delete dict;
    dict = NULL;
  }
}

void KenshoCalResult::sort() {
  data_->stock_list_.sort();
}

void KenshoCalResult::parser_right_sentence(const std::string& sentence) {
  std::string sentence_str = sentence;
  while (sentence_str.length() != 0) {
    size_t start_pos = sentence_str.find(":");
    if (start_pos == std::string::npos) {
      LOG(INFO) << "kafka receive sentence illegel format " <<  sentence;
      return;
    }
    size_t end_pos =
        sentence_str.find("+") == std::string::npos ?
            sentence.length() : sentence_str.find("+");
    std::string type = sentence_str.substr(0, start_pos);
    if (type.length() >= sentence_str.length()) {
      LOG(INFO) << "kafka receive sentence illegel format :" <<  sentence;
      return;
    }
    std::string sentence_tmp = sentence_str.substr(type.length() + 1,
                                               end_pos - type.length() - 1);
    BackTestSentence  asentence;
    asentence.is_right = true;
    asentence.value = sentence_tmp;
    if (type.length() > 0) {
      asentence.id = atoi(type.c_str());
      LOG(INFO) << "Right sentence id " <<  asentence.id << " value:" <<  sentence_tmp.c_str();
    } else {
      asentence.id = 0;
    }
    data_->right_sentence_info_.push_back(asentence);
    if (sentence_str.find("+") != std::string::npos)
      sentence_str = sentence_str.substr(end_pos + 1, sentence_str.length());
    else
      sentence_str.clear();
  }
}

void KenshoCalResult::parser_wrong_sentence(const std::string& sentence) {
  std::string sentence_str = sentence;
  while (sentence_str.length() != 0) {
    size_t pos = sentence_str.find(",");
    std::string sentence_tmp = sentence_str.substr(0,pos);
    BackTestSentence  asentence;
    asentence.is_right = false;
    asentence.id = 0;
    asentence.value = sentence_tmp;
    data_->wrong_sentence_info_.push_back(asentence);
    if (pos != std::string::npos && pos + 1 < sentence_str.length())
      sentence_str = sentence_str.substr(pos + 1, sentence_str.length());
    else
      sentence_str.clear();
  }
}

}
