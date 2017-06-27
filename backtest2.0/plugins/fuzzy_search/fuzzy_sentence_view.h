/*
 * fuzzy_sentence_view.h
 *
 *  Created on: 2016年10月24日
 *      Author: pangh
 */

#ifndef PLUGINS_FUZZY_SEARCH_FUZZY_SENTENCE_VIEW_H_
#define PLUGINS_FUZZY_SEARCH_FUZZY_SENTENCE_VIEW_H_
#include <string>
namespace fuzzy_search_logic {
class FuzzySentenceView {
 public:
  static std::string DateTransform(const std::string &origin_sentence);
};
}
#endif /* PLUGINS_FUZZY_SEARCH_FUZZY_SENTENCE_VIEW_H_ */
