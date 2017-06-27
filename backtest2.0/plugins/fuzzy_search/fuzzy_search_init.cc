//  Copyright (c) 2015-2016 The BACKSVC Authors. All rights reserved.
//  Created on: 2016年9月1日 Author: tangtao
#include <signal.h>
#include "fuzzy_search/fuzzy_search_init.h"
#include "core/common.h"
#include "core/plugins.h"
#include "fuzzy_search/fuzzy_search_logic.h"
#include "fuzzy_search/test/fuzzy_search_test.h"

struct fuzzy_search_plugin {
  char* id;
  char* name;
  char* version;
  char* provider;
};

static void *OnFuzzysearchStart() {
  signal(SIGPIPE, SIG_IGN);
  struct fuzzy_search_plugin *fuzzy_search = (struct fuzzy_search_plugin*) calloc(\
                                  1, sizeof(struct fuzzy_search_plugin));
  fuzzy_search->id = "fuzzy_search";
  fuzzy_search->name = "fuzzy_search";
  fuzzy_search->version = "1.0.0";
  fuzzy_search->provider = "tangtao";
  if (!fuzzy_search_logic::FuzzysearchLogic::GetInstance())
    assert(0);
  return fuzzy_search;
}

static handler_t OnFuzzysearchShutdown(struct server *srv, void *pd) {
  fuzzy_search_logic::FuzzysearchLogic::FreeInstance();
  return HANDLER_GO_ON;
}

static handler_t OnFuzzysearchConnect(struct server *srv, int fd, void *data, int len) {
  fuzzy_search_logic::FuzzysearchLogic::GetInstance()->OnFuzzysearchConnect(srv, fd);
  return HANDLER_GO_ON;
}

static handler_t OnFuzzysearchMessage(struct server *srv, int fd, void *data, int len) {
  fuzzy_search_logic::FuzzysearchLogic::GetInstance()->OnFuzzysearchMessage(srv, fd, data, len);
  return HANDLER_GO_ON;
}

static handler_t OnFuzzysearchClose(struct server *srv, int fd) {
  fuzzy_search_logic::FuzzysearchLogic::GetInstance()->OnFuzzysearchClose(srv, fd);
  return HANDLER_GO_ON;
}

static handler_t OnUnknow(struct server *srv, int fd, void *data, int len) {
  return HANDLER_GO_ON;
}

static handler_t OnBroadcastConnect(struct server *srv, int fd, void *data, int len) {
  fuzzy_search_logic::FuzzysearchLogic::GetInstance()->OnBroadcastConnect(srv, fd, data, len);
  return HANDLER_GO_ON;
}

static handler_t OnBroadcastClose(struct server *srv, int fd) {
  fuzzy_search_logic::FuzzysearchLogic::GetInstance()->OnBroadcastClose(srv, fd);
  return HANDLER_GO_ON;
}

static handler_t OnBroadcastMessage(struct server *srv, int fd, void *data, int len) {
  fuzzy_search_logic::FuzzysearchLogic::GetInstance()->OnBroadcastMessage(srv, fd, data, len);
  return HANDLER_GO_ON;
}

static handler_t OnIniTimer(struct server *srv) {
  fuzzy_search_logic::FuzzysearchLogic::GetInstance()->OnIniTimer(srv);
  return HANDLER_GO_ON;
}

static handler_t OnTimeOut(struct server *srv, char *id, int opcode, int time) {
  fuzzy_search_logic::FuzzysearchLogic::GetInstance()->OnTimeout(srv, id, opcode, time);
  return HANDLER_GO_ON;
}

int fuzzy_search_plugin_init(struct plugin *pl) {

  pl->init = OnFuzzysearchStart;
  pl->clean_up = OnFuzzysearchShutdown;
  pl->connection = OnFuzzysearchConnect;
  pl->connection_close = OnFuzzysearchClose;
  pl->connection_close_srv = OnBroadcastClose;
  pl->connection_srv = OnBroadcastConnect;
  pl->handler_init_time = OnIniTimer;
  pl->handler_read = OnFuzzysearchMessage;
  pl->handler_read_srv = OnBroadcastMessage;
  pl->handler_read_other = OnUnknow;
  pl->time_msg = OnTimeOut;
  pl->data = NULL;
  return 0;
}
