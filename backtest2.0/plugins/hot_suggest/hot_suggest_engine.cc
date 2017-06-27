//
// Created by harvey on 2016/9/1.
//

#include "hot_suggest_engine.h"

#include <sstream>
#include <algorithm>

namespace hot_suggest_logic {

HotSuggestEngine* HotSuggestEngine::suggest_engine_ = NULL;
HotSuggestManager* HotSuggestEngine::suggest_manager_ = NULL;

HotSuggestManager::HotSuggestManager() {
  hot_suggest_cache_ = new HotSuggestCache();
  Initialize();
}

HotSuggestManager::~HotSuggestManager() {
  DeinitThreadrw(lock_);
}

void HotSuggestManager::Initialize() {
  InitThreadrw(&lock_);
}

void HotSuggestManager::Init(back_share::BSEngine* bs_engine) {
  bs_engine_ = bs_engine;
  OnTimeUpdateShareData();    //初始化未排序的句子
}

void HotSuggestManager::OnTimeUpdateShareData() {
  bs_engine_->UpdateShareData();
  bs_engine_->GetAllSentencesList(hot_suggest_cache_->sentence_list_);
  UpdateAllSentenceMap(hot_suggest_cache_->sentence_list_);
}

void HotSuggestManager::UpdateAllSentenceMap(SENTENCE_LIST& sentence_list) {
  base_logic::WLockGd lk(lock_);
  hot_suggest_cache_->seword_map_.clear();
  hot_suggest_cache_->bts_alltype_map_.clear();
  SENTENCE_LIST::const_iterator const_iter(sentence_list.begin());
  for (; sentence_list.end() != const_iter; ++const_iter) {
    const BTSonditionSentence& sentence = (*const_iter);
    const std::string& seword = sentence.seword();
    int32 sentence_type = sentence.type();

    // 最热语句
    hot_suggest_cache_->bts_alltype_map_[BS_HOTTEST_SENTENCE].push_back(sentence);

    // 同一关键字语句只添加一次
    std::list<BTSonditionSentence>& bts_type_list =
        hot_suggest_cache_->bts_alltype_map_[sentence_type];
    if (!seword.empty()
        && hot_suggest_cache_->seword_map_.find(seword)
            == hot_suggest_cache_->seword_map_.end()) {
      hot_suggest_cache_->seword_map_[seword] =  sentence.id();
      bts_type_list.push_back(sentence);
    }
  }
  // 热点事件按创建时间排序
  SENTENCE_LIST& hot_event_list =
      hot_suggest_cache_->bts_alltype_map_[BS_HOT_EVENT];
  hot_event_list.sort(HotSuggestManager::BtHotEventSortFun);
}

bool HotSuggestManager::GetSentencesListByType(int32 type, SENTENCE_LIST& sentence_list) {
  base_logic::RLockGd lk(lock_);
  sentence_list.clear();
  SENTENCE_MAP::iterator iter(hot_suggest_cache_->bts_alltype_map_.find(type));
  if (hot_suggest_cache_->bts_alltype_map_.end() == iter) {
    return false;
  }

  sentence_list.insert(sentence_list.end(),
                       iter->second.begin(),
                       iter->second.end());
  return true;
}

BTS_ALLTYPE_MAP HotSuggestManager::GetBtsAllTypeMap() {
  base_logic::RLockGd lk(lock_);
  return hot_suggest_cache_->bts_alltype_map_;
}

void HotSuggestManager::CheckSpecialSentence(std::string& sentence) {
  trim(sentence);
  static const std::string prefix = "x-x-x-x到x-x-x-x";
  static const std::string replace_prefix = "t-t-t-t到t-t-t-t";
  size_t nPos = sentence.find(prefix);
  if (std::string::npos != nPos) {
    sentence = sentence.replace(0, (nPos + prefix.size()), replace_prefix);
    LOG_DEBUG2("replace special sentence after = %s", sentence.c_str());
  }
  std::stringstream ss;
  for (int i = 0; i < sentence.length(); ++i) {
    if (sentence.at(i) == '"') {
      ss << '\\"';
      continue;
    }
    ss << sentence.at(i);
  }
  sentence = ss.str();
}

void HotSuggestManager::trim(std::string& sentence) {
  size_t npos = std::string::npos;
  npos = sentence.find_first_not_of(" ");
  sentence.erase(0, npos);
  npos = sentence.find_last_not_of(" ");
  sentence.erase(npos + 1, sentence.length());
}

} /*namespace hot_suggest_logic*/
