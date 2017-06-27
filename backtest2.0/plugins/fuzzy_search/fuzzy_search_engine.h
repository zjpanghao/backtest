/*
 * fuzzy_search_engine.h
 *
 *  Created on: 2016年9月8日
 *      Author: pangh
 */

#ifndef PLUGINS_FUZZY_SEARCH_FUZZY_SEARCH_ENGINE_H_
#define PLUGINS_FUZZY_SEARCH_FUZZY_SEARCH_ENGINE_H_
#include <list>
#include <string>
#include <vector>

#include "back_share/back_share_engine.h"
#include "logic/back_infos.h"
#define BACKSVC_DEFAULT_PARAM "1"
#define BACKSVC_DEFAULT_PARAM_VIEW "X"
namespace fuzzy_search_logic {
class SentenceMatch;
class FuzzySentence;

enum BT_SENTENCE_TYPE{
  EVENT_TYPE = 5
};

struct SentenceFindResult {
  int type;
  int sentence_id;
  int hot;
  std::string full_sentence;
  std::string mode_sentence;
  std::vector<back_logic::SentenceParam> precise_params;
  // for event
  std::string abstract;
  long start_ts;
  long end_ts;
};

struct BackTestSearchResult {
  std::list<SentenceFindResult> prompt;
  std::list<SentenceFindResult> check_sentences;
};

class FuzzySearchEngine {
 public:
  explicit FuzzySearchEngine();

  virtual ~FuzzySearchEngine();

  SentenceMatch *get_match_engine() { return match_engine_;}

  void set_match_engine(SentenceMatch *match_engine) { match_engine_ = match_engine;}

  bool Match(const back_logic::BTSonditionSentence *sentence,
             const FuzzySentence &fuzzy_expression);
  //  keep a sorted list by hot
  bool InsertSentenceByHot(std::list<SentenceFindResult> *result,
                           const SentenceFindResult &sentence,
                           int max_results_return);
  int GetParamTotalIndexs(const std::string &mode_sentence) {
    int count = 0;
    for (int i = 0; i < static_cast<int>(mode_sentence.length()); i++) {
      if (mode_sentence[i] == 'x')
        count++;
    }
    return count;
  }

  bool BuildFullSentence(std::string mode_str, std::vector<back_logic::SentenceParam> params,
                         std::string *full_sentence);

 private:
  SentenceMatch *match_engine_;
};


class SentenceFinder : public FuzzySearchEngine{
 public:
  explicit SentenceFinder(const back_share::SENTENCE_LIST &data_sources);

  virtual ~SentenceFinder();

  bool FindSentences(const FuzzySentence &fuzzy_sentence,
                     std::list<SentenceFindResult> *result);
  bool GetAllParams(const back_logic::BTSonditionSentence &sentence,
                    const std::vector<back_logic::SentenceParam> &user_input_params,
                    std::vector<back_logic::SentenceParam>  *params);



 private:
  bool InsertSentenceByHot(std::list<SentenceFindResult> *result,
                           const SentenceFindResult &sentence);
  const back_share::SENTENCE_LIST &data_sources_;
  bool fetch_hottest_;
  // 检索时提高效率， ToDo 当海量数据时采用最小堆的方式
  int max_results_return_;
  bool joint_;
};

class EventFinder : public FuzzySearchEngine{
 public:
  explicit EventFinder(const back_share::SENTENCE_LIST &data_sources);

  virtual ~EventFinder();

  bool FindEvents(const FuzzySentence &fuzzy_sentence,
                     std::list<SentenceFindResult> *result);

 private:
  const back_share::SENTENCE_LIST &data_sources_;
  // 检索时提高效率， ToDo 当海量数据时采用最小堆的方式
  int max_results_return_;
};

class SentenceChecker : public FuzzySearchEngine{
 public:
  SentenceChecker(const back_share::SENTENCE_LIST &data_sources);
  virtual ~SentenceChecker();
  bool CheckModeExists(const FuzzySentence &fuzzy_sentence, SentenceFindResult *result);

 private:
  const back_share::SENTENCE_LIST &data_sources_;
  bool joint_;
};

class EventChecker : public FuzzySearchEngine{
 public:
  EventChecker(const back_share::SENTENCE_LIST &data_sources);
  virtual ~EventChecker();
  bool CheckEventExists(const FuzzySentence &fuzzy_sentence, SentenceFindResult *result);

 private:
  const back_share::SENTENCE_LIST &data_sources_;
};

}
#endif /* PLUGINS_FUZZY_SEARCH_FUZZY_SEARCH_ENGINE_H_ */
