//
// Created by harvey on 2016/8/31.
//

#include "hot_suggest_init.h"

#include "core/common.h"
#include "core/plugins.h"
#include "logic/logic_comm.h"
#include "hot_suggest/hot_suggest_logic.h"

#include <signal.h>

typedef struct hotsuggestplugin {
  char* id;
  char* name;
  char* version;
  char* provider;
} HotSuggestPlugin;

static void* OnHotSuggestStart() {
  signal(SIGPIPE, SIG_IGN);
  HotSuggestPlugin* hotsuggest = (HotSuggestPlugin*) calloc(1, sizeof(HotSuggestPlugin));
  if (!hotsuggest) {
    LOG_ERROR2("OnHostSuggestStart::calloc error, errno = %s", strerror(errno));
    assert(0);
  }
  hotsuggest->id = "hotsuggest";
  hotsuggest->name = "hotsuggest";
  hotsuggest->version = "1.0.0";
  hotsuggest->provider = "harvey";
  if (!hot_suggest_logic::HotSuggestLogic::GetPtr())
    assert(0);
  return hotsuggest;
}

static handler_t OnHotSuggestShutdown(struct server* srv, void* pd) {
  return HANDLER_GO_ON;
}

static handler_t OnHotSuggestConnect(struct server* srv, int fd, void* data,
                                     int len) {
  hot_suggest_logic::HotSuggestLogic::GetPtr()->OnHotSuggestConnect(srv, fd);
  return HANDLER_GO_ON;
}

static handler_t OnHotSuggestMessage(struct server* srv, int fd, void* data,
                                     int len) {
  hot_suggest_logic::HotSuggestLogic::GetPtr()->OnHotSuggestMessage(srv, fd, data, len);
  return HANDLER_GO_ON;
}

static handler_t OnHotSuggestClose(struct server* srv, int fd) {
  hot_suggest_logic::HotSuggestLogic::GetPtr()->OnHotSuggestClose(srv, fd);
  return HANDLER_GO_ON;
}

static handler_t OnUnknow(struct server* srv, int fd, void* data, int len) {
  return HANDLER_GO_ON;
}

static handler_t OnBroadcastConnect(struct server* srv, int fd, void* data,
                                    int len) {
  hot_suggest_logic::HotSuggestLogic::GetPtr()->OnBroadcastConnect(srv, fd, data,
                                                                   len);
  return HANDLER_GO_ON;
}

static handler_t OnBroadcastClose(struct server* srv, int fd) {
  hot_suggest_logic::HotSuggestLogic::GetPtr()->OnBroadcastClose(srv, fd);
  return HANDLER_GO_ON;
}

static handler_t OnBroadcastMessage(struct server* srv, int fd, void* data,
                                    int len) {
  hot_suggest_logic::HotSuggestLogic::GetPtr()->OnBroadcastMessage(srv, fd, data,
                                                                   len);
  return HANDLER_GO_ON;
}

static handler_t OnIniTimer(struct server* srv) {
  hot_suggest_logic::HotSuggestLogic::GetPtr()->OnIniTimer(srv);
  return HANDLER_GO_ON;
}

static handler_t OnTimeOut(struct server* srv, char* id, int opcode, int time) {
  hot_suggest_logic::HotSuggestLogic::GetPtr()->OnTimeout(srv, id, opcode, time);
  return HANDLER_GO_ON;
}

int hotsuggest_plugin_init(struct plugin* pl) {
  pl->init = OnHotSuggestStart;
  pl->clean_up = OnHotSuggestShutdown;
  pl->connection = OnHotSuggestConnect;
  pl->connection_close = OnHotSuggestClose;
  pl->connection_close_srv = OnBroadcastClose;
  pl->connection_srv = OnBroadcastConnect;
  pl->handler_init_time = OnIniTimer;
  pl->handler_read = OnHotSuggestMessage;
  pl->handler_read_srv = OnBroadcastMessage;
  pl->handler_read_other = OnUnknow;
  pl->time_msg = OnTimeOut;
  pl->data = NULL;
  return 0;
}