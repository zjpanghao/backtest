//  Copyright (c) 2015-2016 The BACKSVC Authors. All rights reserved.
//  Created on: 2015年9月16日 Author: kerry
#include "kensho/kensho_init.h"
#include "signal.h"
#include "core/common.h"
#include "core/plugins.h"
#include "kensho/kensho_logic.h"

struct kenshoplugin {
  char* id;
  char* name;
  char* version;
  char* provider;
};

static void *OnKenshoStart() {
  signal(SIGPIPE, SIG_IGN);
  struct kenshoplugin* kensho = (struct kenshoplugin*) calloc(
      1, sizeof(struct kenshoplugin));
  kensho->id = "kensho";
  kensho->name = "kensho";
  kensho->version = "1.0.0";
  kensho->provider = "kerry";
  if (!kensho_logic::KenshoLogic::GetInstance())
    assert(0);
  return kensho;
}

static handler_t OnKenshoShutdown(struct server* srv, void* pd) {
  kensho_logic::KenshoLogic::FreeInstance();

  return HANDLER_GO_ON;
}

static handler_t OnKenshoConnect(struct server *srv, int fd, void *data,
                                 int len) {
  kensho_logic::KenshoLogic::GetInstance()->OnKenshoConnect(srv, fd);
  return HANDLER_GO_ON;
}

static handler_t OnKenshoMessage(struct server *srv, int fd, void *data,
                                 int len) {
  kensho_logic::KenshoLogic::GetInstance()->OnKenshoMessage(srv, fd, data, len);
  return HANDLER_GO_ON;
}

static handler_t OnKenshoClose(struct server *srv, int fd) {
  kensho_logic::KenshoLogic::GetInstance()->OnKenshoClose(srv, fd);
  return HANDLER_GO_ON;
}

static handler_t OnUnknow(struct server *srv, int fd, void *data, int len) {
  return HANDLER_GO_ON;
}

static handler_t OnBroadcastConnect(struct server* srv, int fd, void *data,
                                    int len) {
  kensho_logic::KenshoLogic::GetInstance()->OnBroadcastConnect(srv, fd, data,
                                                               len);
  return HANDLER_GO_ON;
}

static handler_t OnBroadcastClose(struct server* srv, int fd) {
  kensho_logic::KenshoLogic::GetInstance()->OnBroadcastClose(srv, fd);
  return HANDLER_GO_ON;
}

static handler_t OnBroadcastMessage(struct server* srv, int fd, void *data,
                                    int len) {
  kensho_logic::KenshoLogic::GetInstance()->OnBroadcastMessage(srv, fd, data,
                                                               len);
  return HANDLER_GO_ON;
}

static handler_t OnIniTimer(struct server* srv) {
  kensho_logic::KenshoLogic::GetInstance()->OnIniTimer(srv);
  return HANDLER_GO_ON;
}

static handler_t OnTimeOut(struct server* srv, char* id, int opcode, int time) {
  kensho_logic::KenshoLogic::GetInstance()->OnTimeout(srv, id, opcode, time);
  return HANDLER_GO_ON;
}

int kensho_plugin_init(struct plugin *pl) {
  pl->init = OnKenshoStart;
  pl->clean_up = OnKenshoShutdown;
  pl->connection = OnKenshoConnect;
  pl->connection_close = OnKenshoClose;
  pl->connection_close_srv = OnBroadcastClose;
  pl->connection_srv = OnBroadcastConnect;
  pl->handler_init_time = OnIniTimer;
  pl->handler_read = OnKenshoMessage;
  pl->handler_read_srv = OnBroadcastMessage;
  pl->handler_read_other = OnUnknow;
  pl->time_msg = OnTimeOut;
  pl->data = NULL;
  return 0;
}
