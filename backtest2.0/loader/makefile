
###############################################################################
## 输出文件名
OUTPUT_FILE = ./../bin/backtest20
## 编译器
COMPILER = g++

## 编译选项
## for debug -DROOM_DEBUG_MCACHE
##CPP_FLAG = -g -Wall -w -fPIC -c -D_LINUX -DDEBUG -D_DEBUG -D_REENTRANT -D_GNU_SOURCE
## for release
CPP_FLAG = -O3 -Wall -s -w -fPIC -c -D_LINUX -DNDEBUG -D_REENTRANT -D_GNU_SOURCE

## 链接选项
## for debug
LINK_FLAGS = -g  -fPIC
## for release
## LINK_FLAGS = -s -O3 -shared -fPIC

## 头文件
LIB_DIR=./../../mylib

INCLUDE_PATHS = -I./../../mylib/public -I./../../mylib/third_party/breakpad/src -I$(LIB_DIR)/third_party/glog/include

## 库
LINK_PATHS =  -L/usr/local/lib -L../bin/plugins/user -L $(LIB_DIR)/third_party/glog/lib
LINK_LIBRARYS = -lpthread -lexpat -ldl -levent -levent-2.0 -levent_core-2.0 -Wl,-rpath=/usr/local/lib  -Wl,-rpath=$(LIB_DIR)/third_party/glog/lib -lglog

# 编译
COMPILE = $(COMPILER) $(CPP_FLAG) $(INCLUDE_PATHS) -o $@

# 链接
LINK = $(COMPILER) $(LINK_FLAGS) $(LINK_PATHS) $? -o $(OUTPUT_FILE) $(LINK_LIBRARYS)

all: clean output delete-temp-file
output: loader.o native_library.o mig_log.o
	$(LINK)

clean:
	-rm -f *.o
	-rm -f $(OUTPUT_FILE)

delete-temp-file:
	-rm -f *.o

### 编译
loader.o:
	$(COMPILE) ./loader.cc

native_library.o:
	$(COMPILE) ./../../mylib/public/basic/native_library.cc

mig_log.o:
	$(COMPILE) ./../../mylib/public/log/mig_log.cc
