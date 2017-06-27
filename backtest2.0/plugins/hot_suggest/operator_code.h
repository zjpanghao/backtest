//
// Created by harvey on 2016/8/31.
//

#ifndef BACKSVC_OPERATOR_CODE_H
#define BACKSVC_OPERATOR_CODE_H

#include "net/comm_head.h"
#include "basic/basictypes.h"

namespace hot_suggest_logic {

// 句子分类
const int32 BS_ALL_TYPE = 0;            //全部
const int32 BS_BASIC_TYPE = 1;          //基本面
const int32 BS_MARKET_TYPE = 2;         //行情面
const int32 BS_TECHNOLOGRY_TYPE = 3;    //技术面
const int32 BS_MESSAGE_TYPE = 4;        //消息面
const int32 BS_HOT_EVENT = 5;           //热点事件
const int32 BS_HOTTEST_SENTENCE = 6;    //最热语句


#define HOT_SUGGEST_REQ   back_logic::HOT_SUGGEST_TYPE * 1000 + 1
#define HOT_SUGGEST_RPL   back_logic::HOT_SUGGEST_TYPE * 1000 + 2

#define ALL_SUGGEST_REQ   back_logic::HOT_SUGGEST_TYPE * 1000 + 3
#define ALL_SUGGEST_RPL   back_logic::HOT_SUGGEST_TYPE * 1000 + 4

}

#endif //BACKSVC_OPERATOR_CODE_H
