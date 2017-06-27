#include "usersvc/redis_link.h"
#include "storage/storage.h"

namespace login {

RedisEngine EngineManage::engine_;

void RedisEngine::InitQueue(int link_num) {
  for (int i = 0; i < link_num; ++i) {
    base_storage::DictionaryStorageEngine* redis_engine =
      base_storage::DictionaryStorageEngine::Create(base_storage::IMPL_RADIES);
    bool r = redis_engine->Connections(addrlist_);
    if (!r) {
      LOG_DEBUG2("message %s", "redis_engine_ connect error");
      continue;
    }

    link_queue_.push(redis_engine);
  }
}

base_storage::DictionaryStorageEngine* RedisEngine::GetRedisEngine() {
  MutexLockGuard lock(mutex_lock_);
  if (link_queue_.size() != 0) {
    base_storage::DictionaryStorageEngine* redis_engine = link_queue_.front();
    link_queue_.pop();

    if (!redis_engine->CheckConnect()) {
      LOG_DEBUG("redis lost connection");
      if(!redis_engine->Connections(addrlist_)) {
    	LOG_DEBUG2("message %s", "redis_engine_ connect error");
        link_queue_.push(redis_engine);
        return NULL;
      }
    }
    return redis_engine;
  }
  return NULL;
}

void RedisEngine::PushRedisEngine(base_storage::DictionaryStorageEngine* redis_engine) {
  MutexLockGuard lock(mutex_lock_);
  link_queue_.push(redis_engine);
}

}
