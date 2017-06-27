// Copyright (c) 2015-2016 The BACKSVC Authors. All rights reserved.
// Created on: 2016年10月12日 Author: luyao

#include "token.h"

namespace token {

uint32_t key[4] = {0x04, 0x03, 0x02, 0x01};   

void NumToChar(void *d, size_t l, std::string &token) {                                                                                                                 
  std::stringstream os; 
  char *p = reinterpret_cast<char *>(d);
  int temp;
  for (int i = 0; i < l; ++i) {
    temp = p[i];
    os << temp << ",";
  }
  token = os.str();
}
 
size_t CharToNum(void **d, std::string &token) {
  ContainerStr out;
  SeparatorStr<ContainerStr>(token, ',', &out, true);
 
  *d = reinterpret_cast<void *>(malloc(out.size()));
  char *p = reinterpret_cast<char*>(*d);
  for (int i = 0; i < out.size(); ++i) {
    p[i] = atoi(out[i].c_str());
  }
  return out.size();
}

void encrypt(uint32_t v[], uint32_t k[]) {                                                                                                                              
  uint32_t v0 = v[0], v1 = v[1], sum = 0, i;
  uint32_t delta = 0x9e3779b9;
  uint32_t k0 = k[0], k1 = k[1], k2 = k[2], k3 = k[3];
  for (i=0; i < 32; i++) {
    sum += delta;
    v0 += ((v1<<4) + k0) ^ (v1 + sum) ^ ((v1>>5) + k1);
    v1 += ((v0<<4) + k2) ^ (v0 + sum) ^ ((v0>>5) + k3);
  }
  v[0] = v0; 
  v[1] = v1; 
}

void decrypt(uint32_t v[], uint32_t k[]) {
  uint32_t v0 = v[0], v1 = v[1], sum = 0xC6EF3720, i;
  uint32_t delta = 0x9e3779b9;
  uint32_t k0 = k[0], k1 = k[1], k2 = k[2], k3 = k[3];
  for (i = 0; i < 32; i++) {
    v1 -= ((v0<<4) + k2) ^ (v0 + sum) ^ ((v0>>5) + k3);
    v0 -= ((v1<<4) + k0) ^ (v1 + sum) ^ ((v1>>5) + k1);
    sum -= delta;
  }
  v[0] = v0; 
  v[1] = v1; 
}

bool StrEn(struct tea_data *td) {
  if (td == NULL || td->d == NULL) {
    return false;
  }
 
  if (td->l % 8 != 0) {
    size_t new_l = ((td->l - 1) / 8 + 1) * 8;
    void *new_d = reinterpret_cast<void *>(malloc(new_l));
    memset(new_d, 0, new_l);
 
    for (size_t i = 0; i < td->l; ++i) {
      (reinterpret_cast<char *>(new_d))[i] = \ 
      (reinterpret_cast<char *>(td->d))[i];
    }   
 
    td->d = reinterpret_cast<unsigned char *>(new_d);
    td->l = new_l;
  }
 
  uint32_t *p = reinterpret_cast<uint32_t*>(td->d);
  size_t l = td->l / 4;
  for (int i = 0; i < l; i += 2) {
    encrypt(&p[i], key);
  }
  return true;                                                                                                                                                          
}

bool StrDe(struct tea_data *td) {
  if (td == NULL || td->d == NULL || td->l % 8 != 0) {
    return false;
  }
 
  uint32_t *p = reinterpret_cast<uint32_t*>(td->d);
  size_t l = td->l / 4;
 
  for (int i = 0; i < l; i += 2) {
    decrypt(&p[i], key);
  }
  return true;
} 

// xml
void Init(std::list<base::ConnAddr> *const addrlist, size_t num) {
  base_dic::RedisPool::Init(*addrlist, num);
}

// xml
void Release() {
  base_dic::RedisPool::Dest(); 
}
// 更新redis中token
void SaveUserToken(int64 user_id, std::string const& user_token) {
  base_dic::AutoDicCommEngine auto_engine;
  base_storage::DictionaryStorageEngine *redis = auto_engine.GetDicEngine();
  if (NULL == redis) {
    return;
  }
  std::string table_name = "bk_user_token";
  std::string key_name;
  std::stringstream ss;
  ss << user_id;
  ss >> key_name;
  ss.clear();
  ss.str("");

  redis->SetHashElement(table_name.c_str(), key_name.c_str(), \
      key_name.length(), user_token.c_str(), user_token.length());
  LOG_DEBUG2("SaveUserToken, user_id:%lld, token:%s", user_id, user_token.c_str());
}

std::string GetUserToken(int64 user_id) {
  std::string token_saved = "0";
  // TODO : 
  // get user's token from redis

  base_dic::AutoDicCommEngine auto_engine;
  base_storage::DictionaryStorageEngine *redis = auto_engine.GetDicEngine();

  if (NULL == redis) {
    LOG_MSG2("%s", "Get token connection NULL");
    return token_saved;
  }
  std::string table_name = "bk_user_token";
  std::string key_name = "0";
  std::stringstream ss;
  ss << user_id;
  ss >> key_name;
  ss.str("");
  ss.clear();

  char * val = NULL;
  size_t val_len = 0;
  if (false == redis->GetHashElement(table_name.c_str(), key_name.c_str(), \
      key_name.length(), &val, &val_len)) {
    LOG_MSG2("%s", "GetHashElement error");
    return token_saved;
  }
  if (NULL != val) {
    token_saved = val;
    free(val);
  }
  return token_saved;
}

std::string GetToken(int64 user_id, std::string &token) {
  std::stringstream os;
  std::string cur_token;
  std::string temp;
  os.str("");
  os << user_id;
  os << ",";
  os << time(NULL);
  cur_token = os.str();

  int len = ((cur_token.length() - 1) / 8 + 1) * 8;
  char *in = reinterpret_cast<char*>(malloc(len));
  memset(in, 0, len);
  strncpy(in, cur_token.c_str(), cur_token.length());

  struct tea_data td;
  td.d = reinterpret_cast<void *>(in);
  td.l = len;

  StrEn(&td);
  NumToChar(td.d, td.l, token);
  free(in);
  return token;
}

bool CheckToken(int64 user_id, std::string &token) {
  std::string token_saved = GetUserToken(user_id);
  if (token != token_saved) {
    LOG_DEBUG2("单点登录失败：redis_token:%s, cur_token:%s", token_saved.c_str(), token.c_str()); 
    return false;
  }
  struct tea_data td;
  td.l = CharToNum(&td.d, token);
  StrDe(&td);

  std::string origin_token("");
  for (int i = 0; i < td.l; ++i) {
    origin_token.append(1, (reinterpret_cast<char*>(td.d))[i]);
  }
  origin_token.append(1, '\0');

  std::string::size_type separator_pos = origin_token.find(',', 0);
  std::string origin_id = origin_token.substr(0, separator_pos);

  std::stringstream os;
  os.str("");
  os << origin_token.substr(separator_pos + 1, origin_token.length());
  int64 origin_time;
  os >> origin_time;

  os.str("");
  os << user_id;
  std::string current_id = os.str();

  // LOG_DEBUG2("\n\norigin token: %s,%d\n\n", origin_id.c_str(), origin_time);

  int64 current_time = time(NULL);

  const int TOKEN_SURVIVE_TIME = 86400;
  if (origin_id == current_id && (current_time - origin_time <= TOKEN_SURVIVE_TIME)) {
    return true;
  }
  return false;
}

}
