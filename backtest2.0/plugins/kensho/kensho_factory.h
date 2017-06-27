//  Copyright (c) 2015-2016 The BACKSVC Authors. All rights reserved.
//  Created on: 2016年8月25日 Author: kerry
#ifndef BACKSVC_KENSHO_FACTORY_H__
#define BACKSVC_KENSHO_FACTORY_H__

#include "kensho_sentence.h"
#include "kensho_result.h"
#include "kensho/kensho_kafka.h"
#include "kensho/kensho_db.h"
#include "net/proto_buf.h"
#include "net/packet_process.h"
#include "basic/basictypes.h"
#include "config/config.h"

#define TIME_KENSHO_RESULT                          10000
#define TIME_UPDATE_BACKSHARE                       10001
#define TIME_UPDATE_STOCK                           10002
#define TIME_UPDATE_BACKSHARE_LONG_PERIOD           10003
#define TIME_KENSHO_RESULT1                         10004
#define TIME_KENSHO_RESULT2                         10005
#define TIME_KENSHO_RESULT3                         10006
#define TIME_KENSHO_RESULT4                         10007
#define TIME_KENSHO_RESULT5                         10008
#define TIME_UPDATE_SENTENCE_HOT                    10009
namespace kensho_logic {

class KenshoFactory {
 public:
  KenshoFactory();
  virtual ~KenshoFactory();
 public:
  void Init();
  void Dest();
 private:
  static KenshoFactory* instance_;
 public:
  static KenshoFactory* GetInstance();
  static void FreeInstance();

  void InitParam(config::FileConfig* config);

  void InitBS(back_share::BSEngine* bs_engine);

  void TimeEvent(int opcode, int time);

 public:
  bool OnBTSentenceReq(const int socket, base_logic::DictionaryValue* dict,
                       back_logic::PacketHead* packet);

  bool OnBTResultReq(const int socket, base_logic::DictionaryValue* dict,
                       back_logic::PacketHead* packet);

  bool OnBTYield(const int socket, base_logic::DictionaryValue* dict,
                         back_logic::PacketHead* packet);
 public:
  void SendPacket(const int socket, back_logic::PacketHead* packet,
                  back_logic::AttachField* attach, const int16 operator_code,
                  const int8 type);

  void SendHeader(const int socket, back_logic::AttachField* attach,
                  const int16 operator_code, const int8 type);

  kensho_logic::KenshoResultManager* kensho_result() {
    return kensho_result_;
  }

 private:
  base_logic::Value* ParserSondition(std::string& sondition);

  void UpdateKenshoResult();

  void UpdateBullish();

  void UpdateSentenceHot();

 private:
  kensho_logic::KenshoSentenceManager* kensho_sentence_;
  kensho_logic::KenshoResultManager* kensho_result_;
  back_logic::json_packet::PacketProcess* packet_json_;
  back_logic::jsonp_packet::PacketProcess* packet_jsonp_;
  back_share::BSEngine* bs_engine_;
  KenshoKafka* kensho_kakfa_;
  kensho_logic::KenshoDB*  kensho_db_;

};
}
#endif
