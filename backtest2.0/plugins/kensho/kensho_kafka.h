//  Copyright (c) 2015-2016 The BACKSVC Authors. All rights reserved.
//  Created on: 2016年8月25日 Author: kerry
#ifndef BACK_KENSHO_KAFKA_H__
#define BACK_KENSHO_KAFKA_H__

#include <list>
#include <set>
#include "queue/kafka_producer.h"
#include "queue/kafka_consumer.h"

namespace kensho_logic {

class KenshoKafka {
 public:
  KenshoKafka();
  virtual ~KenshoKafka();

 private:
  void Init();
  void InitPorducer(const std::string &topic,
                    const std::string &server);
  void InitConsumer(const std::string &topic,
                    const std::string &server,
                    const std::string &group_id);
  kafka_consumer* InitConsumer(const int in_partition,
                    const char* in_topic,
                    const char* in_brokers,
                    const char* group_id,
                    MsgConsume msg_consume);
  void FetchPatitionKenshoResult(kafka_consumer* k,
                                  std::set < std::string >& data_list);

  void DestPatitionConsumer();

 public:
  void InitMultiplePatitionConsumer(int32 n);
 public:
  bool AddKafkaConditionInfo(base_logic::DictionaryValue* value);
  bool FetchBatchKenshoResult(std::list<base_logic::DictionaryValue*>& list);
  bool FetchMultiplePatitionKenshoResult(std::list<base_logic::DictionaryValue*>& list);
 private:
  kafka_producer     kafka_producer_;
  kafka_consumer     kafka_consumer_;
  std::list<kafka_consumer*> kafka_consumcer_list_;
};
}
#endif


