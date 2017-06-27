//  Copyright (c) 2015-2015 The restful Authors. All rights reserved.
//  Created on: 2015/11/24 Author: jiaoyongqing

#ifndef _HOME_TEST_WOOKONG_NORTHSEA_RESTFUL_PLUGINS_USERSVC_USER_LOGIC_H_
#define _HOME_TEST_WOOKONG_NORTHSEA_RESTFUL_PLUGINS_USERSVC_USER_LOGIC_H_

#include "usersvc/http_data.h"
#include "core/common.h"

#define DEFAULT_CONFIG_PATH     "./plugins/usersvc/user_config.xml"
namespace back_share {
  class BSEngine;
}

namespace usersvc_logic {
class Userlogic {
 public:
  virtual ~Userlogic();

 private:
  Userlogic();

class crelease {
  ~crelease() {
    if (instance_ != NULL) {
      delete instance_;
      instance_ = NULL;
    }
  }
};

 private:
  static crelease release_;
  static Userlogic *instance_;

 public:
  static Userlogic *GetInstance() {
    if (instance_ == NULL) {
      instance_ = new Userlogic();
    }

    return instance_;
  }

 public:
  bool OnUserConnect(struct server *srv, const int socket);

  bool OnUserMessage(struct server *srv, \
                              const int socket, \
                               const void *msg, \
                                 const int len);

  bool OnUserClose(struct server *srv, const int socket);

  bool OnBroadcastConnect(struct server *srv, \
                            const int socket, \
                            const void *data, \
                               const int len);

  bool OnBroadcastMessage(struct server *srv, \
                            const int socket, \
                             const void *msg, \
                               const int len);

  bool OnBroadcastClose(struct server *srv, \
                          const int socket);

  bool OnIniTimer(struct server *srv);

  bool OnTimeout(struct server *srv, \
                           char* id, \
                         int opcode, \
                           int time);

 public:
  enum UserLevel{ LEVEL_ONE, LEVEL_TWO, LEVEL_THREE };

 private:
  bool OnGetPlatformInfo(struct server *srv, \
                           const int socket, \
                           NetBase* netbase, \
                     const void* msg = NULL, \
                          const int len = 0);

  bool OnLogin(struct server *srv, \
                 const int socket, \
                 NetBase* netbase, \
           const void* msg = NULL, \
                 const int len = 0);

 private:
  bool init();
  back_share::BSEngine *bs_engine_;
};


}  //  namespace usersvc_logic

#endif  //  _HOME_TEST_WOOKONG_NORTHSEA_RESTFUL_PLUGINS_USERSVC_USER_LOGIC_H_

