
#include <stdio.h>
#include <stdlib.h>

#include <string>
#include <iostream>
#include <sstream>

// #include "client/linux/handler/exception_handler.h"

#if defined (FCGI_STD)
#include <fcgi_stdio.h>
#elif defined(FCGI_PLUS)
#include <fcgio.h>
#include <fcgi_config.h>
#endif

#include "log/mig_log.h"
#include "../pub/net/comm_head.h"
#include "../plugins/fuzzy_search/operator_code.h"
#include "fcgimodule/fcgimodule.h"
// 设置请求类型
#define API_TYPE            KENSHO_BT_CONTITION_SENTENCE_CHECK_SEARCH
#define LOG_TYPE            log_trace::TRACE_API_LOG

int main(int agrc, char* argv[]) {
  fcgi_module::FcgiModule fcgi_client;
  fcgi_client.Init(BACKTEST_CORE, back_logic::SEARCH_TYPE, API_TYPE, 1);
                   
  fcgi_client.Run();
  fcgi_client.Close();
  return 0;
}
