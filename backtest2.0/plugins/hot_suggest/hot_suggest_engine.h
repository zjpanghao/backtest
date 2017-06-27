//
// Created by harvey on 2016/9/1.
//

#ifndef BACKSVC_HOT_SUGGEST_ENGINE_H
#define BACKSVC_HOT_SUGGEST_ENGINE_H

#include "back_share/back_share_engine.h"
#include "hot_suggest/operator_code.h"
#include "logic/logic_comm.h"

#include <list>
#include <map>

using back_logic::BTSonditionSentence;

namespace hot_suggest_logic {

typedef std::list<BTSonditionSentence> SENTENCE_LIST;
typedef std::map<int32, SENTENCE_LIST> SENTENCE_MAP;
typedef std::map<int32, std::map<int32, int32> > SENTENCE_TYPE_MAP;

typedef std::map<std::string, int32> ALL_SENTENCE_MAP;
typedef std::map<int32, std::list<BTSonditionSentence> > BTS_ALLTYPE_MAP;

struct HotSuggestCache {
  SENTENCE_LIST sentence_list_;

  ALL_SENTENCE_MAP seword_map_;
  BTS_ALLTYPE_MAP bts_alltype_map_;
};

class HotSuggestManager {
 public:
  ~HotSuggestManager();
  void Initialize();
  void Init(back_share::BSEngine* bs_engine);

 public:
  void OnTimeUpdateShareData();

 private:
  friend class HotSuggestEngine;
  HotSuggestManager();
  DISALLOW_COPY_AND_ASSIGN(HotSuggestManager);
  void UpdateAllSentenceMap(SENTENCE_LIST& all_random_sentence_list);

  static bool BtHotEventSortFun(const BTSonditionSentence& lhs,
                                const BTSonditionSentence& rhs) {
    return lhs.create_time() > rhs.create_time();
  }

 public:
  bool GetSentencesListByType(int32 type, SENTENCE_LIST& sentence_list);
  BTS_ALLTYPE_MAP GetBtsAllTypeMap();
  void CheckSpecialSentence(std::string& sentence);
  void trim(std::string& sentence);


 private:
  struct threadrw_t* lock_;
  back_share::BSEngine* bs_engine_;
  HotSuggestCache* hot_suggest_cache_;
};

class HotSuggestEngine {
 private:
  static HotSuggestEngine* suggest_engine_;
  static HotSuggestManager* suggest_manager_;
  HotSuggestEngine() {}
  ~HotSuggestEngine() {}

 public:
  static HotSuggestEngine* GetHotSuggestEngine() {
    if (NULL == suggest_engine_) {
      suggest_engine_ = new HotSuggestEngine();
    }
    return suggest_engine_;
  }
  static HotSuggestManager* GetHotSuggestManager() {
    if (NULL == suggest_manager_) {
      suggest_manager_ = new HotSuggestManager();
    }
    return suggest_manager_;
  }

};

} /*namespace hot_suggest_logic*/

#endif //BACKSVC_HOT_SUGGEST_ENGINE_H
