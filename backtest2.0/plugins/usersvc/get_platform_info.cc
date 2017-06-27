//  Copyright (c) 2015-2015 The restful Authors. All rights reserved.
//  Created on: 2015/11/24 Author: jiaoyongqing
#include "usersvc/get_platform_info.h"

#include <string>

#include "logic/logic_comm.h"
#include "usersvc/login_logic_unit.h"
#include "usersvc/user_comm_head.h"
#include "usersvc/db_comm.h"

namespace get_platform_info {
  bool GetPlatFormInfo::GetData() {
    DataInMySql(0, 0);
    return true;
  }

  bool GetPlatFormInfo::DataInMySql(time_t start_time, time_t end_time) {
    db::DbSql sql;
    sql.GetPlatFormInfo(&vec_platform_id_, &vec_platform_name_);
    error_info_ = sql.get_error_info();
    if (error_info_ != 0) {
      return false;
    }
    return true;
  }

  void GetPlatFormInfo::SendMessage() {
    netcomm_send::SendPlatformInfo platform;
    std::vector<int>::iterator id_it = vec_platform_id_.begin();
    std::vector<std::string>::iterator name_it = vec_platform_name_.begin();
    for (; id_it != vec_platform_id_.end(); ++id_it, ++name_it) {
      NetBase *platform_info = new NetBase();
      platform_info->SetBigInteger("platform_id", *id_it);
      platform_info->SetString("platform_name", *name_it);
      platform.set_comment(platform_info);
    }

    int json_type = 0;
    if (jsonp_str_ != "") {
      platform.set_jsonp_callback(jsonp_str_);
      json_type = 3;
    }
    send_message(socket_, &platform, json_type);
  }
}  //  namespace get_platform_info
