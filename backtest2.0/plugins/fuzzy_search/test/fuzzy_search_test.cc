/*
 * fuzzy_search_test.cc
 *
 *  Created on: 2016年9月2日
 *      Author: Administrator
 */
#include "fuzzy_search/test/fuzzy_search_test.h"
#include "logic/logic_comm.h"
#include "fuzzy_search/fuzzy_search_parser.h"
#include "fuzzy_search/fuzzy_search_handle.h"
#include "fuzzy_search/fuzzy_search_match.h"
#include "fuzzy_search/fuzzy_sentence_view.h"
namespace fuzzy_search_logic {

void fuzzy_get_test_sentencelist(back_share::SENTENCE_LIST *ll) {
  back_logic::BTSonditionSentence sen;

  sen.set_sentence("总市值等于x亿");
  sen.set_id(1);
  sen.set_type(1);
  back_logic::SentenceParam pa;
  pa.value = "50";
  sen.set_param(0, pa);
  ll->push_back(sen);

}

void fuzzy_parser_test() {
}

void fuzzy_search_test() {
  back_logic::BTSonditionSentence sentence;
  LOG_MSG("fuzzy begin");
  //GetBackShareEngine()->SetSentence(0, &sentence);
  LOG_MSG("fuzzy test");
  FuzzySearchHandle handle;
  handle.DoSearchWork("rttyMAX");
  bool r = SentenceMatch::RegularDateMatch("2016-X-10-9到2016-9-10-23之间的查看热度大于2倍前30天日均热度标准差");
  std::string view = FuzzySentenceView::DateTransform("X-X-X-X到X-X-X-X之间的查看热度大于2倍前30天日均热度标准差");
  LOG_MSG2("The match %d", r);
  LOG_MSG2("The view is %s", view.c_str());
  LOG_MSG("Fuzzy test ok");
  //fuzzy_parser_test();
}

}

