#ifndef _REDIS_LINK_H_
#define _REDIS_LINK_H_

#include <pthread.h>
#include <queue>
#include <string>
#include "storage/storage.h"
#include "config/config.h"
#include "logic/logic_comm.h"

namespace login {

static std::string REDIS_ENGINE_PATH = "./plugins/usersvc/user_config.xml";

class MutexLockGuard {
   public:
    explicit MutexLockGuard(pthread_mutex_t &mutex)
      : mutex_(mutex) {
      pthread_mutex_lock(&mutex_);
    }

    ~MutexLockGuard() {
      pthread_mutex_unlock(&mutex_);
    }

   private:
    const MutexLockGuard & operator=(const MutexLockGuard &);

    pthread_mutex_t &mutex_;
};

class RedisEngine {
 public:
  RedisEngine(int link_num = 6) {
    config::FileConfig* config = config::FileConfig::GetFileConfig();
	if (config == NULL) {
      LOG_DEBUG2("%s", "config is NULL");
	  return;
	}
	config->redis_list_.clear();
	bool r = config->LoadConfig(REDIS_ENGINE_PATH);

    addrlist_ = config->redis_list_;
    InitQueue(link_num);

    pthread_mutex_init(&mutex_lock_, NULL);
  }

  ~RedisEngine() {
    while (link_queue_.size() > 0) {
      base_storage::DictionaryStorageEngine* redis_engine = link_queue_.front();
      redis_engine->Release();
      delete redis_engine;
      link_queue_.pop();
    }
  }

  base_storage::DictionaryStorageEngine* GetRedisEngine();
  void PushRedisEngine(base_storage::DictionaryStorageEngine* redis_engine);

 private:
  void InitQueue(int link_num);
  std::queue<base_storage::DictionaryStorageEngine*> link_queue_;
  std::list<base::ConnAddr> addrlist_;
  pthread_mutex_t mutex_lock_;
};



class EngineManage {
 public:
  static base_storage::DictionaryStorageEngine* const GetRedisEngine() { return engine_.GetRedisEngine(); }

  static void PushRedisEngine(base_storage::DictionaryStorageEngine* redis_engine) {
    engine_.PushRedisEngine(redis_engine);
  }

 private:
  static RedisEngine engine_;
};

class AutoEngine {
public:
 AutoEngine() {
   redis_engine_ = EngineManage::GetRedisEngine();
 }
 ~AutoEngine() {
   EngineManage::PushRedisEngine(redis_engine_);
 }

 base_storage::DictionaryStorageEngine * const GetRedisEngine() {
   return redis_engine_;
 }

private:
  base_storage::DictionaryStorageEngine *redis_engine_;
};

}

#endif
