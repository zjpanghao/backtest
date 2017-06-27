//  Copyright (c) 2015-2016 The BACKTEST Authors. All rights reserved.
//  Created on: 2016年8月30日 Author: kerry

#ifndef BACKTEST_LOGIC_UNIT_H__
#define BACKTEST_LOGIC_UNIT_H__
#include "basic/native_library.h"
#include <string>
#include "net/proto_buf.h"

namespace back_logic {
class LogicUnit {
 public:
  static void SendFull(const int socket, const char *buf, int blen);
};
class SomeUtils {
 public:
  static void* GetLibraryFunction(const std::string& library_name,
                                  const std::string& func_name);
  static inline std::string StringReplaceAll(std::string& str,
                                          const std::string& old_value,
                                          const std::string& new_value) {
    for (std::string::size_type pos(0); pos != std::string::npos; pos +=
        new_value.length()) {

      if ((pos = str.find(old_value, pos)) != std::string::npos)
        str.replace(pos, old_value.length(), new_value);
      else
        break;
    }
    return str;
  }

  static inline std::string StringReplace(std::string& str,
                                          const std::string& old_value,
                                          const std::string& new_value) {
    for (std::string::size_type pos(0); pos != std::string::npos; pos +=
        new_value.length()) {

      if ((pos = str.find(old_value, pos)) != std::string::npos){
        str.replace(pos, old_value.length(), new_value);
        break;
      }
      else
        break;
    }
    return str;
  }

};
class SendUtils {
 public:
  static void SendFailMessage(const int socket, const int error_code,
                              const std::string& error_str);
  static void SendFailMessage(const int socket, back_logic::PacketFail& msg);
};
}
#endif
