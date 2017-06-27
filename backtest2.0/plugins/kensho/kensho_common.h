/*
 * kensho_common.h
 *
 *  Created on: 2016年10月13日
 *      Author: pangh
 */

#ifndef PLUGINS_KENSHO_KENSHO_COMMON_H_
#define PLUGINS_KENSHO_KENSHO_COMMON_H_

#define BACKTEST_KAFKA_STOCK_SEPARATOR "->"
#define BACKTEST_KAFKA_MESSAGE_DEFAULT_TYPE 1
#define BACKTEST_KAFKA_MESSAGE_ENHANCE_TYPE 2
#include <set>
namespace kensho_logic {

struct BackTestSentence {
  int id;
  int type;
  bool is_right;
  std::string value;
  time_t start_time;
  time_t end_time;
  std::string abstract;
};

struct BackTestKenShoResult {
  std::set<std::string> symbol_set;
  std::list<std::string> right_sentence;
  std::list<std::string> wrong_sentence;
  std::list<BackTestSentence>  right_sentence_info;
  std::list<BackTestSentence>  wrong_sentence_info;
};

}
#endif /* PLUGINS_KENSHO_KENSHO_COMMON_H_ */
