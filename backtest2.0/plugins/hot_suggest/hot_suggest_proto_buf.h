//
// Created by harvey on 2016/8/31.
//

#ifndef BACKSVC_HOT_SUGGEST_PROTO_BUF_H
#define BACKSVC_HOT_SUGGEST_PROTO_BUF_H

#include "basic/basictypes.h"
#include "logic/base_values.h"
#include "net/proto_buf.h"
#include "logic/back_infos.h"
#include "logic/logic_comm.h"

namespace hot_suggest_logic {
namespace net_request {

class HotSuggestRequestMsg : public back_logic::PacketHead {
 public:
  HotSuggestRequestMsg()
      : uid_(NULL),
        token_(NULL),
        flag_(NULL),
        pos_(NULL),
        count_(NULL) {
  }

  virtual ~HotSuggestRequestMsg() {
    if (uid_) {
      delete uid_;
      uid_ = NULL;
    }
    if (token_) {
      delete token_;
      token_ = NULL;
    }
    if (flag_) {
      delete flag_;
      flag_ = NULL;
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

  void set_flag(const int32 flag) {
    flag_ = new base_logic::FundamentalValue(flag);
  }

  void set_pos(const int32 pos) {
    pos_ = new base_logic::FundamentalValue(pos);
  }

  void set_count(const int32 count) {
    count_ = new base_logic::FundamentalValue(count);
  }

  const int64 uid() const {
    int64 uid = 0;
    if (NULL != uid_) {
      uid_->GetAsBigInteger(&uid);
    }
    return uid;
  }

  const std::string token() const {
    std::string token;
    if (NULL != token_) {
      token_->GetAsString(&token);
    }
    return token;
  }

  const int32 flag() const {
    int32 flag = 0;
    if (NULL != flag_) {
      flag_->GetAsInteger(&flag);
    }
    return flag;
  }

  const int32 pos() const {
    int32 pos = 0;
    if (NULL != pos_) {
      pos_->GetAsInteger(&pos);
    }
    return pos;
  }

  const int32 count() const {
    int32 count = 0;
    if (NULL != count_) {
      count_->GetAsInteger(&count);
    }
    return count;
  }

 private:
  base_logic::FundamentalValue* uid_;
  base_logic::StringValue* token_;
  base_logic::FundamentalValue* flag_;
  base_logic::FundamentalValue* pos_;
  base_logic::FundamentalValue* count_;
};

class AllSuggestRequest : public HotSuggestRequestMsg {
 public:
  AllSuggestRequest()
      : after_sentence_("") {

  }
  virtual ~AllSuggestRequest() {}

  virtual void set_http_packet(base_logic::DictionaryValue* value) {
    // set base packet
    HotSuggestRequestMsg::set_http_packet(value);

    bool r = false;
    std::string after_sentence;
    r = value->GetString(L"after_sentence", &after_sentence);
    if (r) {
      std::string out_str;
      base::BasicUtil::UrlDecode(after_sentence, out_str);
      after_sentence_ = out_str;
    }
  }

  const std::string after_sentence() const {
    return after_sentence_;
  }

 private:
  std::string after_sentence_;
};

} /*namespace net_request*/

namespace net_reply {

class HotSuggestResponseMsg : public back_logic::PacketHead {
 public:
  HotSuggestResponseMsg()
      : list_value_(NULL), total_value_(NULL) {
  }

  virtual ~HotSuggestResponseMsg() {
    // list_value_ 已经在 body_value 中 delete
    if (body_value_) {
      delete body_value_;
      body_value_ = NULL;
    }
  }

  virtual base_logic::DictionaryValue* body() {
    body_value_ = new base_logic::DictionaryValue();
    if (NULL != total_value_) {
      body_value_->SetWithoutPathExpansion(L"total_num", total_value_);
    }
    if (NULL != list_value_) {
      body_value_->SetWithoutPathExpansion(L"sentences", list_value_);
    }
    return body_value_;
  }

  void set_list_value(base_logic::ListValue* listValue) {
    list_value_ = listValue;
  }

  void append_sentence(base_logic::DictionaryValue* dictValue) {
    if (NULL != dictValue) {
      list_value_->Append(dictValue);
    }
  }

  void set_total_sentence_num(int32 total_num) {
    total_value_ = new base_logic::FundamentalValue(total_num);
  }

 private:
  base_logic::FundamentalValue* total_value_;
  base_logic::ListValue* list_value_;
};

} /*namespace net_reply*/

} /*namespace hot_suggest_logic*/

#endif //BACKSVC_HOT_SUGGEST_PROTO_BUF_H
