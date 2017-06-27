/*
 * fuzzy_search_parser.cc
 *
 *  Created on: 2016年9月2日
 *      Author: pangh
 */
#include "fuzzy_search/fuzzy_search_parser.h"
#include <algorithm>
#include <queue>
#include "basic/basic_util.h"
#include "logic/back_infos.h"
#include "logic/logic_comm.h"

#define MIN_FULL_SENTENCE_LEN 1

namespace fuzzy_search_logic {

BackTestSentenceParser::BackTestSentenceParser() {
}

BackTestSentenceParser::~BackTestSentenceParser() {
}

std::string BackTestSentenceParser::Trim(const std::string &origin_str) {
  std::string::size_type pos = origin_str.find_first_not_of(' ');
  if (pos == std::string::npos) {
    return origin_str;
  }
  std::string::size_type pos2 = origin_str.find_last_not_of(' ');
  if (pos2 != std::string::npos) {
    return origin_str.substr(pos, pos2 -pos + 1);
  }
  return origin_str.substr(pos);
}

bool BackTestSentenceParser::UrlDecode(const std::vector<std::string> &origin,
                                      std::vector<std::string> *elems) {
  std::string decode;
  std::vector<std::string>::const_iterator it = origin.begin();
  while (it != origin.end()) {
    if (it->length() < 0)
      return false;
    std::string origin_str = *it;
    base::BasicUtil::UrlDecode(origin_str, decode);
    elems->push_back(decode);
    it++;
  }
  return true;
}

bool BackTestSentenceParser::Split(const std::string &s, std::string delim,
                                  std::vector<std::string> *elems) {
  int old_pos = 0;
  int pos = 0;
  std::vector<std::string> origin_str;
  while (true) {
    pos = s.find(delim, old_pos);
    if (pos == -1)
      break;
    int len = pos - old_pos;
    if (len) {
      elems->push_back(s.substr(old_pos, len));
    } else {
      return false;
    }
    old_pos = pos + delim.length();
  }

  int len = static_cast<int>(s.size()) - old_pos;
  if (len > 0) {
    elems->push_back(s.substr(old_pos, len));
  }
  return true;
}

bool BackTestSentenceParser::CheckParam(const FuzzySentence &fuzzy_sentence) {
  const std::vector<back_logic::SentenceParam> &param 
      = fuzzy_sentence.precise_params;
  std::vector<back_logic::SentenceParam>::const_iterator it = param.begin();
  bool dot = false;
  while (it != param.end()) {
    std::string value = it->value;
    if (value == "-")
      return false;
    for (int i = 0; i < value.length(); i++) {
      if (i != 0 && value[i] == '-') {
        return false;
      }
      if (value[i] == '.') {
        if (dot) {
          return false;
        }
        dot = true;
        if  (i == 0 || i == value.length() - 1)
          return false;
        if (value[i - 1] <'0' || value[i - 1] > '9')
          return false;
        if (value[i + 1] <'0' || value[i + 1] > '9')
          return false;
      }
    }
    it++;
    dot = true;
  }
  return true;
}
bool BackTestSentenceParser::GetModeSentence(std::string sentence,
                                             FuzzySentence *fuzzy_sentence) {
  if (sentence.length() < MIN_FULL_SENTENCE_LEN)
    return false;
  fuzzy_sentence->origin_str = sentence;
  int end = sentence.length() - 1;
  int old_end = sentence.length() - 1;
  float sum = 0;
  bool param_flag = false;
  int begin = 0;
  std::queue<unsigned char> ch_q;
  int symbol = 1;
  back_logic::SentenceParam  param;
  float factor = 10;
  bool float_flag = false;
  std::string  param_value;
  std::string sentence_mode;
  auto GetParam = [&] {
    sentence_mode += BACKTEST_WILDCARD;
    param.value = param_value;
    param.precise = 0;
    fuzzy_sentence->precise_params.push_back(param);
    LOG_MSG2("get input param %s", param.value.c_str());
  };

  while (begin <= end) {
    unsigned char c = sentence[begin];
    if ((c >= '0' && c <= '9') || c == '.' || c== '-') {
      if (param_flag == false) {
        param_value.clear();
      }
      param_value.append(1, c);
      param_flag = true;
    } else {
      if (param_flag) {
        GetParam();
      }
      sentence_mode.append(1, c);
      param_flag = false;
    }
    begin++;
  }
  // check last param
  if (param_flag) {
    GetParam();
  }
  LOG_MSG2("The mode str %s", sentence_mode.c_str());
  fuzzy_sentence->mode_str = sentence_mode;
  return CheckParam(*fuzzy_sentence);
}
#if 0
bool BackTestSentenceParser::GetModeSentence(std::string sentence,
                                             FuzzySentence *fuzzy_sentence) {
  if (sentence.length() < MIN_FULL_SENTENCE_LEN)
    return false;
  fuzzy_sentence->origin_str = sentence;
  int end = sentence.length() - 1;
  int old_end = sentence.length() - 1;
  float sum = 0;
  bool param_flag = false;
  int begin = 0;
  std::string mode_sentence;
  std::queue<unsigned char> ch_q;
  int symbol = 1;
  back_logic::SentenceParam  param;
  float factor = 10;
  bool float_flag = false;
  while (begin <= end) {
    unsigned char c = sentence[begin];
    if (c >= '0' && c <= '9') {
      if (param_flag == false) {
        std::string str;
        while (ch_q.size() > 0) {
          str.append(1, ch_q.front());
          ch_q.pop();
        }
        mode_sentence =  mode_sentence + str;
      }
      if (float_flag) {
        sum += (c - '0') / factor;
        factor *= 10;
      } else {
        sum *= 10;
        sum += c - '0';
      }
      param_flag = true;
    } else {
      float_flag = false;
      factor = 10;
      if (c == '-')
        symbol = -1;
      else if (c == '.') {
        float_flag = true;
        begin++;
        continue;
      }
      else
        ch_q.push(c);
      if (param_flag) {
        mode_sentence = mode_sentence + BACKTEST_WILDCARD;
        sum *= symbol;
        int precise = float_flag ? log10(factor / 10) : 0;
        param.value = sum;
        param.precise = precise;
        fuzzy_sentence->precise_params.push_back(param);
        LOG_MSG2("get input param %.3f  len:%d", sum, (int)precise);
        sum = 0;
      }
      param_flag = false;
    }
    begin++;
  }
  // check str
  std::string str;
  while (ch_q.size() > 0) {
    str.append(1, ch_q.front());
    ch_q.pop();
  }
  mode_sentence =  mode_sentence + str;
  // check last param
  if (param_flag) {
    sum *= symbol;
    int precise = float_flag ? log10(factor / 10) : 0;
    param.value = sum;
    param.precise = precise;
    fuzzy_sentence->precise_params.push_back(param);
    mode_sentence = mode_sentence + BACKTEST_WILDCARD;
    LOG_MSG2("get input param %.3f len:%d", sum, precise);
  }
  LOG_MSG2("The mode str %s", mode_sentence.c_str());
  fuzzy_sentence->mode_str = mode_sentence;
  return true;
}
#endif
bool BackTestSentenceParser::Load(std::string request_str) {
  LOG_MSG2("Load %s", request_str.c_str());
  std::vector<std::string> origin_sentences;
  if (!Split(request_str, BACKTEST_LINK, &origin_sentences)) {
    LOG_ERROR2("parse error for %s", request_str.c_str());
    return false;
  }
  std::vector<std::string> &full_sentences = origin_sentences;

  // Trim
  std::vector<std::string>::iterator it = full_sentences.begin();
  while (it != full_sentences.end()) {
    *it = Trim(*it);
    it++;
  }

  if (full_sentences.size() == 0) {
    LOG_ERROR("No valid sentence found!");
    return false;
  }

  // As cann't recv sentences only  number, front end send extral char ','
  std::string  &last_str = full_sentences.back();
  if (last_str.length() > 0 && *(last_str.end() - 1) == ',')
    last_str = last_str.substr(0, last_str.length() - 1);
  it = full_sentences.begin();
  while (it != full_sentences.end()) {
    FuzzySentence sentence;
    LOG_MSG2("The input sentence is %s", it->c_str());
    if (!GetModeSentence(*it, &sentence)) {
      LOG_ERROR2("Fail to get mod for %s", it->c_str());
      return false;
    }
    // when constains only number, only search sentences with 0 params
    if (sentence.mode_str == BACKTEST_WILDCARD) {
      sentence.precise_params.clear();
      sentence.mode_str = sentence.origin_str;
    }
    mod_sentence_.push_back(sentence);
    it++;
  }
  return true;
}

}  // namespace fuzzy_search_logic

