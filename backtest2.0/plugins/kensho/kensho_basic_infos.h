//  Copyright (c) 2015-2016 The BACKSVC Authors. All rights reserved.
//  Created on: 2016年8月25日 Author: kerry
#ifndef KENSHO_BASIC_INFOS_H__
#define KENSHO_BASIC_INFOS_H__

#include "basic/basictypes.h"
#include "logic/base_values.h"
#include "basic/basic_util.h"
#include "logic/logic_comm.h"
#include <list>
#include <string>
#include <algorithm>
#include "kensho/kensho_common.h"

namespace kensho_logic {

class KenshoStock {
 public:
  KenshoStock();
  KenshoStock(const KenshoStock& stock);

  KenshoStock& operator =(const KenshoStock& stock);

  virtual ~KenshoStock() {
    if (data_ != NULL) {
      data_->Release();
    }
  }

  bool operator < (const KenshoStock &stock) {
    return extra_info() < stock.extra_info();
  }
  static bool cmp(const KenshoStock& r_stock, const KenshoStock& t_stock) {
    LOG_DEBUG2("r_stock index %lld t_stock index %lld",
               r_stock.index(),t_stock.index());
    return r_stock.index() > t_stock.index();
  }

  void set_volume(const double volume) {
    data_->volume_ = volume;
  }
  void set_trade(const double trade) {
    data_->trade_ = trade;
  }
  void set_amount(const double amount) {
    data_->amount_ = amount;
  }
  void set_changepercent(const double changepercent) {
    data_->changepercent_ = changepercent;
  }
  void set_bvps(const double bvps) {
    data_->bvps_ = bvps;
  }
  void set_totals(const double totals) {
    data_->totals_ = totals;
  }
  void set_pb(const double pb) {
    data_->pb_ = pb;
  }
  void set_symbol(const std::string& symbol) {
    data_->symbol_ = symbol;
  }
  void set_name(const std::string& name) {
    data_->name_ = name;
  }
  void set_bord_keycode(const std::string& keycode) {
    data_->bord_keycode_ = keycode;
    base::BasicUtil::StringUtil::StringToInt64(data_->bord_keycode_,
                                               &data_->index_);
  }

  void set_extra_info(const std::string &extra_info) {
    data_->extra_info_ = extra_info;
  }

  void set_bord_keyname(const std::string& name) {
    data_->bord_name_ = name;
  }

  void set_outstanding(double outstanding) {
    data_->outstanding_ = outstanding;
  }
  const int64 index() const {
    return data_->index_;
  }
  const double volume() const {
    return data_->volume_;
  }
  const double trade() const {
    return data_->trade_;
  }
  const double amount() const {
    return data_->amount_;
  }
  const double changepercent() const {
    return data_->changepercent_;
  }
  const double bvps() const {
    return data_->bvps_;
  }
  const double totals() const {
    return data_->totals_;
  }
  const double pb() const {
    return data_->pb_;
  }
  const double market_value() const {
    //totalAssets_ * bvps_ * pb_
    return data_->totals_ * data_->bvps_ * data_->pb_;
  }
  const std::string& symbol() const {
    return data_->symbol_;
  }
  const std::string& name() const {
    return data_->name_;
  }
  const std::string& bord_keycode() const {
    return data_->bord_keycode_;
  }
  const std::string& bord_keyname() const {
    return data_->bord_name_;
  }
  double outstanding() const {
    return data_->outstanding_;
  }
  const std::string& extra_info() const {
     return data_->extra_info_;
  }

  void ValueSerialization(base_logic::DictionaryValue* dict);

 private:
  class Data {
   public:
    Data()
        : refcount_(1),
          volume_(0.0),
          trade_(0.0),
          amount_(0.0),
          changepercent_(0.0),
          index_(0),
          bvps_(0.0),
          totals_(0.0),
          pb_(0.0),
          outstanding_(0.0),
          market_value_(0.0){

    }

   public:
    std::string symbol_;
    std::string name_;
    std::string bord_keycode_;
    std::string bord_name_;
    // kafka stock date
    std::string extra_info_;
    int64 index_;
    double trade_;
    double volume_;  // 成交量
    double amount_;  //换手率
    double changepercent_;  //换手率
    double bvps_;
    double totals_;
    double pb_;
    double outstanding_;
    double market_value_; //市值

    void AddRef() {
      __sync_fetch_and_add(&refcount_, 1);
    }
    void Release() {
      if (__sync_fetch_and_sub(&refcount_, 1) == 1)
        delete this;
    }
   private:
    int refcount_;
  };
  Data* data_;
};
/*
struct node{
 bool operator()(const KenshoStock& t1,const KenshoStock& t2){

  LOG_DEBUG2("t1 index %lld t2 index %lld",
             t1.index(),t2.index());
  return t1.index()<t2.index();    //会产生升序排序,若改为>,则变为降序
 }
};*/

/*
template<>
struct std::greater<KenshoStock*>
{
    bool operator()( KenshoStock*_X,  KenshoStock* _Y) const
    {
        return (_X->index() > _Y->index());
    }
};*/

class KenshoCalResult {
 public:
  KenshoCalResult();
  KenshoCalResult(const KenshoCalResult& result);

  KenshoCalResult& operator =(const KenshoCalResult& result);

  virtual ~KenshoCalResult() {
    if (data_ != NULL) {
      data_->Release();
    }
  }

  typedef std::list<std::string>::iterator str_iterator;

  typedef std::list<KenshoStock>::iterator iterator;
  typedef std::list<KenshoStock>::const_iterator const_iterator;


  std::list<KenshoStock>::iterator begin() {
    return data_->stock_list_.begin();
  }
  std::list<KenshoStock>::iterator end() {
    return data_->stock_list_.end();
  }

  std::list<KenshoStock>::const_iterator begin() const {
    return data_->stock_list_.begin();
  }
  std::list<KenshoStock>::const_iterator end() const {
    return data_->stock_list_.end();
  }

  int64 size() const {
    return data_->stock_list_.size();
  }

  void cmp() {
    //list_a.sort(node());
    data_->stock_list_.sort(kensho_logic::KenshoStock::cmp);
    //data_->stock_list_.sort(std::greater<KenshoStock*>());
    //data_->stock_list_.sort(KenshoStock::cmp);
  }

  void create_yield();

  void sort();

  void parser_right_sentence(const std::string& sentence);

  void parser_wrong_sentence(const std::string& sentence);

  std::list<BackTestSentence>  get_right_sentence_info() {
    return data_->right_sentence_info_;
  }

  std::list<BackTestSentence>  get_wrong_sentence_info() {
     return data_->wrong_sentence_info_;
   }

  void InsertRightSentences(const std::list<BackTestSentence>  &right_sentence_info) {
    data_->right_sentence_info_.insert(data_->right_sentence_info_.begin(),
                                       right_sentence_info.begin(), right_sentence_info.end());
  }

  void InsertWrongSentences(const std::list<BackTestSentence>  &wrong_sentence_info) {
    data_->wrong_sentence_info_.insert(data_->wrong_sentence_info_.begin(),
                                       wrong_sentence_info.begin(), wrong_sentence_info.end());
  }

  void set_uid(const int64 uid) {
    data_->uid_ = uid;
  }
  void set_session_id(const int64 session_id) {
    data_->session_id_ = session_id;
  }
  void set_start_time(const std::string& start_time) {
    data_->start_time_ = start_time;
  }
  void set_end_time(const std::string& end_time) {
    data_->end_time_ = end_time;
  }
  void add_stock(kensho_logic::KenshoStock& stock) {
    data_->stock_list_.push_back(stock);
  }

  void set_yield(const std::string& yield){
    data_->yield_ = yield;
  }

  const int64 uid() const {
    return data_->uid_;
  }
  const int64 session_id() const {
    return data_->session_id_;
  }
  const std::string& start_time() const {
    return data_->start_time_;
  }
  const std::string& end_time() const {
    return data_->end_time_;
  }
  const std::string& yield() const {
    return data_->yield_;
  }

 private:
  class Data {
   public:
    Data()
        : refcount_(1),
          uid_(0),
          session_id_(0) {
    }
   public:
    int64 uid_;
    int64 session_id_;
    std::string start_time_;
    std::string end_time_;
    std::string yield_;
    std::list<KenshoStock> stock_list_;
    std::list<std::string> right_sentence_list_;
    std::list<std::string> wrong_sentence_list_;
    std::list<BackTestSentence> right_sentence_info_;
    std::list<BackTestSentence> wrong_sentence_info_;
    void AddRef() {
      __sync_fetch_and_add(&refcount_, 1);
    }
    void Release() {
      if (__sync_fetch_and_sub(&refcount_, 1) == 1)
        delete this;
    }
   private:
    int refcount_;
  };

  Data* data_;
};
}
#endif
