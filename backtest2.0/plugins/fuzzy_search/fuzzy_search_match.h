/*
 * fuzzy_search_match.h
 *
 *  Created on: 2016年9月6日
 *      Author: pangh
 */

#ifndef BACKSVC_PLUGINS_FUZZY_SEARCH_FUZZY_SEARCH_MATCH_H_
#define BACKSVC_PLUGINS_FUZZY_SEARCH_FUZZY_SEARCH_MATCH_H_
#include <list>
#include <string>
#include <vector>

#include "back_share/back_share_engine.h"
#include "logic/back_infos.h"
namespace fuzzy_search_logic {
class FuzzySentence;
class SentenceMatch {
 public:
  SentenceMatch();
  virtual ~SentenceMatch();

  virtual bool Match(const back_logic::BTSonditionSentence *sentence,
                     const FuzzySentence &fuzzy_expression) = 0;


  static bool RegularFullMatch(const std::string origin_str,
                               const std::string compare_str);

  static bool RegularMatch(const std::string origin_str,
                           const std::string compare_str);
  // start with  x-x-x-x到x-x-x-x
  static bool RegularDateMatch(const std::string origin_str);

  std::string GetModeSentence(const FuzzySentence *sentence);
};

class SentenceFullMatch : public SentenceMatch {
 public:
  explicit SentenceFullMatch();
  virtual ~SentenceFullMatch();
  virtual bool Match(const back_logic::BTSonditionSentence *sentence,
                     const FuzzySentence &fuzzy_expression);
};

class SentencePartMatch : public SentenceMatch {
 public:
  explicit SentencePartMatch();
  virtual ~SentencePartMatch();
  virtual bool Match(const back_logic::BTSonditionSentence *sentence,
                     const FuzzySentence &fuzzy_expression);
};

}  // fuzzy_search_logic

#endif /* BACKSVC_PLUGINS_FUZZY_SEARCH_FUZZY_SEARCH_MATCH_H_ */
