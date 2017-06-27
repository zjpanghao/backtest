//
// Created by harvey on 2016/8/31.
//

#ifndef BACKSVC_HOT_SUGGEST_FACTORY_H
#define BACKSVC_HOT_SUGGEST_FACTORY_H

#include "net/packet_process.h"
#include "basic/basictypes.h"
#include "config/config.h"
#include "thread/base_thread_lock.h"

#include <map>

namespace hot_suggest_logic {

class HotSuggestFactory {
 public:
  ~HotSuggestFactory();
  static HotSuggestFactory* GetPtr();
  static HotSuggestFactory& GetRef();

 public:
  bool OnRequestAllSuggest(const int socket,
                           base_logic::DictionaryValue* dict,
                           back_logic::PacketHead* packet);

 public:
  void SendPacket(const int socket, back_logic::PacketHead* packet,
                  back_logic::AttachField* attach,
                  const int16 operator_code, const int8 type);

 private:
  HotSuggestFactory();
  DISALLOW_COPY_AND_ASSIGN(HotSuggestFactory);

 private:
  struct threadrw_t* lock_;
  back_logic::json_packet::PacketProcess* packet_json_;
  back_logic::jsonp_packet::PacketProcess* packet_jsonp_;
};

} /*namespace hot_suggest_logic*/

#endif //BACKSVC_HOT_SUGGEST_FACTORY_H
