//  Copyright (c) 2015-2016 The back Authors. All rights reserved.
//  Created on: 2016年8月29日 Author: kerry

#ifndef BACKSVC_NET_COMM_HEAD_H__
#define BACKSVC_NET_COMM_HEAD_H__
#define BACKTEST_CORE    "/var/www/tmp/backtestcorefile20"
namespace back_logic {

enum PACKET_TYPE {
  ERROR_TYPE = 0,
  KENSHO_TYPE = 1,
  HOT_SUGGEST_TYPE = 2,
  SEARCH_TYPE = 3,
  USER_LOGIN_TYPE = 4,
  GET_PLATFORM_TYPE = 5
};

enum ERROR_TYPE {
  PACKET_ILLEGAL = -101,
  UNPACK_FAILED = -102,
  TOKEN_INVALID = -103,
  ILLEGAL_TYPE = -104,
};
}
#endif
