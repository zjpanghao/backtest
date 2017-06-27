//  Copyright (c) 2015-2016 The BACK Authors. All rights reserved.
//  Created on: 2016年8月31日 Author: kerry
#ifndef BACK_KENSHO_SENTENCE_H_
#define BACK_KENSHO_SENTENCE_H_
#define MAX_SENTENCE_HOT_UPDATE 1000

#include "logic/back_infos.h"
#include "back_share/back_share_engine.h"

namespace kensho_logic {

class KenshoSentenceManager {
 public:
  KenshoSentenceManager();
  virtual ~KenshoSentenceManager();
 private:
  void Init();
  void Dest();
 public:
  void Init(back_share::BSEngine* bs_engine);
 public:
  //void OnBTSentence(const int socket, base_logic::DictionaryValue* dict,
    //                back_logic::PacketHead* pakcet);
  void GetBTSentence(const int32 id, back_logic::BTSonditionSentence& sentence);

  void GetBatchBTSentence(std::list<int32>& ids);

  base_logic::Value* CreateBatchBTSentence(base_logic::ListValue* value,
                                           base_logic::DictionaryValue* dict_value);
  int PopSentenceHotForUpdate(int *id, int *hot); 

  void PrepareUpdateSentenceHot(back_logic::BTSonditionSentence& sentence) {
    pthread_mutex_lock(&lock_);
    if (sentence_hot_update_.size() < MAX_SENTENCE_HOT_UPDATE) {
      if (sentence.type() != 5) {
        sentence_hot_update_[sentence.id()] = sentence.hot();
      }
    }
    pthread_mutex_unlock(&lock_);
    LOG_MSG2("addhot %d:%d",   sentence.id(),sentence.hot());
  }

 private:
  pthread_mutex_t lock_;
  std::map<int, int> sentence_hot_update_;
  back_share::BSEngine*         bs_engine_;
};


class KenshoSentenceEngine {
 private:
  static KenshoSentenceManager*  sentence_mgr_;
  static KenshoSentenceEngine*   sentence_engine_;
  KenshoSentenceEngine() {}
  virtual ~KenshoSentenceEngine() {}
 public:
  static KenshoSentenceManager* GetSentenceManager() {
    if (sentence_mgr_ == NULL)
      sentence_mgr_ = new KenshoSentenceManager();
    return sentence_mgr_;
  }

  static KenshoSentenceEngine* GetSentenceEngine() {
    if (sentence_engine_ == NULL)
      sentence_engine_ = new KenshoSentenceEngine();
    return sentence_engine_;
  }

  static void FreeSentenceManager(){
    delete sentence_mgr_;
    sentence_mgr_ = NULL;
  }

  static void FreeSentenceEngine(){
    delete sentence_engine_;
    sentence_engine_ = NULL;
  }
};
}
#endif
