/*
 * fuzzy_search_match.cc
 *
 *  Created on: 2016年9月6日
 *      Author: pangh
 */
#include "fuzzy_search/fuzzy_search_match.h"
#include "regex.h"
#include "logic/back_infos.h"
#include "logic/logic_comm.h"
#include "fuzzy_search/fuzzy_search_parser.h"

namespace fuzzy_search_logic {

SentenceMatch::SentenceMatch() {
}

SentenceMatch::~SentenceMatch() {
}

std::string SentenceMatch::GetModeSentence(const FuzzySentence *sentence) {
  return sentence->mode_str;
}

bool SentenceMatch::RegularMatch(const std::string origin_str,
                                 const std::string compare_str) {
  bool r = false;
  std::string regex_str = compare_str + ".*";
  regex_t reg_middle;
  if (regcomp(&reg_middle, regex_str.c_str(), REG_NOSUB | REG_ICASE) != 0) {
    LOG_DEBUG2("message %s", "reg init error");
    return false;
  }

  r = regexec(&reg_middle, origin_str.c_str(), 0, NULL, 0) == 0;
  regfree(&reg_middle);
  return r;
}

bool SentenceMatch::RegularFullMatch(const std::string origin_str,
                                     const std::string compare_str) {
  bool r = false;
  // return origin_str == compare_str;
  std::string regex_str = "^" + compare_str + "$";
  regex_t reg_middle;
  if (regcomp(&reg_middle, regex_str.c_str(), REG_NOSUB | REG_ICASE) != 0) {
    LOG_DEBUG2("message %s", "reg init error");
    return false;
  }

  r = regexec(&reg_middle, origin_str.c_str(), 0, NULL, 0) == 0;
  regfree(&reg_middle);
  return r;
}

bool SentenceMatch::RegularDateMatch(const std::string origin_str) {
  bool r = false;
  std::string regex_str = "^(([0-9]{1,32}|X)-){3}([0-9]{1,32}|X)到(([0-9]{1,32}|X)-){3}([0-9]{1,32}|X)";
  // std::string regex_str = "[0-9]{1,32}";
  regex_t reg_middle;
  if (regcomp(&reg_middle, regex_str.c_str(), REG_NOSUB|REG_EXTENDED) != 0) {
    LOG_DEBUG2("message %s", "reg init error");
    return false;
  }

  r = regexec(&reg_middle, origin_str.c_str(), 0, NULL, 0) == 0;
  regfree(&reg_middle);
  return r;
}

SentenceFullMatch::SentenceFullMatch() {
}

SentenceFullMatch::~SentenceFullMatch() {
}

bool SentenceFullMatch::Match(const back_logic::BTSonditionSentence *sentence,
                              const FuzzySentence &fuzzy_expression) {
  bool r = false;
  std::string regex_str = "^" + fuzzy_expression.mode_str + "$";
  regex_t reg_middle;
  if (regcomp(&reg_middle, regex_str.c_str(), REG_NOSUB | REG_ICASE) != 0) {
    LOG_DEBUG2("message %s", "reg init error");
    return false;
  }

  r = regexec(&reg_middle, sentence->sentence().c_str(), 0, NULL, 0) == 0;
  regfree(&reg_middle);
  return r;
}

SentencePartMatch::SentencePartMatch() {
}

SentencePartMatch::~SentencePartMatch() {
}

bool SentencePartMatch::Match(const back_logic::BTSonditionSentence *sentence,
                              const FuzzySentence &fuzzy_expression) {
  bool r = false;
  std::string regex_str = fuzzy_expression.mode_str + ".*";
  regex_t reg_middle;
  if (regcomp(&reg_middle, regex_str.c_str(), REG_NOSUB | REG_ICASE) != 0) {
    LOG_DEBUG2("message %s", "reg init error");
    return false;
  }

  r = regexec(&reg_middle, sentence->sentence().c_str(), 0, NULL, 0) == 0;
  regfree(&reg_middle);
  return r;
}

}  // namespace fuzzy_search_logic

