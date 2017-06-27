//
// Created by Harvey on 2016/10/25.
//

#ifndef BACKSVC_ALL_SUGGEST_HANDLER_H
#define BACKSVC_ALL_SUGGEST_HANDLER_H

#include "hot_suggest/hot_suggest_proto_buf.h"
#include "hot_suggest/hot_suggest_engine.h"

namespace hot_suggest_logic {

class AllSuggestHandler {
 public:
    AllSuggestHandler();
    ~AllSuggestHandler();

 public:
    bool OnAllSuggestRequest(net_request::AllSuggestRequest& req,
                             net_reply::HotSuggestResponseMsg& resp);

 private:
    DISALLOW_COPY_AND_ASSIGN(AllSuggestHandler);

    base_logic::DictionaryValue* AssembleBtSentence(
        std::list<BTSonditionSentence>& sentence_list,
        std::string& after_sentence, int32 page_count, int32 type);

 private:
    HotSuggestManager* hot_suggest_mgr_;
};

} /*namespace hot_suggest_logic*/

#endif //BACKSVC_ALL_SUGGEST_HANDLER_H
