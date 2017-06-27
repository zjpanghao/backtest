/*
 * fuzzy_search_test.h
 *
 *  Created on: 2016年9月2日
 *      Author: Administrator
 */

#ifndef BACKSVC_SRC_PLUGINS_FUZZY_SEARCH_TEST_FUZZY_SEARCH_TEST_H_
#define BACKSVC_SRC_PLUGINS_FUZZY_SEARCH_TEST_FUZZY_SEARCH_TEST_H_
#include <list>
#include <string>
#include <vector>

#include "back_share/back_share_engine.h"
#include "logic/back_infos.h"
namespace fuzzy_search_logic {
void fuzzy_search_test();
void fuzzy_get_test_sentencelist(back_share::SENTENCE_LIST *ll);
}
#endif /* BACKSVC_PLUGINS_FUZZY_SEARCH_TEST_FUZZY_SEARCH_TEST_H_ */
