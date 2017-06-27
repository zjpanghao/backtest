//  Copyright (c) 2015-2016 The BACKSVC Authors. All rights reserved.
//  Created on: 2016年9月1日 Author: tangtao

#ifndef BACK_FUZZY_SEARCH_LOGIC_H_
#define BACK_FUZZY_SEARCH_LOGIC_H_

#include "back_share/back_share_engine.h"
#include "fuzzy_search/fuzzy_search_factory.h"
#include "fuzzy_search/fuzzy_search_packet_process.h"
#include "net/comm_head.h"
#include "core/common.h"
#include "basic/basictypes.h"
#include "basic/scoped_ptr.h"

namespace fuzzy_search_logic {

class FuzzysearchLogic {
 public:
  FuzzysearchLogic();
  virtual ~FuzzysearchLogic();
 private:
  static FuzzysearchLogic *instance_;
 public:
  static FuzzysearchLogic* GetInstance();
  static void FreeInstance();
 public:

  bool OnFuzzysearchConnect(struct server *srv, const int socket);

  bool OnFuzzysearchMessage(struct server *srv, const int socket, const void *msg, const int len);

  bool OnFuzzysearchClose(struct server *srv, const int socket);

  bool OnBroadcastConnect(struct server *srv, const int socket, const void *data, const int len);

  bool OnBroadcastMessage(struct server *srv, const int socket, const void *msg, const int len);

  bool OnBroadcastClose(struct server *srv, const int socket);

  bool OnIniTimer(struct server *srv);

  bool OnTimeout(struct server *srv, char *id, int opcode, int time);

 private:
  bool Init();

 private:
  back_share::BSEngine *bs_engine_;
  fuzzy_search_logic::FuzzysearchFactory *factory_;
  back_logic::http_packet::PacketProcess *packet_;
};

}   //namespace fuzzy_search_logic

#endif
