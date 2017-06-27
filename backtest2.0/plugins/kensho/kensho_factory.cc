//  Copyright (c) 2015-2016 The BACKSVC Authors. All rights reserved.
//  Created on: 2016年8月25日 Author: kerry
#include "kensho_factory.h"
#include "logic/logic_comm.h"
#include "kensho/kensho_common.h"
#include "kensho_proto_buf.h"
#include "operator_code.h"
#include "basic/scoped_ptr.h"
#include "logic/base_values.h"
#include "basic/radom_in.h"

namespace kensho_logic {

KenshoFactory* KenshoFactory::instance_ = NULL;

KenshoFactory*
KenshoFactory::GetInstance() {
  if (instance_ == NULL)
    instance_ = new KenshoFactory();
  return instance_;
}

void KenshoFactory::FreeInstance() {
  delete instance_;
  instance_ = NULL;
}

KenshoFactory::KenshoFactory() {
  Init();
}

KenshoFactory::~KenshoFactory() {
  if (packet_json_) {
    delete packet_json_;
    packet_json_ = NULL;
  }
  if (packet_jsonp_) {
    delete packet_jsonp_;
    packet_jsonp_ = NULL;
  }

  if (kensho_db_) {
    delete kensho_db_;
    kensho_db_ = NULL;
  }
  if (kensho_kakfa_) {
    delete kensho_kakfa_;
    kensho_kakfa_ = NULL;
  }
  kensho_logic::KenshoSentenceEngine::FreeSentenceManager();
}

void KenshoFactory::Init() {
  kensho_kakfa_ = new KenshoKafka();
//  kensho_kakfa_->InitMultiplePatitionConsumer(3);
  packet_json_ = new back_logic::json_packet::PacketProcess();
  packet_jsonp_ = new back_logic::jsonp_packet::PacketProcess();
  kensho_sentence_ = kensho_logic::KenshoSentenceEngine::GetSentenceManager();
  kensho_result_ = kensho_logic::KenshoResultEngine::GetResultManager();
}

void KenshoFactory::InitBS(back_share::BSEngine* bs_engine) {
  bs_engine_ = bs_engine;
  kensho_sentence_->Init(bs_engine);
  kensho_result_->Init(bs_engine);
}

void KenshoFactory::UpdateKenshoResult() {
  std::list<base_logic::DictionaryValue*> list;
  //kensho_kakfa_->FetchMultiplePatitionKenshoResult(list);
  kensho_kakfa_->FetchBatchKenshoResult(list);
  if (list.size() > 0)
    kensho_result_->UpdateKenshoResult(list);
}
void KenshoFactory::UpdateBullish() {
  kensho_result_->UpdateKenshoStock();
}

void KenshoFactory::InitParam(config::FileConfig* config) {
  kensho_db_ = new kensho_logic::KenshoDB(config);
  kensho_result_->InitDB(kensho_db_);
  UpdateBullish();
}

void KenshoFactory::Dest() {

}

void KenshoFactory::UpdateSentenceHot() {
  int id;
  int hot;
  int rc = kensho_sentence_->PopSentenceHotForUpdate(&id, &hot);
  if (rc == -1)
    return;
  int count = 0;
  while (true) {
    if (count++ > 3)
      break;
    LOG_MSG2("Try update %d %d", id, hot);
    if (kensho_db_->UpdateSentenceHot(id, hot))
      break;
  }
}

void KenshoFactory::TimeEvent(int opcode, int time) {
  switch (opcode) {
    case TIME_KENSHO_RESULT1: 
    case TIME_KENSHO_RESULT2: 
    case TIME_KENSHO_RESULT3: 
    case TIME_KENSHO_RESULT4: 
    case TIME_KENSHO_RESULT5: 
    case TIME_KENSHO_RESULT: {
      UpdateKenshoResult();
      break;
    }
    case TIME_UPDATE_BACKSHARE:{
      bs_engine_->SortParamByHot();
      bs_engine_->SortSentenceByHot();
      break;
    }
    case TIME_UPDATE_STOCK: {
      UpdateBullish();
      break;
    }
    case TIME_UPDATE_SENTENCE_HOT: {
      UpdateSentenceHot();
      break;
    }
    case TIME_UPDATE_BACKSHARE_LONG_PERIOD: {
      bs_engine_->UpdateShareLongPeriodData();
      break;
    }
    default:
      break;

  }
}

bool KenshoFactory::OnBTResultReq(const int socket,
                                  base_logic::DictionaryValue* dict,
                                  back_logic::PacketHead* packet) {
  kensho_logic::net_request::BTResult* result =
      new kensho_logic::net_request::BTResult;
  result->set_http_packet(dict);
  kensho_logic::net_reply::BTResult reply_result;
  //获取缓存
  kensho_result_->OnGetResultBySession(result->uid(), result->session_id(),
                                       result->pos(),result->count(),
                                       &reply_result);

  SendPacket(socket, &reply_result, result->attach_field(),
             KENSHO_BT_RESULT_RLY, back_logic::KENSHO_TYPE);
  return true;
}

bool KenshoFactory::OnBTYield(const int socket, base_logic::DictionaryValue* dict,
                         back_logic::PacketHead* packet) {

  kensho_logic::net_request::BTKenshoYield* yield =
      new kensho_logic::net_request::BTKenshoYield;
  yield->set_http_packet(dict);
  int64 session = yield->session_id();
  int64 uid = yield->uid();
  std::string yield_str;
  kensho_result_->OnGetYield(uid,session,yield_str);
  if (yield_str.empty())
    SendHeader(socket,packet->attach_field(),KENSHO_BT_YIELD_RLY,back_logic::KENSHO_TYPE);
  else
    base_logic::LogicComm::SendFull(socket, yield_str.c_str(), yield_str.length());
  return true;
}

bool KenshoFactory::OnBTSentenceReq(const int socket,
                                    base_logic::DictionaryValue* dict,
                                    back_logic::PacketHead* packet) {
  kensho_logic::net_request::BTSondition* sondition =
      new kensho_logic::net_request::BTSondition;
  sondition->set_http_packet(dict);
  std::string sondition_content = sondition->sonditions();
  base_logic::ListValue* value = (base_logic::ListValue*) ParserSondition(
      sondition_content);
  if (value == NULL)
    return false;


  int64 bt_session = base::SysRadom::GetInstance()->GetRandomID();
  // makesure the randomid not equal as time flows
  bt_session &= 0xffffffff;
  bt_session |= time(NULL) << 32;
  base_logic::DictionaryValue rdict;
  rdict.SetBigInteger(L"uid", sondition->uid());
  rdict.SetBigInteger(L"session", bt_session);
  LOG_MSG2("Create sessionid for %ld  id %ld  time:%u", sondition->uid(), bt_session, time(NULL));
  rdict.SetBigInteger(L"base_session", sondition->base_sessionid());
  rdict.SetBigInteger(L"search_type", BACKTEST_KAFKA_MESSAGE_ENHANCE_TYPE);
  rdict.SetString(L"start_time", sondition->start_time());
  rdict.SetString(L"end_time", sondition->end_time());
  kensho_sentence_->CreateBatchBTSentence(value, &rdict);
  kensho_kakfa_->AddKafkaConditionInfo(&rdict);

  //返回sessionid 用于请求
  net_reply::BTSondition reply_btsondition;
  reply_btsondition.set_bt_session(bt_session);

  SendPacket(socket, &reply_btsondition, sondition->attach_field(),
  KENSHO_BT_SENTENCE_RLY,
             back_logic::KENSHO_TYPE);
  return true;
}

void KenshoFactory::SendPacket(const int socket, back_logic::PacketHead* packet,
                               back_logic::AttachField* attach,
                               const int16 operator_code, const int8 type) {
  packet->set_operator_code(operator_code);
  packet->set_type(type);
  if (attach != NULL && attach->format() == "jsonp") {
    packet->attach_field()->set_callback(attach->callback());
    packet_jsonp_->PackPacket(socket, packet->packet());
  } else
    packet_json_->PackPacket(socket, packet->packet());
}

void KenshoFactory::SendHeader(const int socket,
                               back_logic::AttachField* attach,
                               const int16 operator_code, const int8 type) {
  back_logic::PacketHead *header = new back_logic::PacketHead();
  SendPacket(socket, header, attach, operator_code, type);
  if (header) {
    delete header;
    header = NULL;
  }
}

base_logic::Value* KenshoFactory::ParserSondition(std::string& sondition) {

  scoped_ptr<base_logic::ValueSerializer> serializer(
      base_logic::ValueSerializer::Create(base_logic::IMPL_JSON));
  int error_code;
  std::string error_str;
  return serializer.get()->Deserialize(&sondition, &error_code, &error_str);
}

}
