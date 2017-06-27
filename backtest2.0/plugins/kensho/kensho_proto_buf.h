//  Copyright (c) 2015-2016 The BACK Authors. All rights reserved.
//  Created on: 2016年8月29日 Author: kerry

#ifndef BACK_KENSHO_PROTO_H__
#define BACK_KENSHO_PROTO_H__

#include "basic/basictypes.h"
#include "basic/basic_util.h"
#include "logic/base_values.h"
#include "net/proto_buf.h"
#include "logic/back_infos.h"
#include "kensho_common.h"

namespace kensho_logic {

namespace net_request {

class BTKenshoYield{
 public:
  BTKenshoYield()
      : uid_(NULL),
        token_(NULL),
        session_id_(NULL){
  }
  ~BTKenshoYield() {
    if (uid_) {
      delete uid_;
      uid_ = NULL;
    }
    if (token_) {
      delete token_;
      token_ = NULL;
    }
    if (session_id_) {
      delete session_id_;
      session_id_ = NULL;
    }
  }


  virtual void set_http_packet(base_logic::DictionaryValue* value);

  void set_uid(const int64 uid) {
    uid_ = new base_logic::FundamentalValue(uid);
  }

  void set_token(const std::string& token) {
    token_ = new base_logic::StringValue(token);
  }

  void set_session_id(const int64 session_id) {
    session_id_ = new base_logic::FundamentalValue(session_id);
  }


  const int64 uid() const {
    int64 uid = 0;
    if (uid_ != NULL)
      uid_->GetAsBigInteger(&uid);
    return uid;
  }

  const std::string token() const {
    std::string token;
    if (token_ != NULL)
      token_->GetAsString(&token);
    return token;
  }

  const int64 session_id() const {
    int64 session_id = 0;
    if (session_id_ != NULL)
      session_id_->GetAsBigInteger(&session_id);
    return session_id;
  }

 public:
  base_logic::FundamentalValue* uid_;
  base_logic::StringValue* token_;
  base_logic::FundamentalValue* session_id_;
};

class BTResult : public back_logic::PacketHead {
 public:
  BTResult()
      : uid_(NULL),
        token_(NULL),
        session_id_(NULL),
        pos_(NULL),
        count_(NULL){
  }
  ~BTResult() {
    if (uid_) {
      delete uid_;
      uid_ = NULL;
    }
    if (token_) {
      delete token_;
      token_ = NULL;
    }
    if (session_id_) {
      delete session_id_;
      session_id_ = NULL;
    }
    if (pos_) {
      delete pos_;
      pos_ = NULL;
    }

    if (count_) {
      delete count_;
      count_ = NULL;
    }
  }

  virtual void set_http_packet(base_logic::DictionaryValue* value);

  void set_uid(const int64 uid) {
    uid_ = new base_logic::FundamentalValue(uid);
  }

  void set_token(const std::string& token) {
    token_ = new base_logic::StringValue(token);
  }

  void set_session_id(const int64 session_id) {
    session_id_ = new base_logic::FundamentalValue(session_id);
  }

  void set_count(const int64 count) {
    count_ = new base_logic::FundamentalValue(count);
  }

  void set_pos(const int64 pos) {
    pos_ = new base_logic::FundamentalValue(pos);
  }

  const int64 uid() const {
    int64 uid = 0;
    if (uid_ != NULL)
      uid_->GetAsBigInteger(&uid);
    return uid;
  }

  const std::string token() const {
    std::string token;
    if (token_ != NULL)
      token_->GetAsString(&token);
    return token;
  }

  const int64 session_id() const {
    int64 session_id = 0;
    if (session_id_ != NULL)
      session_id_->GetAsBigInteger(&session_id);
    return session_id;
  }

  const int64 count() const {
     int64 count = 10;
     if (count_ != NULL)
       count_->GetAsBigInteger(&count);
     return count;
   }

   const int64 pos() const {
     int64 pos = 0;
     if (pos_ != NULL)
       pos_->GetAsBigInteger(&pos);
     return pos;
   }

 public:
  base_logic::FundamentalValue* uid_;
  base_logic::StringValue* token_;
  base_logic::FundamentalValue* session_id_;
  base_logic::FundamentalValue* pos_;
  base_logic::FundamentalValue* count_;
};

class BTSondition : public back_logic::PacketHead {
 public:
  BTSondition()
      : uid_(NULL),
        base_sessionid_(NULL),
        token_(NULL),
        sonditions_(NULL),
        start_time_(NULL),
        end_time_(NULL){
  }

  ~BTSondition() {
    if (uid_) {
      delete uid_;
      uid_ = NULL;
    }

    if (base_sessionid_) {
      delete base_sessionid_;
      base_sessionid_ = NULL;
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

  virtual void set_http_packet(base_logic::DictionaryValue* value);

  void set_uid(const int64 uid) {
    uid_ = new base_logic::FundamentalValue(uid);
  }

  void set_base_sessionid(const int64 base_sessionid) {
    base_sessionid_ = new base_logic::FundamentalValue(base_sessionid);
  }

  void set_token(const std::string& token) {
    token_ = new base_logic::StringValue(token);
  }

  void set_sonditions(const std::string& sonditions) {
    sonditions_ = new base_logic::StringValue(sonditions);
  }

  void set_start_time(const std::string& start_time) {
    start_time_ = new base_logic::StringValue(start_time);
  }

  void set_end_time(const std::string& end_time) {
    end_time_ = new base_logic::StringValue(end_time);
  }


  const int64 uid() const {
    int64 uid = 0;
    if (uid_ != NULL)
      uid_->GetAsBigInteger(&uid);
    return uid;
  }

  const int64 base_sessionid() const {
    int64 base_sessionid = 0;
    if (base_sessionid_ != NULL)
      base_sessionid_->GetAsBigInteger(&base_sessionid);
    return base_sessionid;
  }

  const std::string token() const {
    std::string token;
    if (token_ != NULL)
      token_->GetAsString(&token);
    return token;
  }

  const std::string sonditions() const {
    std::string sonditions;
    if (sonditions_ != NULL)
      sonditions_->GetAsString(&sonditions);
    return sonditions;
  }

  const std::string start_time() const {
    std::string start_time;
    if(start_time_ != NULL)
      start_time_->GetAsString(&start_time);
    return start_time;
  }

  const std::string end_time() const {
    std::string end_time;
    if (end_time_ != NULL)
      end_time_->GetAsString(&end_time);
    return end_time;
  }

 public:
  base_logic::FundamentalValue* uid_;
  base_logic::FundamentalValue* base_sessionid_;
  base_logic::StringValue* token_;
  base_logic::StringValue* sonditions_;
  base_logic::StringValue* start_time_;
  base_logic::StringValue* end_time_;
};

}

namespace net_reply {

class BTResult : public back_logic::PacketHead {
 public:
  BTResult() {
    start_time_ = NULL;
    end_time_ = NULL;
    count_ = NULL;
    stocks_list_ = new base_logic::ListValue;
 
    wrong_sentence_list_ = new base_logic::ListValue;

  }

  ~BTResult() {
    if (body_value_) {
      DeleteGroup(L"right_sentence", body_value_);
      delete body_value_;
      body_value_ = NULL;
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
      delete result_value;
    }

  }


  void AddGroup(std::wstring group_name,
                const std::list<BackTestSentence> &search_result,
                base_logic::DictionaryValue* root) {
    std::list<BackTestSentence>::const_iterator it = search_result.begin();
    base_logic::ListValue* list = new base_logic::ListValue();
    while (it != search_result.end()) {
      base_logic::DictionaryValue* info_value =
          new base_logic::DictionaryValue();
      info_value->SetBigInteger(L"id", it->id);
      info_value->SetBigInteger(L"type", it->type);
      info_value->SetString(L"sentence", it->value);
	  info_value->SetString(L"abstract", it->abstract);
      info_value->SetBigInteger(L"start_time", it->start_time);
      info_value->SetBigInteger(L"end_time", it->end_time);
      list->Append((base_logic::Value*) (info_value));
      it++;
    }

    root->Set(group_name, list);

  }
  base_logic::DictionaryValue* body() {
    body_value_ = new base_logic::DictionaryValue();
    if (start_time_ != NULL)
      body_value_->SetWithoutPathExpansion(L"start_time", start_time_);
    if (end_time_ != NULL)
      body_value_->SetWithoutPathExpansion(L"end_time", end_time_);
    if (count_ != NULL)
      body_value_->SetWithoutPathExpansion(L"count", count_);
    if (stocks_list_ != NULL)
      body_value_->SetWithoutPathExpansion(L"stocks", stocks_list_);
    AddGroup(L"right_sentence", right_sentence_, body_value_);
    if (wrong_sentence_list_ != NULL)
      body_value_->SetWithoutPathExpansion(L"wrong_sentence",wrong_sentence_list_);

    return body_value_;
  }

  void set_stock(base_logic::DictionaryValue* value) {
    stocks_list_->Append(value);
  }

  void set_wrong(const std::string& sentence){
    wrong_sentence_list_->Append(base_logic::Value::CreateStringValue(sentence));
  }


 void set_sentence(BackTestSentence *sentence){
    if (sentence->is_right)
      right_sentence_.push_back(*sentence);
  }

  void set_stocks(base_logic::ListValue* list) {
    stocks_list_ = list;
  }

  void set_start_time(const std::string& start_time) {
    start_time_ = new base_logic::StringValue(start_time);
  }

  void set_end_time(const std::string& end_time) {
    end_time_ = new base_logic::StringValue(end_time);
  }

  void set_count(const int64 count) {
    count_ = new base_logic::FundamentalValue(count);
  }



 private:
  base_logic::FundamentalValue* count_;
  base_logic::StringValue* start_time_;
  base_logic::StringValue* end_time_;
  base_logic::ListValue* stocks_list_;

  base_logic::ListValue* wrong_sentence_list_;
  std::list<BackTestSentence> right_sentence_;
};

class BTSondition : public back_logic::PacketHead {
 public:
  BTSondition() {
    bt_session_ = NULL;
  }

  ~BTSondition() {
    if (body_value_) {
      delete body_value_;
      body_value_ = NULL;
    }
  }

  base_logic::DictionaryValue* body() {
    body_value_ = new base_logic::DictionaryValue();
    if (bt_session_ != NULL)
      body_value_->SetWithoutPathExpansion(L"bt_session", bt_session_);

    return body_value_;
  }
  void set_bt_session(int64 bt_session) {
    //bt_session_ = new base_logic::FundamentalValue(bt_session);
    bt_session_ = new base_logic::StringValue(base::BasicUtil::StringUtil::Int64ToString(bt_session));
  }
 private:
  base_logic::StringValue* bt_session_;
};
}
}
#endif
