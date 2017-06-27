// Copyright (c) 2015-2015 The restful Authors. All rights reserved.
// Created on: 2015/11/24 Author: jiaoyongqing
#include "usersvc/db_comm.h"

#include <string>
#include <list>
#include <sstream>

static const int SQL_DATA_LACK          = -1021;          // 数据库里没有对应的数据
static const int SQL_NUM_MORE_THAN_ONE  = -1023;          // 查询结果应该小于1
static const int SQL_EXEC_ERROR         = -1024;          // SQL语句执行错误
static const int EXEC_REDIS_ERROR       = -1025;          // redis执行出错
static const int CONNECT_ERROR          = -1026;          // mysql连接失败   
static const int SQL_NO_USER            = -1103;          // 没有该用户

namespace db {
void DbSql::Init(std::list<base::ConnAddr> *const addrlist) {
  base_db::MysqlDBPool::Init(*addrlist);
}

void DbSql::Dest() {
  base_db::MysqlDBPool::Dest();
}

DbSql::DbSql() {
  error_info_ = 0;
  record_num_ = 0;
  engine_ = base_db::MysqlDBPool::DBConnectionPop();
}

DbSql::~DbSql() {
  base_db::MysqlDBPool::DBConnectionPush(engine_);
}

base_storage::DBStorageEngine* DbSql::GetEntine() {
  error_info_ = 0;
  int num = 0;
  while (num < 5) {
    if (engine_) {
      engine_->FreeRes();
      if (!engine_->CheckConnect()) {
        LOG_DEBUG("lost connection");
        engine_->Release();
        if (!engine_->Connections(base_db::MysqlDBPool::addrlist_)) {
          usleep(100);
          ++num;
        } else {
          break;
        }
      } else {
        break;
      }
    } else {
      LOG_DEBUG2("%s", "engine is NULL");
      return NULL;
    }
  }

  if (num >= 5) {
    LOG_ERROR("GetConnection Error");
    error_info_ = CONNECT_ERROR;
    return NULL;
  }

  return engine_;
}

void DbSql::GetUserInfo(int platform_id, const std::string &user_name) {
  engine_ = db::DbSql::GetEntine();
  if (!engine_) {
    LOG_ERROR("exec sql error");
    error_info_ = SQL_EXEC_ERROR;
    return;
  }
  std::stringstream os;

  os << "call proc_GetUserInfo(\'" << user_name
                                   << "\',"
                                   << platform_id \
                                   << ");";
  std::string sql = os.str();
  bool r = engine_->SQLExec(sql.c_str());
  error_info_ = 0;
  if (!r) {
    LOG_ERROR("exec sql error");
    error_info_ = SQL_EXEC_ERROR;
    return;
  }
  record_num_ = engine_->RecordCount();
  if (record_num_ > 1) {
    error_info_ = SQL_NUM_MORE_THAN_ONE;
  } else if (record_num_ <= 0) {
    error_info_ = SQL_NO_USER;
  }
  return;
}

void DbSql::GetPlatFormInfo(std::vector<int> *vec_id, std::vector<std::string> *vec_name) {
  base_storage::DBStorageEngine* engine  = GetEntine();
  std::stringstream os;
  std::string sql;

  error_info_ = 0;
  os.str("");
  os << "call proc_GetPlatFormInfo();";
  sql = os.str();
  bool r = engine->SQLExec(sql.c_str());
  if (false == r) {
    error_info_ = SQL_EXEC_ERROR;
  }

  int num = engine->RecordCount();
  if (num > 0) {
    MYSQL_ROW rows = NULL;
    for (int i = 0; i < num; ++i) {
      rows = (*reinterpret_cast<MYSQL_ROW*>\
           ((engine->FetchRows())->proc));
      vec_id->push_back(atoll(rows[0]));
      vec_name->push_back(rows[1]);
    }
  }
}

std::string DbSql::GetPlatformName(int platform_id) {
  base_storage::DBStorageEngine* engine  = GetEntine();
  std::stringstream os;
  std::string sql;

  error_info_ = 0;
  os.str("");
  os << "call proc_GetPlatFormName(" << platform_id << ");";
  sql = os.str();
  bool r = engine->SQLExec(sql.c_str());
  if (false == r) {
    error_info_ = SQL_EXEC_ERROR;
  }

  int num = engine->RecordCount();
  if (num == 0) return "";

  MYSQL_ROW rows = (*reinterpret_cast<MYSQL_ROW*>((engine->FetchRows())->proc));

  return rows[0];
}

void DbSql::UpdateToken(int64 user_id, std::string &token) {
  engine_ = GetEntine();
  std::stringstream os;
  std::string sql;

  error_info_ = 0;
  os.str("");
  os << "call UpdateToken(" << user_id << ",\'" << token << "\')";
  sql = os.str();
  bool r = engine_->SQLExec(sql.c_str());
  if (false == r) {
    error_info_ = SQL_EXEC_ERROR;
  }
}

MYSQL_ROW DbSql::NextRecord() {
  if (record_num_<= 0) return NULL;
  --record_num_;
  MYSQL_ROW rows = \
    (*reinterpret_cast<MYSQL_ROW*>((engine_->FetchRows())->proc));
  return rows;
}

}  // namespace db
