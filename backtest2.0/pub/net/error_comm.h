//  Copyright (c) 2015-2015 The restful Authors. All rights reserved.
//  Created on: 2015/11/24 Author: jiaoyongqing
/*
 * error_comm.h
 *
 *  Created on: 2015/10/26
 *      Author: jiaoyongqing
 */

#ifndef _HOME_RUNNER_NORTHSEA_RESTFUL_PUB_NET_ERROR_COMM_H_
#define _HOME_RUNNER_NORTHSEA_RESTFUL_PUB_NET_ERROR_COMM_H_

enum neterrorcode{
  STRUCT_ERROR = -100,                     // 结构错误
  NULL_DATA    = -200,                     // 没有数据
  START_LACK   = -300,                     // 起始时间缺失
  END_LACK     = -400,                     // 结束时间缺失     

  // sql
  SQL_DATA_LACK          = -1021,          // 数据库里没有对应的数据
  SQL_NUM_MORE_THAN_ONE  = -1023,          // 查询结果应该小于1
  SQL_EXEC_ERROR         = -1024,          // SQL语句执行错误
  EXEC_REDIS_ERROR       = -1025,          // redis执行出错
  CONNECT_ERROR          = -1026,          // mysql连接失败
  REIDS_CONNECT_ERROR    = -1027,          // redis连接失

  // user
  USER_ID_ISNOT_IN_SQL   = -1100,          //  数据库中没有对应的id             
  HAVE_NO_RIGHT          = -1101,          // 权限不够
  PLATFORM_NAME_LACK     = -1102,          // platform_name缺失
  SQL_NO_USER            = -1103,          // 没有该用户
  USER_ID_LACK           = -1104,          // 用户id缺失
  PLATFORM_ID_LACK       = -1105,          // 平台id缺失                                                                                                                      
  USER_NAME_LACK         = -1106,          // 用户名缺失
  USER_PASSWORD_LACK     = -1107,          // 用户密码缺失
  USER_PASSWORD_ERROR    = -1108,          // 用户密码错误
  TOKEN_LACK             = -1109,          // 用户凭据缺失
  TOKEN_OVER_ERROR       = -1110,          // 无效的用户凭据

  // information
  INFO_TYPE_LIST_LACK    = -1200,          // 咨询类型列表缺失

  //  strategy
  STRATEGY_LACK = -1133,                   // 策略名缺失
  STOCK_POSITION_ID_LACK=-1140,	           //调仓id错误
  
  STOCK_CODE_LACK = -1015,                 // 股票代码缺失
	OPERATE_CODE_LACK = -1016,               // 操作码缺失
	OPERATE_CODE_ERROR = -1017,              // 错误的操作码
  ORI_NAME_LACK = -1018,                   // 原始名缺失
  CUR_NAME_LACK = -1019,                   // 当前名缺失
	DATA_TYPE_LACK = -1020,                  // datatype缺失
	HOT_TYPE_LACK = -1031,                   // hot_type缺失

	//  USER GROUP
	OVER_GROUP_NUM = -1201,                  // 最多只能有7个标签
  OVER_STOCK_NUM = -1202,                  // 最多只能有20个股票
	ALL_DIGIT      = -1203,                  // 不能全是数字
	GROUP_SUV      = -1204,                  // 组合名已近存在

  MESSAGE_LACK = -1050,                    // 搜索信息缺失

};

const char*
buddha_strerror(int error_code);

#endif  //  _HOME_RUNNER_NORTHSEA_RESTFUL_PUB_NET_ERROR_COMM_H_
