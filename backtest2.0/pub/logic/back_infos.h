//  Copyright (c) 2015-2016 The BACKSVC Authors. All rights reserved.
//  Created on: 2016年8月25日 Author: kerry
#ifndef BACKSVC_BACK_INFOS_H__
#define BACKSVC_BACK_INFOS_H__

#include "basic/basictypes.h"
#include "basic/basic_util.h"
#include "logic/base_values.h"
#include "logic/logic_comm.h"
#include "thread/base_thread_lock.h"
#include <list>

#include <string>
typedef  std::string ParamType;
namespace back_logic {

class BTSonditionParam;
typedef std::map<ParamType,back_logic::BTSonditionParam>  PARAM_MAP; //key_param unit
typedef std::list<back_logic::BTSonditionParam> PARAM_LIST;
typedef std::map<int32,PARAM_MAP> PARAMS_MAP;//index UNIT
typedef std::map<int32,PARAM_LIST> PARAMS_LIST;

struct SentenceParam {
  ParamType value;
  int precise;
};

class StockInfo {
 public:
  StockInfo();

  StockInfo(const StockInfo& stock);

  StockInfo& operator =(const StockInfo& stock);

   ~StockInfo() {
     if (data_ != NULL) {
       data_->Release();
     }
   }

  void set_id(const int32 id) {data_->id_ = id;}
  void set_outstanding(const double outstanding) {data_->outstanding_ = outstanding;}

  void set_symbol(const std::string& symbol) {data_->symbol_ = symbol;}

  void set_sespell(const std::string& sespell) {data_->sespell_ = sespell;}

  void set_seengname(const std::string& seengname) {data_->seengname_ = seengname;}

  void set_exchange(const std::string& exchange) {data_->exchange_ = exchange;}
  void set_bord_keycode(const std::string& keycode) {data_->bord_keycode_ = keycode;}

  void set_bord_keyname(const std::string& name) {data_->bord_name_ = name;}

  const int32 id() const {return data_->id_;}
  double  outstanding() const {return data_->outstanding_;}

  const std::string& name() const {return data_->name_;}

  const std::string& symbol() const {return data_->symbol_;}

  const std::string& sespell() const {return data_->sespell_;}

  const std::string& seengname() const {return data_->seengname_;}

  const std::string& exchange() const {return data_->exchange_;}
  const std::string& bord_keycode() const {return data_->bord_keycode_;}
  const std::string& bord_keyname() const {return data_->bord_name_;}

  void ValueSerialization(base_logic::DictionaryValue* dict);

 private:
  class Data {
   public:
    Data()
        : refcount_(1),
          id_(0),
          outstanding_(0.0) {
    }

   public:
    int32        id_;
    double       outstanding_;
    std::string  flag_;
    std::string  symbol_ext_;
    std::string  exchange_;
    std::string  symbol_;
    std::string  name_;
    std::string  sespell_;
    std::string  seengname_;
    std::string  bord_keycode_;
    std::string  bord_name_;

    void AddRef() {
      __sync_fetch_and_add(&refcount_, 1);
    }
    void Release() {
     if ( __sync_fetch_and_sub(&refcount_, 1) == 1)
       delete this;
    }

   private:
    int refcount_;
  };
  Data* data_;
};


class BTSonditionParam {
 public:

  void set_id(const int32 id) {data_.id_ = id;}

  void set_param(const SentenceParam &param) {data_.precise_param_ = param;}

  void set_hot(const int32 hot) {data_.hot_ = hot;}

  void hot_add() {
    __sync_fetch_and_add(&data_.hot_, 1);
  }

  int32 id() const {return data_.id_;}


  SentenceParam precise_param() const {return data_.precise_param_;}
  
  int32 hot() const {return data_.hot_;}

  static bool cmp(const BTSonditionParam& t_param,
                  const BTSonditionParam& r_param){
    return t_param.hot() > r_param.hot();
  }


 private:
  class Data {
   public:
    Data():
          id_(0),
          hot_(1){
    }

   public:
    int32 id_;
    SentenceParam  precise_param_;
    int32 hot_;

   private:
    int refcount_;
  };
  Data  data_;
};


class BTSonditionSentence {
 public:
  BTSonditionSentence();

  BTSonditionSentence(const BTSonditionSentence& sentence);

  BTSonditionSentence& operator =(const BTSonditionSentence& sentence);

  ~BTSonditionSentence() {
    if (data_ != NULL) {
      data_->Release();
    }
  }

  void set_id(const int32 id) {
    data_->id_ = id;
  }
  // only for test
  void set_type(const int32 type) {
     data_->type_ = type;
  }

  void set_sentence(const std::string& sentence) {
    data_->sentence_ = sentence;
  }

  void hot_add() {
    __sync_fetch_and_add(&data_->hot_, 1);
  }

  const int32 id() const {return data_->id_;}

  const std::string& sentence() const {return data_->sentence_;}

  std::string GetFullSentence() const {
    if (type() == 5)
      return sentence();
    std::list<BTSonditionParam> params;
    GetHottestParamsInfo(params);
    std::string full = sentence();
    while (true) {
      int pos = full.find("x");
      if (pos == -1)
        break;
      char buf[64];
      if (params.empty()) {
        strcpy(buf, "1");
      }
      else {
        BTSonditionParam pa = params.front();
        const char *param = pa.precise_param().value.c_str();
        snprintf(buf, sizeof(buf), "%s", param ? param : "1");
        params.pop_front();
      }
      full = full.replace(pos, 1, buf);
    }
    return full;
  }

  const std::string& seword() const { return data_->seword_; }

  const std::string& abstract() const {  return data_->abstract_; }

  const int32 type() const {return data_->type_;}

  const int32 hot() const {return data_->hot_;}

  const int64 create_time() const { return data_->create_time_; }

  const int64 update() const { return  data_->update_; }

  void ValueSerialization(base_logic::DictionaryValue* dict);

  void set_param(const int32 index, const SentenceParam &param);

  static bool cmp(const BTSonditionSentence& t_sentence,
                  const BTSonditionSentence& r_sentence){
    return t_sentence.hot() > r_sentence.hot();
  }

  void sort_param();

  void GetHottestParamsInfo(std::list<BTSonditionParam>& params_list) const;
  void get_params_list(PARAMS_LIST *params_list) const {
    base_logic::RLockGd lk(data_->lock_);
    *params_list = data_->params_list_;
  }

 private:
  class Data {
   public:
    Data()
        : refcount_(1),
          id_(0),
          opcode_(0),
          type_(0),
          hot_(),
          sentence_(" "),
          seword_(" "),
          abstract_(" "),
          create_time_(0),
          update_(0){
      InitThreadrw(&lock_);
    }
    ~Data(){
      DeinitThreadrw(lock_);
    }

   public:
    int32 id_;
    int32 opcode_;
    int32 type_;
    int32 hot_;
    struct threadrw_t* lock_;
    std::string sentence_;
    std::string seword_;
    std::string abstract_;
    int64 create_time_;
    int64 update_;
    //PARAM_MAP param_map_; //param_key->param unit
    PARAMS_MAP params_map_;
    PARAMS_LIST params_list_;

    void AddRef() {
      __sync_fetch_and_add(&refcount_, 1);
    }
    void Release() {
      if (__sync_fetch_and_sub(&refcount_, 1) == 1) {
        delete this;
      }
    }

   private:
    int refcount_;
  };
  Data* data_;
};
}
#endif
