/*
 * fuzzy_sentence_view.cc
 *
 *  Created on: 2016年10月24日
 *      Author: pangh
 */
#include "fuzzy_search/fuzzy_search_match.h"
#include "fuzzy_search/fuzzy_sentence_view.h"
namespace fuzzy_search_logic {
std::string FuzzySentenceView::DateTransform(const std::string &origin_sentence) {
  bool r = SentenceMatch::RegularDateMatch(origin_sentence);
  if (!r)
    return origin_sentence;
  std::string sentence = origin_sentence;
  // X_X_X_X到X_X_X_X
  int flag_num = 6;
  int begin_pos = 0;
  for (int i = 0; i < flag_num; i++) {
    int pos = sentence.find("-", begin_pos);
    // not likely
    if (pos == -1)
      return "";
    int xindex = pos - 1;
    if (xindex >= 0 && xindex < sentence.length()) {
      if (sentence[xindex] == 'X')
        sentence[xindex] = 't';
    }

    xindex = pos + 1;
    if (xindex >= 0 && xindex < sentence.length()) {
      if (sentence[xindex] == 'X')
        sentence[xindex] = 't';
    }
    begin_pos = pos + 1;
  }
  return sentence;
}

}

