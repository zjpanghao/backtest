/*
 * fuzzy_search_handle.h
 *
 *  Created on: 2016年9月6日
 *      Author: pangh
 */

#ifndef BACKSVC_PLUGINS_FUZZY_SEARCH_FUZZY_SEARCH_HANDLE_H_
#define BACKSVC_PLUGINS_FUZZY_SEARCH_FUZZY_SEARCH_HANDLE_H_
#include <list>
#include <string>
#include <vector>

#include "back_share/back_share_engine.h"
#include "logic/back_infos.h"
#include "fuzzy_search_engine.h"
namespace fuzzy_search_logic {
class SentenceMatch;
class FuzzySentence;

class FuzzySearchHandle {
 public:
  FuzzySearchHandle();
  ~FuzzySearchHandle();
  bool DoSearchWork(std::string search_str);
  const BackTestSearchResult & get_search_result() {
    return bvc_search_result_;
  }

 private:
  bool DoParse(std::string search_str,
               std::vector<FuzzySentence> *fuzzy_sentences);

  bool CheckValidSentence(const std::vector<FuzzySentence> &parse_result);

  bool DoCheckSentencesIntegrity(
      const back_share::SENTENCE_LIST &datasource,
      const std::vector<FuzzySentence> &parse_result);

  BackTestSearchResult  bvc_search_result_;
};

}  // namespace fuzzy_search_logic

#endif /* BACKSVC_PLUGINS_FUZZY_SEARCH_FUZZY_SEARCH_HANDLE_H_ */
