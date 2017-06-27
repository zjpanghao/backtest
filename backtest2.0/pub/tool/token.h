// Copyright (c) 2015-2016 The BACKSVC Authors. All rights reserved.
// Created on: 2016年10月12日 Author: luyao

#ifndef _BACKSVC_PUB_TOKEN_H_
#define _BACKSVC_PUB_TOKEN_H_

#include <string>
#include <sstream>
#include <stdlib.h>
#include <map>
#include <vector>
#include <list>
#include <memory.h>

#include "base/dic/base_dic_redis_auto.h"

namespace token {

typedef long int64;
typedef unsigned int uint32_t;
typedef std::map<std::string, int64> MapStrToInt64; 
typedef std::vector<std::string> ContainerStr;   
 
struct tea_data {                                                                                                                                                       
  void *d; 
  size_t l;
};

void NumToChar(void *d, size_t l, std::string &token);
size_t CharToNum(void **d, std::string &token);
void encrypt(uint32_t v[], uint32_t k[]);
void decrypt(uint32_t v[], uint32_t k[]);
bool StrEn(struct tea_data * td);
bool StrDe(struct tea_data *td);

// xml
void Init(std::list<base::ConnAddr> *const addrlist, size_t num = 4);
void Release();

std::string GetUserToken(int64 user_id);
void SaveUserToken(int64 user_id, std::string const& user_token);
std::string GetToken(int64 user_id, std::string &token);
bool CheckToken(int64 user_id, std::string &token);


//  以separator为分隔符拆分字符串，拆分后放到列表l中,
//  repeat = true:列表里允许重复，repeat = false:列表里不允许重复
template<class T>
void SeparatorStr(const std::string &instr, char separator, T *l, bool repeat = true) {
  std::string str(instr);
 
  if (str == "") return;
    
  str = std::string(1, separator) + str; 
  if (str[str.length() - 1] != separator) {
    str = str + std::string(1, separator);
  }
 
  MapStrToInt64 map_repeat;
  map_repeat.clear();
  MapStrToInt64::iterator tmp_it;
  std::string value;
  std::string::size_type start = 0;
  std::string::size_type end = str.find(separator, start + 1);
  while (end != std::string::npos) {
    if (end - start == 1) {
      value = "";
    } else {
      value = str.substr(start + 1, end - (start + 1));
    }
  
    start = end;
    end = str.find(separator, start + 1);
  
    if (repeat == false) {
      tmp_it = map_repeat.find(value);
      if (tmp_it == map_repeat.end()) {
        map_repeat[value] = 1;
      } else {
        continue;
      }
    }
    l->push_back(value);
  }
}

}
#endif // _BACKSVC_PUB_TOKEN_H_
