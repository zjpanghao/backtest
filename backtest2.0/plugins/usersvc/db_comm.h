// Copyright (c) 2015-2015 The restful Authors. All rights reserved.
// Created on: 2015/11/24 Author: jiaoyongqing
#ifndef _HOME_TEST_WOOKONG_NORTHSEA_RESTFUL_PLUGINS_USERSVC_DB_COMM_H_
#define _HOME_TEST_WOOKONG_NORTHSEA_RESTFUL_PLUGINS_USERSVC_DB_COMM_H_

#include <mysql.h>
#include <list>
#include <string>

#include "config/config.h"
#include "db/base_db_mysql_auto.h"
#include "storage/storage.h"

namespace db {
class DbSql {
 public:
  DbSql();
  ~DbSql();

 public:
  static void Init(std::list<base::ConnAddr> * const addrlist);
  static void Dest();

 public:
  //  得到用户的等级
  int64 GetUserLevel(int64 user_id);

  //  得到用户信息
  void GetUserInfo(int platform_id, const std::string &user_name);

  //  得到平台信息
  void GetPlatFormInfo(std::vector<int> *vec_id, std::vector<std::string> *vec_name);

  //  得到平台名字
  std::string GetPlatformName(int platform_id);

  void UpdateToken(int64 user_id, std::string &token);

  int get_record_num() { return engine_->RecordCount(); }

  MYSQL_ROW NextRecord();
  int get_error_info() { return error_info_; }

 private:
  base_storage::DBStorageEngine* GetEntine();

 private:
  //  记录数据库的状态
  int error_info_;
  base_storage::DBStorageEngine* engine_;
  int record_num_;
};

}  // namespace db

#endif  //  _HOME_TEST_WOOKONG_NORTHSEA_RESTFUL_PLUGINS_USERSVC_DB_COMM_H_
