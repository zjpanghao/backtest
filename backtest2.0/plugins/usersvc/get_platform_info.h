//  Copyright (c) 2015-2015 The restful Authors. All rights reserved.
//  Created on: 2015/11/24 Author: jiaoyongqing
#ifndef _HOME_RUNNER_NORTHSEA_RESTFUL_PLUGINS_USERSVC_GET_PLATFORM_INFO_H_
#define _HOME_RUNNER_NORTHSEA_RESTFUL_PLUGINS_USERSVC_GET_PLATFORM_INFO_H_

#include <vector>
#include <string>

#include "usersvc/user_comm_head.h"
#include "usersvc/base.h"

namespace get_platform_info {
class GetPlatFormInfo : public base::Base{
 public:
    GetPlatFormInfo() :socket_(-1) {
    vec_platform_id_.clear();
    vec_platform_name_.clear();
  }

  GetPlatFormInfo(const std::string &jsonp_str, int socket) \
    :jsonp_str_(jsonp_str), \
     socket_(socket) {
      vec_platform_id_.clear();
      vec_platform_name_.clear();
    }

 public:
  bool GetData();
  void SendMessage();

 private:
  bool DataInMySql(time_t start_time, time_t end_time);

  bool DataInRedis(time_t start_time, time_t end_time) {return true;}

  bool DataBetweenRedisAndMySql(time_t start_time, \
                                  time_t end_time, \
                               time_t middle_time) {return true;}

 private:
  std::vector<int> vec_platform_id_;
  std::vector<std::string> vec_platform_name_;
  std::string jsonp_str_;
  int socket_;
};

}  //  namespace get_platform_info

#endif  //  _HOME_RUNNER_NORTHSEA_RESTFUL_PLUGINS_USERSVC_GET_PLATFORM_INFO_H_
