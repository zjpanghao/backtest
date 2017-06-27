//  Copyright (c) 2015-2016 The KID Authors. All rights reserved.
//  Created on: 2016.8.18 Author: kerry

#include "kensho_kafka.h"
#include "logic/logic_comm.h"
#include "baseconfig/base_config.h"
#define KAFKA_CONFIG_FILE "./plugins/kenshosvc/kafka.config"
namespace kensho_logic {

KenshoKafka::KenshoKafka() {
  Init();
}

KenshoKafka::~KenshoKafka() {
  kafka_producer_.Close();
  kafka_consumer_.Close();
  DestPatitionConsumer();
}

void KenshoKafka::Init() {
  plugin_pub::BaseConfig ini_config;
  if (!ini_config.LoadFile(KAFKA_CONFIG_FILE)) {    
    LOG_ERROR2("DbMysql::Init,加载配置文件 [%s]失败", KAFKA_CONFIG_FILE);
    return;
  }
  std::string consumer_server = 
      ini_config.GetParam<std::string>("consumer", "server");
  std::string consumer_topic =
      ini_config.GetParam<std::string>("consumer", "topic");
  std::string consumer_group =
      ini_config.GetParam<std::string>("consumer", "group");
  std::string producer_server = 
      ini_config.GetParam<std::string>("producer", "server");
  std::string producer_topic = 
      ini_config.GetParam<std::string>("producer", "topic");
  InitPorducer(producer_topic, producer_server);
  InitConsumer(consumer_topic, consumer_server, consumer_group);
}

void KenshoKafka::DestPatitionConsumer() {
  std::list<kafka_consumer*>::iterator it = kafka_consumcer_list_.begin();
    for (; it != kafka_consumcer_list_.end(); it++) {
      kafka_consumer* k = (*it);
      if(k) {
        k->Close();
        delete k;
        k = NULL;
      }
    }
}

void KenshoKafka::InitPorducer(const std::string &topic, 
                               const std::string &server) {
  if (PRODUCER_INIT_SUCCESS
      != kafka_producer_.Init(
          0,
          topic.c_str(),
          server.c_str(),
          NULL))
    LOG_ERROR("producer backtesting_filter init failed");
  else
    LOG_ERROR("producer backtesting_filter init success");
}

void KenshoKafka::InitMultiplePatitionConsumer(int32 n) {
  kafka_consumer* kafka = NULL;
  while (n > 0) {
    kafka =
        InitConsumer(
            (n - 1),
            "backtesting_result",
            "192.168.1.74:9092",
            "one",
            NULL);
    if (kafka != NULL)
      kafka_consumcer_list_.push_back(kafka);
    n--;
  }
}

kafka_consumer* KenshoKafka::InitConsumer(const int in_partition,
                                          const char* in_topic,
                                          const char* in_brokers,
                                          const char* group_id,
                                          MsgConsume msg_consume) {
  kafka_consumer* kafka_consumer_unit = new kafka_consumer;
  if (CONSUMER_INIT_SUCCESS
      != kafka_consumer_unit->Init(in_partition, in_topic, in_brokers, group_id,
                                   msg_consume))
    LOG_MSG2("partition %d kafka consumer backtesting_result init failed",
        in_partition);
  else {
    LOG_MSG2("partition %d kafka consumer backtesting_result init success",
        in_partition);
  }
  return kafka_consumer_unit;
}

void KenshoKafka::InitConsumer(const std::string &topic,
                               const std::string &server,
                               const std::string &group_id) {
  if (CONSUMER_INIT_SUCCESS
      != kafka_consumer_.Init(
          0,
          topic.c_str(),
          server.c_str(),
          group_id.c_str(),
          NULL))
    LOG_ERROR("kafka consumer backtesting_result init failed");
  else
    LOG_MSG("kafka consumer backtesting_result init success");
}

bool KenshoKafka::AddKafkaConditionInfo(base_logic::DictionaryValue* value) {

  /*{"id":560, "attrid":14, "depth":3, "cur_depth":2, "method":2, "url":"http://tech.caijing.com.cn/index.html"}
   * */
  int re = PUSH_DATA_SUCCESS;
  const char *tmp = "{}";
  // test kafka
  re = kafka_producer_.PushData(tmp, strlen(tmp));
  if (re != PUSH_DATA_SUCCESS) {
    LOG_ERROR("SEND data error");
    return false;
  }
  re = kafka_producer_.PushData(value);
  if (PUSH_DATA_SUCCESS == re)
    return true;
  else {
    LOG_ERROR("kafka producer send data failed");
    return false;
  }
}

bool KenshoKafka::FetchMultiplePatitionKenshoResult(
    std::list<base_logic::DictionaryValue*>& list) {
  std::set < std::string > data_list;

  std::list<kafka_consumer*>::iterator it = kafka_consumcer_list_.begin();
  for (; it != kafka_consumcer_list_.end(); it++) {
    kafka_consumer* k = (*it);
    FetchPatitionKenshoResult(k, data_list);
  }
  base_logic::ValueSerializer* engine = base_logic::ValueSerializer::Create(
      base_logic::IMPL_JSON);
  for (std::set<std::string>::iterator it = data_list.begin();
      it != data_list.end(); it++) {
    std::string data = *it;
    int error_code = 0;
    std::string error_str;
    base_logic::DictionaryValue* value = (base_logic::DictionaryValue*) engine
        ->Deserialize(&data,&error_code, &error_str);
    if (0 != error_code || NULL == value)
      continue;
    list.push_back(value);
  }
  base_logic::ValueSerializer::DeleteSerializer(base_logic::IMPL_JSON, engine);
  return true;
}

void KenshoKafka::FetchPatitionKenshoResult(kafka_consumer* k,
                                std::set<std::string>& data_list) {
  std::string data;
  for (int i = 0; i < 1; i++) {
    int pull_re = k->PullData(data);
    if (PULL_DATA_TIMEOUT == pull_re) {
      // LOG_MSG2("consumer get url timeout,pull_re=%d", pull_re);
      break;
    }
    data_list.insert(data);
  }
}

bool KenshoKafka::FetchBatchKenshoResult(
    std::list<base_logic::DictionaryValue*>& list) {
  std::set < std::string > data_list;
  std::string data;

  for (int i = 0; i < 1; i++) {
    int pull_re = kafka_consumer_.PullData(data);
    if (PULL_DATA_TIMEOUT == pull_re) {
      // LOG_MSG2("consumer get url timeout,pull_re=%d", pull_re);
      break;
    }
    data_list.insert(data);
  }
  base_logic::ValueSerializer* engine = base_logic::ValueSerializer::Create(
      base_logic::IMPL_JSON, &data);
  for (std::set<std::string>::iterator it = data_list.begin();
      it != data_list.end(); it++) {
    std::string data = *it;
    int error_code = 0;
    std::string error_str;
    base_logic::DictionaryValue* value = (base_logic::DictionaryValue*) engine
        ->Deserialize(&error_code, &error_str);
    if (0 != error_code || NULL == value)
      continue;
    list.push_back(value);
  }

  base_logic::ValueSerializer::DeleteSerializer(base_logic::IMPL_JSON, engine);
  return true;
}

}
