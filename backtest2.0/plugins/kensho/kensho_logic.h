//  Copyright (c) 2015-2016 The BACKSVC Authors. All rights reserved.
//  Created on: 2016年8月25日 Author: kerry
#ifndef BACK_KENSHO_LOGIC_H_
#define BACK_KENSHO_LOGIC_H_
#include "back_share/back_share_engine.h"
#include "kensho/kensho_factory.h"
#include "kensho/kensho_packet_process.h"
#include "net/comm_head.h"
#include "core/common.h"
#include "basic/basictypes.h"
#include "basic/scoped_ptr.h"

namespace kensho_logic {

class KenshoLogic {
 public:
  KenshoLogic();
  virtual ~KenshoLogic();
 private:
  static KenshoLogic* instance_;
 public:
  static KenshoLogic* GetInstance();
  static void FreeInstance();
 public:

  bool OnKenshoConnect(struct server *srv, const int socket);

  bool OnKenshoMessage(struct server *srv, const int socket, const void *msg,
                       const int len);

  bool OnKenshoClose(struct server *srv, const int socket);

  bool OnBroadcastConnect(struct server *srv, const int socket,
                          const void *data, const int len);

  bool OnBroadcastMessage(struct server *srv, const int socket, const void *msg,
                          const int len);

  bool OnBroadcastClose(struct server *srv, const int socket);

  bool OnIniTimer(struct server *srv);

  bool OnTimeout(struct server *srv, char* id, int opcode, int time);

 private:
  bool Init();

 private:
  back_share::BSEngine*            bs_engine_;
  kensho_logic::KenshoFactory*      factory_;
  back_logic::http_packet::PacketProcess*  packet_;
};
}

#endif
