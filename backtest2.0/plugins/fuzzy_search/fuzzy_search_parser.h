/*
 * fuzzy_search_parser.h
 *
 *  Created on: 2016年9月2日
 *      Author: pangh
 */

#ifndef BACKSVC_PLUGINS_FUZZY_SEARCH_FUZZY_SEARCH_PARSER_H_
#define BACKSVC_PLUGINS_FUZZY_SEARCH_FUZZY_SEARCH_PARSER_H_
#include <list>
#include <string>
#include <vector>

#include "back_share/back_share_engine.h"
#include "logic/back_infos.h"

#define BACKTEST_WILDCARD "x"

#define BACKTEST_LINK "+"

namespace back_logic {
class BTSonditionSentence;
}

namespace fuzzy_search_logic {

struct FuzzySentence {
  std::string origin_str;
  std::string mode_str;
  std::vector<back_logic::SentenceParam> precise_params;
};

class BackTestSentenceParser {
 public:
  BackTestSentenceParser();

  ~BackTestSentenceParser();

  bool Load(std::string request_str);

  std::vector<FuzzySentence> get_parser_result() {
    return mod_sentence_;
  }

 private:
  std::string Trim(const std::string &origin_str);

  bool GetModeSentence(std::string sentence, FuzzySentence *fuzzy_sentence);

  bool CheckParam(const FuzzySentence &fuzzy_sentence);
  bool Split(const std::string &s, std::string delim,
            std::vector<std::string> *elems);
  bool UrlDecode(const std::vector<std::string> &origin,
                 std::vector<std::string> *elems);
  std::vector<FuzzySentence> mod_sentence_;
};

}  // namespace fuzzy_search_logic

#endif /* PLUGINS_FUZZY_SEARCH_FUZZY_SEARCH_PARSER_H_ */
