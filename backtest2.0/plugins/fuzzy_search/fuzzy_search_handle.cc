/*
 * fuzzy_search_handle.cc
 *
 *  Created on: 2016年9月6日
 *      Author: pangh
 */
#include "fuzzy_search/fuzzy_search_handle.h"
#include <algorithm>
#include "basic/basictypes.h"
#include "logic/logic_comm.h"
#include "fuzzy_search/fuzzy_search_engine.h"
#include "fuzzy_search/fuzzy_search_match.h"
#include "fuzzy_search/fuzzy_search_parser.h"
#include "fuzzy_search/test/fuzzy_search_test.h"

namespace fuzzy_search_logic {

FuzzySearchHandle::FuzzySearchHandle() {
}

FuzzySearchHandle::~FuzzySearchHandle() {
}

bool FuzzySearchHandle::DoParse(std::string search_str,
                                std::vector<FuzzySentence> *fuzzy_sentences) {
  BackTestSentenceParser parser;
  if (!parser.Load(search_str))
    return false;
  *fuzzy_sentences = parser.get_parser_result();
  if (fuzzy_sentences->size() == 0)
    return false;
  return true;
}

/* 检查句子是否完整， 若前n-1个句子不完整返回 false 否则返回true
 * 检查结果存储在bvc_search_result_*/
bool FuzzySearchHandle::DoCheckSentencesIntegrity(
    const back_share::SENTENCE_LIST &datasource,
    const std::vector<FuzzySentence> &parse_result) {
  if (parse_result.size() == 0) {
    return false;
  }
  SentenceFindResult tmp;
  SentenceFindResult *last_result = &tmp;
  SentenceChecker checker(datasource);
  EventChecker event_checker(datasource);
  std::vector<FuzzySentence>::const_iterator it = parse_result.begin();
  while (it != (parse_result.end() - 1)) {
    const FuzzySentence &fuzzy_sentence = *it;
    LOG_MSG2("The modstr %s", fuzzy_sentence.mode_str.c_str());
    if (!checker.CheckModeExists(fuzzy_sentence, last_result)
        && !event_checker.CheckEventExists(fuzzy_sentence, last_result)) {
      bvc_search_result_.check_sentences.clear();
      LOG_MSG2("Check valid failed for %s, %s", fuzzy_sentence.mode_str.c_str(),
          fuzzy_sentence.origin_str.c_str());
      return false;
    }
    bvc_search_result_.check_sentences.push_back(*last_result);
    it++;
  }
  last_result->sentence_id = -1;
  last_result->precise_params = (parse_result.end() - 1)->precise_params;
  if (checker.CheckModeExists(*(parse_result.end() - 1), last_result)) {
    bvc_search_result_.check_sentences.push_back(*last_result);
    return true;
  }
  last_result->precise_params.clear();
  if (event_checker.CheckEventExists(*(parse_result.end() - 1), last_result)) {
    bvc_search_result_.check_sentences.push_back(*last_result);
    return true;
  }
  bvc_search_result_.check_sentences.clear();
  return true;
}

// sentences  must not contain "x"
bool FuzzySearchHandle::CheckValidSentence(
    const std::vector<FuzzySentence> &parse_result) {
  std::vector<FuzzySentence>::const_iterator it = parse_result.begin();
  while (it != parse_result.end()) {
    const FuzzySentence &sentence = *it;
    if (SentenceMatch::RegularMatch(sentence.origin_str, BACKTEST_WILDCARD)) {
      LOG_ERROR2("Sentence invalid %s", sentence.origin_str.c_str());
      return false;
    }
    it++;
  }
  return true;
}

bool FuzzySearchHandle::DoSearchWork(std::string search_str) {
  std::vector<FuzzySentence> parse_result;
  if (!DoParse(search_str, &parse_result))
    return false;

  back_share::SENTENCE_LIST datasource;
  // fuzzy_get_test_sentencelist(&datasource);
  GetBackShareEngine()->GetAllSentencesList(datasource);
  LOG_MSG2("Back share data sentence size %d", datasource.size());
  SentenceFindResult last_result;
  if (!DoCheckSentencesIntegrity(datasource, parse_result)) {
    return false;
  }
  // 找到完全匹配项，则不再搜索其他
  if (bvc_search_result_.check_sentences.size() > 0) {
    LOG_MSG("Check ok ");
  } else {
    SentenceFinder finder(datasource);
    EventFinder    event_finder(datasource);
    std::list<SentenceFindResult> search_result;
    const FuzzySentence &last_sentence = *(parse_result.end() - 1);
    LOG_MSG2("The last_str_mod %s", last_sentence.mode_str.c_str());
    // 对于sentence 不搜索带'x'语句
    if (!SentenceMatch::RegularMatch(last_sentence.origin_str, BACKTEST_WILDCARD)) {
      finder.FindSentences(last_sentence, &search_result);
    }
    LOG_MSG2("Find sentence size %d", search_result.size());
    LOG_MSG2("Event search str  --%s--", last_sentence.origin_str.c_str());
    event_finder.FindEvents(last_sentence, &search_result);
    LOG_MSG2("Find  events size %d", search_result.size());
    bvc_search_result_.prompt = search_result;
  }
  // for debug
  std::list<SentenceFindResult>::iterator rit = bvc_search_result_.prompt.begin();
  while (rit != bvc_search_result_.prompt.end()) {
    LOG_MSG2("The sentence %s", rit->full_sentence.c_str());
    rit++;
  }

  // for debug
  rit = bvc_search_result_.check_sentences.begin();
  while (rit != bvc_search_result_.check_sentences.end()) {
    LOG_DEBUG2("The checked sentence %s", rit->full_sentence.c_str());
    rit++;
  }
  return true;
}

}  // namespace fuzzy_search_logic

