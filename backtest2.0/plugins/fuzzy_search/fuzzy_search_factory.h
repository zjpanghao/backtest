//  Copyright (c) 2015-2016 The BACKSVC Authors. All rights reserved.
//  Created on: 2016年9月1日 Author: tangtao

#ifndef BACKSVC_FUZZY_SEARCH_FACTORY_H__
#define BACKSVC_FUZZY_SEARCH_FACTORY_H__

#include "net/proto_buf.h"
#include "net/packet_process.h"
#include "basic/basictypes.h"
#include "config/config.h"
class threadrw_t;
namespace fuzzy_search_logic {

class FuzzysearchFactory {
 public:
  FuzzysearchFactory();
  virtual ~FuzzysearchFactory();
 public:
  void Init();
  void Dest();
 private:
  static FuzzysearchFactory *instance_;
 public:
  static FuzzysearchFactory* GetInstance();
  static void FreeInstance();

  void InitParam(config::FileConfig *config);
 public:
  bool OnBTContitionSentenceCheckSearch(const int socket,
                                        base_logic::DictionaryValue *dict,
                                        back_logic::PacketHead *packet);
 public:
  void SendPacket(const int socket,                 \
                  back_logic::PacketHead *packet,   \
                  back_logic::AttachField* attach,  \
                  const int16 operator_code,        \
                  const int8 type);

  void SendHeader(const int socket,                 \
                  back_logic::AttachField *attach,  \
                  const int16 operator_code,        \
                  const int8 type);
 private:
  threadrw_t *m_lock_;
  back_logic::json_packet::PacketProcess *packet_json_;
  back_logic::jsonp_packet::PacketProcess *packet_jsonp_;
};

}   //namespace fuzzy_search_logic
#endif
