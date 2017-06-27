/*
 * fuzzy_search_engine.cc
 *
 *  Created on: 2016年9月8日
 *      Author: pangh
 */
#include "fuzzy_search/fuzzy_search_engine.h"
#include <algorithm>
#include "basic/basictypes.h"
#include "logic/logic_comm.h"
#include "fuzzy_search/fuzzy_search_match.h"
#include "fuzzy_search/fuzzy_search_parser.h"
#define MAX_PARAM_LENGTH 32
namespace fuzzy_search_logic {
FuzzySearchEngine::FuzzySearchEngine()
    : match_engine_(NULL) {
}
FuzzySearchEngine::~FuzzySearchEngine() {
  delete match_engine_;
}

bool FuzzySearchEngine::Match(const back_logic::BTSonditionSentence *sentence,
             const FuzzySentence &fuzzy_expression) {
  return match_engine_->Match(sentence, fuzzy_expression);
}

bool FuzzySearchEngine::BuildFullSentence(std::string mode_str,
                                       std::vector<back_logic::SentenceParam> params,
                                       std::string *full_sentence) {
  int old_index = 0;
  std::string full_str;
  std::vector<back_logic::SentenceParam>::iterator it = params.begin();
  while (old_index < static_cast<int>(mode_str.length())) {
    int index = mode_str.find(BACKTEST_WILDCARD, old_index);
    if (index == -1)
      break;
    full_str += mode_str.substr(old_index, index - old_index);
    if (it != params.end()) {
      char buf[MAX_PARAM_LENGTH];
      LOG_MSG2("insert value %s", it->value.c_str());
      // default param
      if (it->value.length() > 0)
        snprintf(buf, sizeof(buf), "%s", it->value.c_str());
      else
        snprintf(buf, sizeof(buf), "%s", BACKSVC_DEFAULT_PARAM_VIEW);
      full_str += buf;
      it++;
    } else {
      return false;
    }
    old_index = index + strlen(BACKTEST_WILDCARD);
  }
  if (old_index < static_cast<int>(mode_str.length()))
    full_str += mode_str.substr(old_index, mode_str.length() - old_index);
  LOG_MSG2("full string %s", full_str.c_str());
  *full_sentence = full_str;
  return true;
}

bool FuzzySearchEngine::InsertSentenceByHot(std::list<SentenceFindResult> *result,
                                            const SentenceFindResult &sentence,
                                            int max_results_return) {
  std::list<SentenceFindResult>::iterator it = result->begin();
  while (it != result->end()) {
    long hot = it->hot;
    if (hot < sentence.hot)
      break;
    it++;
  }
  result->insert(it, sentence);
  if (static_cast<int>(result->size()) > max_results_return) {
    result->pop_back();
  }
  return true;
}

SentenceFinder::SentenceFinder(const back_share::SENTENCE_LIST &data_sources)
    : data_sources_(data_sources),
      fetch_hottest_(true),
      max_results_return_(10),
      joint_(true) {
  set_match_engine(new SentencePartMatch());
}

SentenceFinder::~SentenceFinder() {
}

bool SentenceFinder::GetAllParams(
    const back_logic::BTSonditionSentence &sentence,
    const std::vector<back_logic::SentenceParam> &user_input_params,
    std::vector<back_logic::SentenceParam>  *params) {

  back_logic::PARAMS_LIST params_list;
  sentence.get_params_list(&params_list);
  int need_index = GetParamTotalIndexs(sentence.sentence());
  if (!need_index)
    return false;

  LOG_MSG2("sentence %d Total need index %d", sentence.id(), need_index);
  std::vector<std::vector<back_logic::SentenceParam> > param_array(need_index);

  for (int index = 0; index < need_index; index++) {
    // default params
    param_array[index].resize(1);
    param_array[index][0].value = BACKSVC_DEFAULT_PARAM;
    param_array[index][0].precise = 0;

    // user input params
    if (static_cast<int>(user_input_params.size()) > index) {
      LOG_MSG2("User user param %s for index %d", user_input_params[index].value.c_str(), index);
      param_array[index][0] = user_input_params[index];
      params->push_back(param_array[index][0]);
      continue;
    }

    // fetch from store params , now only fetch the first(hottest)
    back_logic::PARAMS_LIST::iterator pit = params_list.find(index);
    if (pit != params_list.end()) {
      back_logic::PARAM_LIST param = pit->second;
      LOG_MSG2("index %d params size %d", index, param.size());
      if (param.size() > 0) {
        param_array[index][0] = param.begin()->precise_param();
        LOG_MSG2("index %d value %s", index, param.begin()->precise_param().value.c_str());
      }

    }
    params->push_back(param_array[index][0]);

  }

  return true;
}

bool SentenceFinder::InsertSentenceByHot(std::list<SentenceFindResult> *result,
                                         const SentenceFindResult &sentence) {
  std::list<SentenceFindResult>::iterator it = result->begin();
  while (it != result->end()) {
    long hot = it->hot;
    if (hot < sentence.hot)
      break;
    it++;
  }
  result->insert(it, sentence);
  if (static_cast<int>(result->size()) > max_results_return_) {
    result->pop_back();
  }
  return true;
}

bool SentenceFinder::FindSentences(const FuzzySentence &fuzzy_sentence,
                                   std::list<SentenceFindResult> *result) {
  back_share::SENTENCE_LIST::const_iterator it = data_sources_.begin();
  while (it != data_sources_.end()) {
    const back_logic::BTSonditionSentence &sentence = *it;
    if (sentence.type() == EVENT_TYPE) {
      it++;
      continue;
    }
    SentenceFindResult find_result;
    if (!Match(&sentence, fuzzy_sentence)) {
      it++;
      continue;
    }
    LOG_MSG2("sentence match id %d", sentence.id());

    find_result.sentence_id = sentence.id();
    find_result.type = sentence.type();
    find_result.hot = sentence.hot();
    find_result.mode_sentence = sentence.sentence();
    std::vector<back_logic::SentenceParam>  params;
    LOG_MSG2("Now get params for sid %d", sentence.id());
    GetAllParams(sentence, fuzzy_sentence.precise_params, &params);
    LOG_MSG2("Get params totol num %d for sid %d", params.size(), sentence.id());
    if (params.size() == 0) {
      find_result.mode_sentence = sentence.sentence();
      find_result.full_sentence = find_result.mode_sentence;
      InsertSentenceByHot(result, find_result);
      it++;
      continue;
    } else {
      // current the params size 1
      if (!params.empty()) {
        find_result.precise_params = params;
        if (joint_) {
          std::string full_sentence;
          if (!BuildFullSentence(sentence.sentence(), params, &full_sentence)) {
            LOG_ERROR2("Error build full sentence! %s",
                sentence.sentence().c_str());
            return false;
          }
          find_result.full_sentence = full_sentence;
        }
        InsertSentenceByHot(result, find_result);
      }
    }
    it++;
  }
  return true;
}

SentenceChecker::SentenceChecker(const back_share::SENTENCE_LIST &data_sources)
    : data_sources_(data_sources),
      joint_(true) {
  set_match_engine(new SentenceFullMatch());
}

SentenceChecker::~SentenceChecker() {
}

bool SentenceChecker::CheckModeExists(const FuzzySentence &fuzzy_sentence, SentenceFindResult *result) {
  back_share::SENTENCE_LIST::const_iterator it = data_sources_.begin();
  while (it != data_sources_.end()) {
    const back_logic::BTSonditionSentence &sentence = *it;
    if (sentence.type() == EVENT_TYPE) {
      it++;
      continue;
    }
    if (Match(&sentence, fuzzy_sentence)
        && GetParamTotalIndexs(sentence.sentence())
            == static_cast<int>(fuzzy_sentence.precise_params.size())) {
      result->sentence_id = sentence.id();
      result->type = sentence.type();
      result->hot = sentence.hot();
      result->mode_sentence = sentence.sentence();
      result->precise_params = fuzzy_sentence.precise_params;
      if (joint_) {
        if (!BuildFullSentence(result->mode_sentence, result->precise_params,
                               &result->full_sentence)) {
          LOG_ERROR2("Build full sentence error for %d", result->sentence_id);
          return false;
        }
      }
      return true;
    }
    it++;
  }
  return false;
}

EventFinder::EventFinder(const back_share::SENTENCE_LIST &data_sources)
    : data_sources_(data_sources),
      max_results_return_(10) {
}

EventFinder::~EventFinder() {
}

bool EventFinder::FindEvents(const FuzzySentence &fuzzy_sentence,
                                   std::list<SentenceFindResult> *result) {
  back_share::SENTENCE_LIST::const_iterator it = data_sources_.begin();
  while (it != data_sources_.end()) {
    const back_logic::BTSonditionSentence &sentence = *it;
    if (sentence.type() != EVENT_TYPE) {
      it++;
      continue;
    }

    SentenceFindResult find_result;
    if (!SentenceMatch::RegularMatch(sentence.sentence(), fuzzy_sentence.origin_str)) {
      it++;
      continue;
    }
    LOG_MSG2("sentence match id %d", sentence.id());
    find_result.sentence_id = sentence.id();
    find_result.type = sentence.type();
    find_result.hot = sentence.hot();
    find_result.mode_sentence = sentence.sentence();
    find_result.full_sentence = find_result.mode_sentence;
    find_result.abstract = sentence.abstract();
    find_result.start_ts = sentence.create_time();
    find_result.end_ts = sentence.update();
    InsertSentenceByHot(result, find_result, max_results_return_);
    it++;
  }
  return true;
}

EventChecker::EventChecker(const back_share::SENTENCE_LIST &data_sources)
    : data_sources_(data_sources) {
}

EventChecker::~EventChecker() {
}

bool EventChecker::CheckEventExists(const FuzzySentence &fuzzy_sentence, SentenceFindResult *result) {
  back_share::SENTENCE_LIST::const_iterator it = data_sources_.begin();
  while (it != data_sources_.end()) {
    const back_logic::BTSonditionSentence &sentence = *it;
    if (sentence.type() != static_cast<int>(EVENT_TYPE)) {
      it++;
      continue;
    }

    if (!SentenceMatch::RegularFullMatch(sentence.sentence(), fuzzy_sentence.origin_str)) {
      it++;
      continue;
    }

    result->sentence_id = sentence.id();
    result->type = sentence.type();
    result->hot = sentence.hot();
    result->mode_sentence = sentence.sentence();
    result->full_sentence = sentence.sentence();
    result->abstract = sentence.abstract();
    result->start_ts = sentence.create_time();
    result->end_ts = sentence.update();
    return true;
  }
  return false;
}

}

