//
// Created by harvey on 2016/8/31.
//

#ifndef BACKSVC_HOT_SUGGEST_LOGIC_H
#define BACKSVC_HOT_SUGGEST_LOGIC_H

#include "net/comm_head.h"
#include "core/common.h"
#include "basic/basictypes.h"
#include "basic/scoped_ptr.h"
#include "back_share/back_share_engine.h"
#include "hot_suggest/hot_suggest_factory.h"
#include "hot_suggest/hot_suggest_packet_process.h"
#include "hot_suggest/hot_suggest_engine.h"

#define SORT_BTSOUND_SENTENCE_TIMER       10001
#define TIME_LOAD_CLASSIC_SENTENCE_TIMER  10002
#define UPDATE_SHARE_DATA_TIMER           10003

namespace hot_suggest_logic {

class HotSuggestLogic {
 public:
  ~HotSuggestLogic();
  static HotSuggestLogic* GetPtr();
  static HotSuggestLogic& GetRef();

 private:
  bool Initialize();

 public:
  bool OnHotSuggestConnect(struct server* srv, const int socket);
  bool OnHotSuggestClose(struct server* srv, const int socket);
  bool OnHotSuggestMessage(struct server* srv, const int socket,
                           const void* msg, const int len);

  bool OnBroadcastConnect(struct server* srv, const int socket,
                          const void* data, const int len);
  bool OnBroadcastClose(struct server* srv, const int socket);
  bool OnBroadcastMessage(struct server* srv, const int socket,
                          const void* msg, const int len);

  bool OnIniTimer(struct server* srv);
  bool OnTimeout(struct server* srv, char* id, int opcode, int time);
 private:
  HotSuggestLogic();
  DISALLOW_COPY_AND_ASSIGN(HotSuggestLogic);

 private:
  back_share::BSEngine* bs_engine_;
  HotSuggestFactory* factory_;
  back_logic::http_packet::PacketProcess* packet_;
  HotSuggestManager* hot_suggest_mgr_;
};

} /*namespace hot_suggest_logic*/

#endif //BACKSVC_HOT_SUGGEST_LOGIC_H
