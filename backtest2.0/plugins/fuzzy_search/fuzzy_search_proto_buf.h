//  Copyright (c) 2015-2016 The BACK Authors. All rights reserved.
//  Created on: 2016年9月1日 Author: tangtao

#ifndef BACK_FUZZY_SEARCH_PROTO_H__
#define BACK_FUZZY_SEARCH_PROTO_H__

#include "basic/basictypes.h"
#include "basic/scoped_ptr.h"
#include "logic/base_values.h"
#include "logic/logic_comm.h"
#include "logic/values.h"
#include "net/proto_buf.h"
#include "logic/back_infos.h"
#include "fuzzy_search/fuzzy_search_handle.h"
#include "fuzzy_search/fuzzy_sentence_view.h"
namespace fuzzy_search_logic {

namespace net_request {

class BTSondition : public back_logic::PacketHead {
 public:
  BTSondition() : uid_(NULL),
                  token_(NULL),
                  sonditions_(NULL) {
  }

  ~BTSondition() {
    if (uid_) {
      delete uid_;
      uid_ = NULL;
    }
    if (token_) {
      delete token_;
      token_ = NULL;
    }
    if (sonditions_) {
      delete sonditions_;
      sonditions_ = NULL;
    }
    if (start_time_) {
      delete start_time_;
      start_time_ = NULL;
    }

    if (end_time_) {
      delete end_time_;
      end_time_ = NULL;
    }
  }

  virtual void set_http_packet(base_logic::DictionaryValue *value);

  void set_uid(const int64 uid) {
    uid_ = new base_logic::FundamentalValue(uid);
  }

  void set_token(const std::string &token) {
    token_ = new base_logic::StringValue(token);
  }

  void set_sonditions(const std::string &sonditions) {
    sonditions_ = new base_logic::StringValue(sonditions);
  }

  void set_start_time(const std::string &start_time){
    start_time_ = new base_logic::StringValue(start_time);
  }

  void set_end_time(const std::string &end_time) {
    end_time_ = new base_logic::StringValue(end_time);
  }

  const int64 uid() const {
    int64 uid = 0;
    if (uid_)
      uid_->GetAsBigInteger(&uid);
    return uid;
  }

  const std::string token() const {
    std::string token;
    if (token_)
      token_->GetAsString(&token);
    return token;
  }

  const std::string sonditions() const {
    std::string sonditions;
    if (sonditions_)
      sonditions_->GetAsString(&sonditions);
    return sonditions;
  }

  const std::string start_time() const {
    std::string start_time;
    if (start_time_)
      start_time_->GetAsString(&start_time);
    return start_time;
  }

  const std::string end_time() const {
    std::string end_time;
    if (end_time_)
      end_time_->GetAsString(&end_time);
    return end_time;
  }
 public:
  base_logic::FundamentalValue *uid_;
  base_logic::StringValue *token_;
  base_logic::StringValue *sonditions_;
  base_logic::StringValue *start_time_;
  base_logic::StringValue *end_time_;
};

}   //namespace net_request
namespace net_reply {
class BTSondition : public back_logic::PacketHead {
 public:
  BTSondition() : prompt_(L"prompt"), check_(L"check") {
    // 层面在此处添加， type 对应 index
    const wchar_t *sentence_class[] =  {L"invalid", L"1", L"2", L"3", L"4", L"5", L"6", L"7", L"8", L"9", L"10"};
    for (int i = 0; i < static_cast<int>(sizeof(sentence_class) / sizeof(wchar_t*)); i++) {
      std::wstring tmp(sentence_class[i]);
      sentence_class_.push_back(tmp);
    }
  }

  ~BTSondition() {
    // 对list中数据进行析构
    if (body_value_) {
      base_logic::DictionaryValue *root = NULL;
      body_value_->GetDictionary(prompt_, &root);
      std::vector<std::wstring>::iterator it = sentence_class_.begin();
      while (it != sentence_class_.end()) {
        const std::wstring key = *it;
        DeleteGroup(key, root);
        it++;
      }

      body_value_->GetDictionary(check_, &root);
      DeleteGroup(L"checked_sentences", root);

      if (body_value_) {
        delete body_value_;
        body_value_ = NULL;
      }

    }
  }

  void DeleteGroup(std::wstring key, base_logic::DictionaryValue* root) {
    base_logic::ListValue* listvalue;
    bool r = root->GetList(key, &listvalue);
    if (!r) {
      return;
    }
    while (listvalue->GetSize()) {
      base_logic::Value* result_value;
      listvalue->Remove(0, &result_value);
      base_logic::DictionaryValue* dvalue =
          static_cast<base_logic::DictionaryValue*>(result_value);
      base_logic::ListValue* pmlist;
      r = dvalue->GetList(L"params", &pmlist);
      if (r) {
        while (pmlist->GetSize()) {
          base_logic::Value *param;
          pmlist->Remove(0, &param);
          delete param;
        }
      }
      delete result_value;
    }

  }

  void AddGroup(std::wstring group_name,
                const std::list<SentenceFindResult> &search_result,
                base_logic::DictionaryValue* root) {
    std::list<SentenceFindResult>::const_iterator it = search_result.begin();
    base_logic::ListValue* list = new base_logic::ListValue();
    while (it != search_result.end()) {
      base_logic::DictionaryValue* info_value =
          new base_logic::DictionaryValue();
      info_value->SetBigInteger(L"id", it->sentence_id);
      info_value->SetBigInteger(L"type", it->type);
      if (group_name != L"checked_sentences") {
        std::string view_str = it->full_sentence; // FuzzySentenceView::DateTransform(it->full_sentence);
        info_value->SetString(L"sentence", view_str);
        info_value->SetString(L"mode_sentence", it->mode_sentence);
        if (it->type ==  EVENT_TYPE) {
          info_value->SetString(L"abstract", it->abstract);
          info_value->SetBigInteger(L"start_ts", it->start_ts);
          info_value->SetBigInteger(L"end_ts", it->end_ts);
        }
      }
      if (it->type ==  EVENT_TYPE) {
        info_value->SetString(L"abstract", it->abstract);
        info_value->SetBigInteger(L"start_ts", it->start_ts);
        info_value->SetBigInteger(L"end_ts", it->end_ts);
        info_value->SetString(L"sentence", it->full_sentence);
        
      }  
      info_value->SetBigInteger(L"hot", it->hot);
      base_logic::ListValue* param_ll = new base_logic::ListValue();
      std::vector<back_logic::SentenceParam>::const_iterator pit = it->precise_params.begin();
      while (pit != it->precise_params.end()) {
        base_logic::StringValue *string_value = new base_logic::StringValue(pit->value + "s");
        //base_logic::FundamentalValue *v = new base_logic::FundamentalValue(
        //    pit->value);
        param_ll->Append(string_value);
        pit++;
      }
      info_value->Set(L"params", (base_logic::Value*) (param_ll));
      list->Append((base_logic::Value*) (info_value));
      it++;
    }

    root->Set(group_name, list);

  }
  // 处理协议， 按层面返回给前端
  base_logic::DictionaryValue* body() {
    body_value_ = new base_logic::DictionaryValue();
    int status = checked_sentences_.size() > 0 ? 1 : 0;
    base_logic::DictionaryValue *prompt = new base_logic::DictionaryValue();
    body_value_->Set(prompt_, prompt);

    std::map<int, std::list<SentenceFindResult> >::iterator mit = group_result_.begin();
    while (mit != group_result_.end()) {
      std::list<SentenceFindResult> &search_result = mit->second;
      AddGroup(sentence_class_[mit->first], search_result, prompt);
      mit++;
    }
    base_logic::DictionaryValue *check = new base_logic::DictionaryValue();
    body_value_->Set(check_, check);
    AddGroup(L"checked_sentences", checked_sentences_, check);
    check->SetInteger(L"status", status);
    return body_value_;
  }
  void set_search_result(const BackTestSearchResult &bvc_search_result) {
    checked_sentences_ = bvc_search_result.check_sentences;
    std::map<int, std::list<SentenceFindResult> >::iterator mit;
    std::list<SentenceFindResult>::const_iterator it = bvc_search_result.prompt.begin();
    while (it != bvc_search_result.prompt.end()) {
      if (it->type >= static_cast<int>(sentence_class_.size())) {
        it++;
        continue;
      }
      mit = group_result_.find(it->type);
      if (mit == group_result_.end()) {
        std::list<SentenceFindResult> group_list;
        group_list.push_back(*it);
        group_result_[it->type] = group_list;
      } else {
        mit->second.push_back(*it);
      }
      it++;
    }
  }
 private:
  std::vector<std::wstring> sentence_class_;
  std::map<int, std::list<SentenceFindResult> > group_result_;
  std::list<SentenceFindResult>   checked_sentences_;
  const std::wstring prompt_;
  const std::wstring check_;
};

}  // namespace net_reply

}   //namespace fuzzy_search_logic
#endif
