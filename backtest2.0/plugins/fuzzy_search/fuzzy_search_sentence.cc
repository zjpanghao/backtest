//  Copyright (c) 2015-2016 The BACK Authors. All rights reserved.
//  Created on: 2016年9月1日 Author: tangtao

#include "fuzzy_search_sentence.h"

namespace fuzzy_search_logic {

FuzzysearchSentenceManager *FuzzysearchSentenceEngine::sentence_mgr_ = NULL;
FuzzysearchSentenceEngine *FuzzysearchSentenceEngine::sentence_engine_ = NULL;

FuzzysearchSentenceManager::FuzzysearchSentenceManager() {
  Init();
}

FuzzysearchSentenceManager::~FuzzysearchSentenceManager() {
}

void FuzzysearchSentenceManager::Init(back_share::BSEngine *bs_engine) {
  bs_engine_ = bs_engine;
}

void FuzzysearchSentenceManager::GetBTSentence(const int32 id,  \
                     back_logic::BTSonditionSentence &sentence) {

}

void FuzzysearchSentenceManager::GetBatchBTSentence(std::list<int32> &ids) {
  std::list<back_logic::BTSonditionSentence> sentences;
  bs_engine_->GetBatchSentences(ids, sentences);
}
}
