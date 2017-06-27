//  Copyright (c) 2015-2016 The BACK Authors. All rights reserved.
//  Created on: 2016年8月30日 Author: kerry

#include "back_share/back_share_engine.h"
#include "logic/logic_comm.h"
#include "basic/template.h"

#define DEFAULT_CONFIG_PATH  "./back_share/backshare_config.xml"

back_share::BSEngine *GetBackShareEngine() {
  return new back_share::BSEngineImpl();
}

namespace back_share {

int BacktestAction::OnMessage(const BacktestMessage &message) {
  return 0;
}

void BSEngineImpl::Init() {
  BackShareEngine::GetBackShareManager();
}

bool BSEngineImpl::SetStock(const std::string& symbol,
                            back_logic::StockInfo* stock) {
  BackShareManager* bs_mgr = BackShareEngine::GetBackShareManager();
  return bs_mgr->SetStock(symbol, stock);
}

bool BSEngineImpl::GetStock(const std::string& symbol,
                            back_logic::StockInfo* stock) {
  BackShareManager* bs_mgr = BackShareEngine::GetBackShareManager();
  return bs_mgr->GetStock(symbol, stock);
}

bool BSEngineImpl::SetSentence(const int32 id,
                               back_logic::BTSonditionSentence* sentence) {
  BackShareManager* bs_mgr = BackShareEngine::GetBackShareManager();
  return bs_mgr->SetSentence(id, sentence);
}

bool BSEngineImpl::GetSentence(const int32 id,
                 back_logic::BTSonditionSentence* sentence) {
   BackShareManager* bs_mgr = BackShareEngine::GetBackShareManager();
   return bs_mgr->GetSentence(id, sentence);
}
void BSEngineImpl::GetBatchListSentences(
    std::list<int32>& ids,
    std::list<back_logic::BTSonditionSentence>& sentences) {
  BackShareManager* bs_mgr = BackShareEngine::GetBackShareManager();
  bs_mgr->GetBatchListSentences(ids, sentences);
}

void BSEngineImpl::GetBatchMapSentences(
    std::list<int32>& ids,
    std::map<const int32, back_logic::BTSonditionSentence>& sentences) {
  BackShareManager* bs_mgr = BackShareEngine::GetBackShareManager();
  bs_mgr->GetBatchMapSentences(ids, sentences);
}

void BSEngineImpl::GetBatchListStock(std::list<std::string>& symbol_list,
                                     std::list<back_logic::StockInfo>& stocks) {
  BackShareManager* bs_mgr = BackShareEngine::GetBackShareManager();
  bs_mgr->GetBatchListStock(symbol_list, stocks);
}

void BSEngineImpl::GetBatchMapStock(STOCK_MAP *map) {
  BackShareManager* bs_mgr = BackShareEngine::GetBackShareManager();
  bs_mgr->GetBatchMapStock(map);
}

void BSEngineImpl::SortSentenceByHot() {
  BackShareManager* bs_mgr = BackShareEngine::GetBackShareManager();
  bs_mgr->SortSentenceByHot();
}

void BSEngineImpl::SortParamByHot() {
  BackShareManager* bs_mgr = BackShareEngine::GetBackShareManager();
  bs_mgr->SortParamByHot();
}

void BSEngineImpl::GetAllSentencesList(
    std::list<back_logic::BTSonditionSentence>& sentences) {
  BackShareManager* bs_mgr = BackShareEngine::GetBackShareManager();
  bs_mgr->GetAllSentencesList(sentences);
}

void BSEngineImpl::GetAllRondomSentenceList(SENTENCE_LIST& sentences) {
  BackShareManager* bs_mgr = BackShareEngine::GetBackShareManager();
  bs_mgr->GetAllRondomSentenceList(sentences);
}
void BSEngineImpl::UpdateShareData() {
  BackShareManager* bs_mgr = BackShareEngine::GetBackShareManager();
  bs_mgr->UpdateShareData();
}

void BSEngineImpl::UpdateShareLongPeriodData() {
  BackShareManager* bs_mgr = BackShareEngine::GetBackShareManager();
  bs_mgr->UpdateShareLongPeriodData();
 
}

void BSEngineImpl::UpdateParam(int sentence_id, int index, back_logic::SentenceParam param) {
  BackShareManager* bs_mgr = BackShareEngine::GetBackShareManager();
  bs_mgr->UpdateParam(sentence_id, index, param);
}

void BSEngineImpl::RegisterAction(int message_type, BacktestAction *action) {
  BackShareManager* bs_mgr = BackShareEngine::GetBackShareManager();
  bs_mgr->RegisterAction(message_type, action);;
}
void BSEngineImpl::OnMessage(const BacktestMessage &message) {
  BackShareManager* bs_mgr = BackShareEngine::GetBackShareManager();
  bs_mgr->OnMessage(message);
}
///////////////////////////////////////////////////////////////////////////////

BackShareManager* BackShareEngine::backshare_mgr_ = NULL;
BackShareEngine* BackShareEngine::backshare_engine_ = NULL;

BackShareManager::BackShareManager() {
  share_cache_ = new BackShareCache();
  Init();
}

BackShareManager::~BackShareManager() {
  if (back_share_db_) {
    delete back_share_db_;
    back_share_db_ = NULL;
  }
  DeinitThreadrw(lock_);
}

void BackShareManager::Init() {
  InitThreadrw(&lock_);
  LOG_MSG2("%s", "backshare init");

  bool r = false;
  std::string path = DEFAULT_CONFIG_PATH;
  config::FileConfig* config = config::FileConfig::GetFileConfig();
  if (config == NULL) {
    return;
  }
  r = config->LoadConfig(path);
  if (!r)
    return;

  back_share_db_ = new back_share::BackShareDB(config);
  UpdateStock();
  UpdateSentence();
  UpdateHotParams();
}

bool BackShareManager::SetStock(const std::string& symbol,
                                back_logic::StockInfo* stock) {
  base_logic::WLockGd lk(lock_);
  STOCK_MAP::iterator it = share_cache_->secondary_stock_map_.find(
      stock->symbol());
  if (share_cache_->secondary_stock_map_.end() != it) {
    // LOG_ERROR2("find stock reconnected symbol %s", stock->symbol().c_str());
    return false;
  }

  share_cache_->secondary_stock_list_.push_back((*stock));
  return base::MapAdd<STOCK_MAP, std::string, back_logic::StockInfo>(
      share_cache_->secondary_stock_map_, stock->symbol(), (*stock));
}

bool BackShareManager::GetStock(const std::string& symbol,
                                back_logic::StockInfo* stock) {
  base_logic::RLockGd lk(lock_);
  return base::MapGet<STOCK_MAP, STOCK_MAP::iterator, std::string,
      back_logic::StockInfo>(share_cache_->secondary_stock_map_,
                             stock->symbol(), (*stock));
}

void BackShareManager::GetBatchMapStock(STOCK_MAP  *map) {
  base_logic::RLockGd lk(lock_);
  *map = share_cache_->secondary_stock_map_;
}

void BackShareManager::GetBatchListStock(
    std::list<std::string>& symbol_list,
    std::list<back_logic::StockInfo>& stocks) {
  base_logic::RLockGd lk(lock_);
  bool r = false;
  while (symbol_list.size() > 0) {
    back_logic::StockInfo stock;
    std::string symbol = symbol_list.front();
    symbol_list.pop_front();
    r = base::MapGet<STOCK_MAP, STOCK_MAP::iterator, std::string,
        back_logic::StockInfo>(share_cache_->secondary_stock_map_, symbol,
                               stock);
    if (r) {
      stocks.push_back(stock);
    }
  }
}

bool BackShareManager::SetSentence(const int32 id,
                                   back_logic::BTSonditionSentence* sentence) {
  base_logic::WLockGd lk(lock_);
  return SetSentenceNoLock(id, sentence);

}

bool BackShareManager::GetSentence(const int32 id,
                                   back_logic::BTSonditionSentence* sentence) {
  base_logic::WLockGd lk(lock_);
  return GetSentenceNoLock(id, sentence);

}
bool BackShareManager::SetSentenceNoLock(const int32 id, back_logic::BTSonditionSentence* sentence){
  SENTENCE_MAP::iterator it = share_cache_->sentence_map_.find(sentence->id());
    if (share_cache_->sentence_map_.end() != it) {
      // LOG_ERROR2("find stock reconnected Sentence id  %d", sentence->id());
      return false;
    }
    share_cache_->random_sentence_list_.push_front((*sentence));
    share_cache_->sentence_list_.push_back((*sentence));
    return base::MapAdd<SENTENCE_MAP, int32, back_logic::BTSonditionSentence>(
        share_cache_->sentence_map_, sentence->id(), (*sentence));
}

bool BackShareManager::GetSentenceNoLock(const int32 id, back_logic::BTSonditionSentence* sentence) {
  SENTENCE_MAP::iterator it = share_cache_->sentence_map_.find(id);
    if (share_cache_->sentence_map_.end() == it) {
      // LOG_ERROR2("find stock reconnected Sentence id  %d", sentence->id());
      return false;
    }
    *sentence = it->second;
    return true;
}

bool BackShareManager::SetStockNoLock(const std::string& symbol,
                                      back_logic::StockInfo* stock) {
  STOCK_MAP::iterator it = share_cache_->secondary_stock_map_.find(
      stock->symbol());
  if (share_cache_->secondary_stock_map_.end() != it) {
    // LOG_ERROR2("find stock reconnected symbol %s", stock->symbol().c_str());
    return false;
  }

  share_cache_->secondary_stock_list_.push_back((*stock));
  return base::MapAdd<STOCK_MAP, std::string, back_logic::StockInfo>(
      share_cache_->secondary_stock_map_, stock->symbol(), (*stock));
}

void BackShareManager::SortSentenceByHot() {
  base_logic::WLockGd lk(lock_);
  share_cache_->sentence_list_.sort(back_logic::BTSonditionSentence::cmp);
}

void BackShareManager::SortParamByHot() {
  base_logic::RLockGd lk(lock_);
  SENTENCE_MAP::iterator it = share_cache_->sentence_map_.begin();
  for (; it != share_cache_->sentence_map_.end();it++){
    back_logic::BTSonditionSentence sentence = it->second;
    sentence.sort_param();
  }
}

void BackShareManager::GetAllSentencesList(
    std::list<back_logic::BTSonditionSentence>& sentences) {
  base_logic::RLockGd lk(lock_);
  sentences = share_cache_->sentence_list_;
}

void BackShareManager::GetAllRondomSentenceList(SENTENCE_LIST& sentences) {
  base_logic::RLockGd lk(lock_);
  sentences = share_cache_->random_sentence_list_;
}

void BackShareManager::GetBatchListSentences(
    std::list<int32>& ids,
    std::list<back_logic::BTSonditionSentence>& sentences) {
  base_logic::RLockGd lk(lock_);
  bool r = false;
  while (ids.size() > 0) {
    back_logic::BTSonditionSentence sentence;
    int32 id = ids.front();
    ids.pop_front();
    r = base::MapGet<SENTENCE_MAP, SENTENCE_MAP::iterator, int32,
        back_logic::BTSonditionSentence>(share_cache_->sentence_map_, id,
                                         sentence);
    if (r) {
      sentences.push_back(sentence);
    }
  }
}

void BackShareManager::GetBatchMapSentences(
    std::list<int32>& ids,
    std::map<const int32,back_logic::BTSonditionSentence>& sentences) {
  base_logic::RLockGd lk(lock_);
  bool r = false;
  while (ids.size() > 0) {
    back_logic::BTSonditionSentence sentence;
    int32 id = ids.front();
    ids.pop_front();
    r = base::MapGet<SENTENCE_MAP, SENTENCE_MAP::iterator, int32,
        back_logic::BTSonditionSentence>(share_cache_->sentence_map_, id,
                                         sentence);
    if (r) {
      int id = sentence.id();
      sentences[id] = sentence;
    }
  }
}

void BackShareManager::UpdateShareData() {
  this->UpdateSentence();
}

void BackShareManager::UpdateShareLongPeriodData() {
  this->UpdateStock();
  // this->UpdateHotParams();
  SaveHotParams();
  LOG_MSG2("%s", "Update hotparams");
}

void BackShareManager::UpdateStock() {
  std::list<back_logic::StockInfo> list;
  back_share_db_->FetchBatchStcode(&list);
  if (list.empty())
    return;
  {
    base_logic::WLockGd lk(lock_);
    share_cache_->secondary_stock_list_.clear();
    share_cache_->secondary_stock_map_.clear();
    
    while (list.size() > 0) {
      back_logic::StockInfo stock = list.front();
      list.pop_front();
      SetStockNoLock(stock.symbol(), &stock);
    }
  }
}

void BackShareManager::ClearEvents(std::list<back_logic::BTSonditionSentence> *list) {
  std::list<back_logic::BTSonditionSentence>::iterator it 
      =  list->begin();
  while (it != list->end()) {
    if (it->type() == 5) {
      list->erase(it++);
    } else {
      it++;
    }
  }
}

 void BackShareManager::ClearEvents(SENTENCE_MAP  *map) {
   SENTENCE_MAP::iterator it
       =  map->begin();
   while (it != map->end()) {
     if (it->second.type() == 5) {
       map->erase(it++);
     } else {
       it++;
     }
   }
 }


void BackShareManager::UpdateHotParams() {
  std::map<int, std::map<int, back_logic::SentenceParam> > param_map;
  back_share_db_->FetchBatchSentenceParams(&param_map);
  if (param_map.empty())
    return;
  {
    base_logic::WLockGd lk(lock_);
    std::map<int, std::map<int, back_logic::SentenceParam> >::iterator it 
        = param_map.begin();

    while (it != param_map.end()) { 
      back_logic::BTSonditionSentence sentence;
      if (GetSentenceNoLock(it->first, &sentence)) {
        std::map<int, back_logic::SentenceParam> &param_index_map = it->second;
        for (int i = 0; i < it->second.size(); i++) {
          if (param_index_map.count(i) != 0) {
            LOG_MSG2("(%d  %d %s)", it->first, i, param_index_map[i].value.c_str());
            sentence.set_param(i, param_index_map[i]);
          }
        }
      }
      it++;
    }
  }

}

void BackShareManager::UpdateSentence() {
  std::list<back_logic::BTSonditionSentence> list;
  back_share_db_->FetchBatchSentence(&list);
  int size = list.size();
  back_share_db_->FetchBatchEvent(&list);
  int event_size = list.size() - size;
  {
    base_logic::WLockGd lk(lock_);
    if (event_size) {
      ClearEvents(&share_cache_->random_sentence_list_);
      ClearEvents(&share_cache_->sentence_list_);
      ClearEvents(&share_cache_->sentence_map_);
    }
    
    while (list.size() > 0) {
      back_logic::BTSonditionSentence sentence = list.front();
      list.pop_front();
      SetSentenceNoLock(sentence.id(), &sentence);
    }
  }
}

void BackShareManager::UpdateParam(int sentence_id, int index, back_logic::SentenceParam param) {
  base_logic::WLockGd lk(lock_);
  std::map<int, back_logic::BTSonditionSentence>::iterator it =
      share_cache_->sentence_map_.find(sentence_id);
  if (it == share_cache_->sentence_map_.end())
    return;
  it->second.set_param(index, param);
}

void BackShareManager::SaveHotParams() {
  std::map<int, std::map<int, back_logic::SentenceParam> > param_map;
  back_share_db_->FetchBatchSentenceParams(&param_map);
  LOG_MSG2("The param size %d", param_map.size());
  if (param_map.empty())
    return;
  std::map<int, std::map<int, back_logic::SentenceParam> >::iterator db_it;
  int count = 1;
  std::list<back_logic::BTSonditionSentence> sentence_list;
  GetAllSentencesList(sentence_list);
  std::list<back_logic::BTSonditionSentence>::iterator it = sentence_list.begin();
  while (it != sentence_list.end()) { 
    if (count % 10 == 0) {
      sleep(1);
    }
    back_logic::BTSonditionSentence &sentence = *it;
    if (sentence.type() == 5) {
      it++;
      continue;
    }
    db_it = param_map.find(sentence.id());
    std::map<int, back_logic::SentenceParam> param_index_map;
    if (db_it != param_map.end())
      param_index_map = db_it->second;
    std::list<back_logic::BTSonditionParam> params;
    sentence.GetHottestParamsInfo(params);
    int i = 0;
    int sentence_id = sentence.id();
    while (!params.empty()) {
      back_logic::BTSonditionParam &pa = params.front();
      if (param_index_map.count(i) == 0 ||
        pa.precise_param().value != param_index_map[i].value) {
        LOG_MSG2("Save params (%d %d %s)", sentence_id, i, pa.precise_param().value.c_str());
        back_share_db_->SaveHotParams(sentence_id, i, pa.precise_param());
        count++;
      }
      if (sentence_id == 425) {
        LOG_MSG2("the v1 %s  %s", pa.precise_param().value.c_str(), param_index_map[i].value.c_str());
      }
      i++;
      params.pop_front(); 
    }
    it++;
  }
}

}
