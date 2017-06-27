//  Copyright (c) 2015-2016 The BACK Authors. All rights reserved.
//  Created on: 2016年9月1日 Author: tangtao

#ifndef BACK_FUZZY_SEARCH_SENTENCE_H_
#define BACK_FUZZY_SEARCH_SENTENCE_H_

#include "logic/back_infos.h"
#include "back_share/back_share_engine.h"

namespace fuzzy_search_logic {

class FuzzysearchSentenceManager {
 public:
  FuzzysearchSentenceManager();
  virtual ~FuzzysearchSentenceManager();
 private:
  void Init();
  void Dest();
 public:
  void Init(back_share::BSEngine *bs_engine);
 public:
  //void OnBTSentence(const int socket,                   \
  //                  base_logic::DictionaryValue *dict,  \
  //                  back_logic::PacketHead *pakcet);
  void GetBTSentence(const int32 id, back_logic::BTSonditionSentence &sentence);

  void GetBatchBTSentence(std::list<int32> &ids);

 private:
  back_share::BSEngine *bs_engine_;
};


class FuzzysearchSentenceEngine {
 private:
  static FuzzysearchSentenceManager *sentence_mgr_;
  static FuzzysearchSentenceEngine *sentence_engine_;
  FuzzysearchSentenceEngine() {}
  virtual ~FuzzysearchSentenceEngine() {}
 public:
  static FuzzysearchSentenceManager* GetSentenceManager() {
    if (sentence_mgr_ == NULL)
      sentence_mgr_ = new FuzzysearchSentenceManager();
    return sentence_mgr_;
  }

  static FuzzysearchSentenceEngine *GetSentenceEngine() {
    if (sentence_engine_ == NULL)
      sentence_engine_ = new FuzzysearchSentenceEngine();
    return sentence_engine_;
  }
};

}   //namespace fuzzy_search_logic
#endif
