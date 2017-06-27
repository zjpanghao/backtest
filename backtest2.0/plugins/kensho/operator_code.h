//  Copyright (c) 2015-2016 The BACKTEST Authors. All rights reserved.
//  Created on: 2016年8月30日 Author: kerry

#ifndef KENSHO_OPERATOR_CODE_H_
#define KENSHO_OPERATOR_CODE_H_

#include "net/comm_head.h"

namespace kensho_logic {

#define KENSHO_BT_SENTENCE_REQ  back_logic::KENSHO_TYPE * 1000 + 1
#define KENSHO_BT_SENTENCE_RLY  back_logic::KENSHO_TYPE * 1000 + 2
#define KENSHO_BT_RESULT_REQ    back_logic::KENSHO_TYPE * 1000 + 3
#define KENSHO_BT_RESULT_RLY    back_logic::KENSHO_TYPE * 1000 + 4
#define KENSHO_BT_YIELD_REQ     back_logic::KENSHO_TYPE * 1000 + 5
#define KENSHO_BT_YIELD_RLY     back_logic::KENSHO_TYPE * 1000 + 6
}
#endif
