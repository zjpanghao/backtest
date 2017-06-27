//  Copyright (c) 2015-2015 The restful Authors. All rights reserved.
//  Created on: 2015/11/24 Author: jiaoyongqing

#include <signal.h>

#include "core/common.h"
#include "core/plugins.h"
#include "usersvc/user_init.h"
#include "usersvc/user_logic.h"
#include "logic/logic_comm.h"

struct userplugin {
  char* id;
  char* name;
  char* version;
  char* provider;
};

static void *OnUserStart() {
  signal(SIGPIPE, SIG_IGN);
  struct userplugin* user = \
    (struct userplugin*)calloc(1, sizeof(struct userplugin));

  user->id = "user";

  user->name = "user";

  user->version = "1.0.0";

  user->provider = "jiaoyongqing";

  return user;
}

static handler_t OnUserShutdown(struct server* srv, void* pd) {
  return HANDLER_GO_ON;
}

static handler_t OnUserConnect(struct server *srv, \
                                                 int fd, \
                                             void *data, \
                                                int len) {
  usersvc_logic::Userlogic::GetInstance()\
                           ->OnUserConnect(srv, fd);

  return HANDLER_GO_ON;
}

static handler_t OnUserMessage(struct server *srv, \
                                           int fd, \
                                       void *data, \
                                          int len) {
  bool r = usersvc_logic::Userlogic::GetInstance()\
                  ->OnUserMessage(srv, fd, data, len);
  if (r == true)
    return HANDLER_FINISHED;
  else
    return HANDLER_GO_ON;
}

static handler_t OnUserClose(struct server *srv, int fd) {
  usersvc_logic::Userlogic::GetInstance()\
                         ->OnUserClose(srv, fd);

  return HANDLER_GO_ON;
}

static handler_t OnUnknow(struct server *srv, \
                                      int fd, \
                                  void *data, \
                                     int len) {
  return HANDLER_GO_ON;
}

static handler_t OnBroadcastConnect(struct server* srv, \
                                                int fd, \
                                            void *data, \
                                               int len) {
  usersvc_logic::Userlogic::GetInstance()->\
                  OnBroadcastConnect(srv, fd, data, len);

  return HANDLER_GO_ON;
}

static handler_t OnBroadcastClose(struct server* srv, int fd) {
  usersvc_logic::Userlogic::GetInstance()->\
                              OnBroadcastClose(srv, fd);

  return HANDLER_GO_ON;
}

static handler_t OnBroadcastMessage(struct server* srv, \
                                                int fd, \
                                            void *data, \
                                               int len) {
  usersvc_logic::Userlogic::GetInstance()->\
                  OnBroadcastMessage(srv, fd, data, len);

  return HANDLER_GO_ON;
}

static handler_t OnIniTimer(struct server* srv) {
  usersvc_logic::Userlogic::GetInstance()->OnIniTimer(srv);
  return HANDLER_GO_ON;
}

static handler_t OnTimeOut(struct server* srv, \
                                     char* id, \
                                   int opcode, \
                                     int time) {
  usersvc_logic::Userlogic::GetInstance()->\
                       OnTimeout(srv, id, opcode, time);

  return HANDLER_GO_ON;
}

int user_plugin_init(struct plugin *pl) {
  pl->init = OnUserStart;

  pl->clean_up = OnUserShutdown;

  pl->connection = OnUserConnect;

  pl->connection_close = OnUserClose;

  pl->connection_close_srv = OnBroadcastClose;

  pl->connection_srv = OnBroadcastConnect;

  pl->handler_init_time = OnIniTimer;

  pl->handler_read = OnUserMessage;

  pl->handler_read_srv = OnBroadcastMessage;

  pl->handler_read_other = OnUnknow;

  pl->time_msg = OnTimeOut;

  pl->data = NULL;

  return 0;
}

