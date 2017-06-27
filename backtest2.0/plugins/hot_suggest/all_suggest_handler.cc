//
// Created by Harvey on 2016/10/25.
//

#include "all_suggest_handler.h"
#include "basic/scoped_ptr.h"

#include <sstream>

namespace hot_suggest_logic {

AllSuggestHandler::AllSuggestHandler() {
  hot_suggest_mgr_ = HotSuggestEngine::GetHotSuggestManager();
}

AllSuggestHandler::~AllSuggestHandler() {

}

bool AllSuggestHandler::OnAllSuggestRequest(net_request::AllSuggestRequest& req,
                                            net_reply::HotSuggestResponseMsg& resp) {
  using base_logic::DictionaryValue;

  int32 flag = req.flag();
  std::string after_sentence = req.after_sentence();
  int32 page_count = req.count();
  if (page_count <= 0) {
    page_count = 8;
  }
  LOG_DEBUG2("RequestAllSuggestSentence，flag=%d, after_sentence=%s, page_count=%d",
             flag, after_sentence.c_str(), page_count);
  BTS_ALLTYPE_MAP bts_alltype_map = hot_suggest_mgr_->GetBtsAllTypeMap();
  scoped_ptr<base_logic::ListValue> list_value(new base_logic::ListValue());
  BTS_ALLTYPE_MAP::iterator iter = bts_alltype_map.begin();
  if (BS_ALL_TYPE != flag) {
    iter = bts_alltype_map.find(flag);
  }
  for (; bts_alltype_map.end() != iter; ++iter) {
    DictionaryValue* dict = AssembleBtSentence(
        iter->second, after_sentence, page_count, iter->first);
    if (NULL != dict) {
      list_value->Append(dict);
    }
    if (BS_ALL_TYPE != flag) {
      break;
    }
  }
  resp.set_list_value(list_value.release());
  return true;
}

base_logic::DictionaryValue* AllSuggestHandler::AssembleBtSentence(
    std::list<BTSonditionSentence>& sentence_list,
    std::string& after_sentence,
    int32 page_count,
    int32 type) {
  if (sentence_list.empty()) {
    LOG_DEBUG2("sentence type=%d empty", type);
    return NULL;
  }
  int count = 0;
  bool is_find = false;
  scoped_ptr<base_logic::DictionaryValue> dict_rsp(new base_logic::DictionaryValue());
  scoped_ptr<base_logic::ListValue> list_value(new base_logic::ListValue());
  std::list<BTSonditionSentence>::iterator iter(sentence_list.begin());
  for (; sentence_list.end() != iter; ++iter) {
    // find request begin pos
    const BTSonditionSentence& btSonditionSentence = (*iter);
    std::string sentence;
    if (BS_HOTTEST_SENTENCE == type) {
      sentence = btSonditionSentence.GetFullSentence();
    } else {
      sentence = btSonditionSentence.seword();
    }

    if (after_sentence.empty()) {
      is_find = true;
    } else if (!is_find && (sentence == after_sentence)) {
      is_find = true;
      continue;
    }
    // begin assemble response
    if (is_find && --page_count >= 0) {
      base_logic::DictionaryValue* dict = new base_logic::DictionaryValue();
      //组装语句和参数
      hot_suggest_mgr_->CheckSpecialSentence(sentence);
      dict->SetString(L"sentence", sentence);
      if (BS_HOT_EVENT == btSonditionSentence.type()) {
        dict->SetBigInteger(L"create_time", btSonditionSentence.create_time());
        dict->SetBigInteger(L"update", btSonditionSentence.update());

        std::string abstract = btSonditionSentence.abstract();
        hot_suggest_mgr_->CheckSpecialSentence(abstract);
        dict->SetString(L"abstract", abstract);
      }
      list_value->Append(dict);
      ++count;
    }
  }
  // assemble request type total condition num
  if (is_find) {
    std::stringstream ss;
    ss << type;
    dict_rsp->Set(ss.str(), list_value.release());
    dict_rsp->SetInteger(L"totals", (int32) sentence_list.size());
  } else {
    LOG_MSG2("allsuggest not find begin pos: %s in type: %d",
             after_sentence.c_str(), type);
  }
  LOG_DEBUG2("send allsuggest sentence: %d", count);
  return dict_rsp.release();
}

} /*namespace hot_suggest_logic*/
