
###############################################################################
## 编译器
COMPILER = g++
COMPILERC = gcc

## 编译选项
## for debug -DROOM_DEBUG_MCACHE
CPP_FLAG = -g -Wall -w -fPIC -c -D_LINUX -DDEBUG -D_DEBUG -D_REENTRANT -D_GNU_SOURCE -D__DEBUG_CORE__ -DFCGI_STD  #-DCHECK_HEADER-DTEST
RCPP_FLAG = -g -Wall -w -fPIC -c -D_LINUX -DDEBUG -D_DEBUG -D_REENTRANT -D_GNU_SOURCE -DFCGI_STD #-DTEST
## for release
##CPP_FLAG = -O3 -Wall -s -w -fPIC -c -D_LINUX -DNDEBUG -D_REENTRANT -D_GNU_SOURCE -DFCGI_STD

## 链接选项
## for debug
LINK_FLAGS = -g  -fPIC
## for release
## LINK_FLAGS = -s -O3 -shared -fPIC

LIB_DIR = ../../mylib
BASE_DIR = ./../pub
PLUGIN_DIR = ../plugins
## 头文件
INCLUDE_PATHS = -I$(LIB_DIR)/public/\
		-I$(LIB_DIR)/base\
		-I$(LIB_DIR)/\
		-I$(LIB_DIR)/public\
		-I/usr/local/lib\
		-I$(BASE_DIR)/\
		-I$(PLUGIN_DIR)\
		-I$(LIB_DIR)/third_party/breakpad/src

##库
LINK_PATHS =  -L./
LINK_LIBRARYS = -lrt -lpthread -lfcgi++ \


# 编译
COMPILE = $(COMPILER) $(CPP_FLAG) $(INCLUDE_PATHS) -o $@
RCOMPILE = $(COMPILER) $(RCPP_FLAG) $(INCLUDE_PATHS) -o $@

# 链接
LINK = $(COMPILER) $(LINK_FLAGS) $(LINK_PATHS) $? -o $(OUTPUT_FILE) $(LINK_LIBRARYS)
LINKALL = $(COMPILER) $(LINK_FLAGS) $(LINK_PATHS) $?  $(LINK_LIBRARYS)

POBJ =  mig_log.o core_interface.o http_formate.o fcgimodule.o logic_comm.o
OUTPUT_FILE = /var/www/cgi-bin/backtest
#############################################################################################################################
all: clean proc delete-temp-file

proc: userproc kenshoproc searchproc hotsuggestproc

######################userproc#####################################################
USERNAME = user_
USERPROC_PATH = /user
USER_OBJECT_PATH = ./obj$(VIPPROC_PATH)
OTJERPROC_VERSION = /1
userproc:$(USERPROC_PATH)/get_platform_info.fcgi $(USERPROC_PATH)/user_login.fcgi

$(USERPROC_PATH)/get_platform_info.fcgi: $(USER_OBJECT_PATH)/$(USERNAME)get_platform_info.o $(POBJ)
	$(LINKALL) -o $(OUTPUT_FILE)$(USERPROC_PATH)$(OTJERPROC_VERSION)/get_platform_info.fcgi

$(USERPROC_PATH)/user_login.fcgi: $(USER_OBJECT_PATH)/$(USERNAME)user_login.o $(POBJ)
	$(LINKALL) -o $(OUTPUT_FILE)$(USERPROC_PATH)$(OTJERPROC_VERSION)/user_login.fcgi

 ### 编译

$(USER_OBJECT_PATH)/$(USERNAME)get_platform_info.o:
	$(COMPILE) .$(USERPROC_PATH)$(OTJERPROC_VERSION)/get_platform_info/get_platform_info.cc

$(USER_OBJECT_PATH)/$(USERNAME)user_login.o:                                        
	$(COMPILE) .$(USERPROC_PATH)$(OTJERPROC_VERSION)/user_login/user_login.cc

clean:        
	-rm -f *.o
	-rm -f $(OUTPUT_FILE)./*.fcgi
               
delete-temp-file:
	-rm -f *.o

######################kenshoproc###############################################
KENSHONAME = kensho_
KENSHOPROC_PATH = /kensho
KENSHO_OBJECT_PATH = ./obj$(VIPPROC_PATH)
OTJERPROC_VERSION = /1
kenshoproc:$(KENSHOPROC_PATH)/btsentence.fcgi $(KENSHOPROC_PATH)/btresult.fcgi $(KENSHOPROC_PATH)/btyield.fcgi

$(KENSHOPROC_PATH)/btsentence.fcgi: $(KENSHO_OBJECT_PATH)/$(KENSHONAME)btsentence.o $(POBJ)
	$(LINKALL) -o $(OUTPUT_FILE)$(KENSHOPROC_PATH)$(OTJERPROC_VERSION)/btsentence.fcgi
	
$(KENSHOPROC_PATH)/btresult.fcgi: $(KENSHO_OBJECT_PATH)/$(KENSHONAME)btresult.o $(POBJ)
	$(LINKALL) -o $(OUTPUT_FILE)$(KENSHOPROC_PATH)$(OTJERPROC_VERSION)/btresult.fcgi
$(KENSHOPROC_PATH)/btyield.fcgi: $(KENSHO_OBJECT_PATH)/$(KENSHONAME)btyield.o $(POBJ)
	$(LINKALL) -o $(OUTPUT_FILE)$(KENSHOPROC_PATH)$(OTJERPROC_VERSION)/btyield.fcgi
	
### 编译

$(KENSHO_OBJECT_PATH)/$(KENSHONAME)btsentence.o:
	$(COMPILE) .$(KENSHOPROC_PATH)$(OTJERPROC_VERSION)/btsentence/btsentence.cc
	
$(KENSHO_OBJECT_PATH)/$(KENSHONAME)btresult.o:
	$(COMPILE) .$(KENSHOPROC_PATH)$(OTJERPROC_VERSION)/btresult/btresult.cc
$(KENSHO_OBJECT_PATH)/$(KENSHONAME)btyield.o:
	$(COMPILE) .$(KENSHOPROC_PATH)$(OTJERPROC_VERSION)/btyield/btyield.cc



clean:
	-rm -f *.o
	-rm -f $(OUTPUT_FILE)./*.fcgi

delete-temp-file:
	-rm -f *.o

######################searchproc###############################################
BTNAME = search
BTOPROC_PATH = /search
BT_OBJECT_PATH = ./obj$(VIPPROC_PATH)
OTJERPROC_VERSION = /1
searchproc:$(BTOPROC_PATH)/btsearch.fcgi $(BTOPROC_PATH)/btsearch.fcgi

$(BTOPROC_PATH)/btsearch.fcgi: $(BT_OBJECT_PATH)/$(BTNAME)btsearch.o $(POBJ)
	$(LINKALL) -o $(OUTPUT_FILE)$(BTOPROC_PATH)$(OTJERPROC_VERSION)/btsearch.fcgi
	
	
### 编译

$(BT_OBJECT_PATH)/$(BTNAME)btsearch.o:
	$(COMPILE) .$(BTOPROC_PATH)$(OTJERPROC_VERSION)/btsentence/btsearch.cc

clean:
	-rm -f *.o
	-rm -f $(OUTPUT_FILE)./*.fcgi

delete-temp-file:
	-rm -f *.o




#############################################################################################################################
all: clean proc delete-temp-file

proc: hotsuggestproc

######################hotsuggestproc###############################################
HOTSUGGESTNAME = hotsuggest_
HOTSUGGESTPROC_PATH = /hotsuggest
HOTSUGGEST_OBJECT_PATH = ./obj$(VIPPROC_PATH)
OTJERPROC_VERSION = /1
hotsuggestproc:$(HOTSUGGESTPROC_PATH)/hotsuggest.fcgi $(HOTSUGGESTPROC_PATH)/hotsuggest.fcgi

$(HOTSUGGESTPROC_PATH)/hotsuggest.fcgi: $(HOTSUGGEST_OBJECT_PATH)/$(HOTSUGGESTNAME)hotsuggest.o $(POBJ)
	 $(LINKALL) -o $(OUTPUT_FILE)$(HOTSUGGESTPROC_PATH)$(OTJERPROC_VERSION)/hotsuggest.fcgi
### 编译
$(HOTSUGGEST_OBJECT_PATH)/$(HOTSUGGESTNAME)hotsuggest.o:
	   $(COMPILE) .$(HOTSUGGESTPROC_PATH)$(OTJERPROC_VERSION)/hotsuggest/hotsuggest.cc

clean:
	   -rm -f *.o
	   -rm -f $(OUTPUT_FILE)./*.fcgi

delete-temp-file:
	   -rm -f *.o


#############################################################################################################################
all: clean proc delete-temp-file

proc: allsuggestproc

######################allsuggestproc###############################################
ALLSUGGESTNAME = allsuggest_
ALLSUGGESTPROC_PATH = /hotsuggest
ALLSUGGEST_OBJECT_PATH = ./obj$(VIPPROC_PATH)
OTJERPROC_VERSION = /1
allsuggestproc:$(ALLSUGGESTPROC_PATH)/allsuggest.fcgi $(ALLSUGGESTPROC_PATH)/allsuggest.fcgi

$(ALLSUGGESTPROC_PATH)/allsuggest.fcgi: $(ALLSUGGEST_OBJECT_PATH)/$(ALLSUGGESTNAME)allsuggest.o $(POBJ)
	$(LINKALL) -o $(OUTPUT_FILE)$(ALLSUGGESTPROC_PATH)$(OTJERPROC_VERSION)/allsuggest.fcgi

### 编译

$(ALLSUGGEST_OBJECT_PATH)/$(ALLSUGGESTNAME)allsuggest.o:
	$(COMPILE) .$(ALLSUGGESTPROC_PATH)$(OTJERPROC_VERSION)/allsuggest/allsuggest.cc

clean:
	-rm -f *.o
	-rm -f $(OUTPUT_FILE)./*.fcgi

delete-temp-file:
	-rm -f *.o



############public###############################

logic_comm.o:
	$(COMPILE) $(LIB_DIR)/base/logic/logic_comm.cc
	
mig_log.o:
	$(COMPILE) $(LIB_DIR)/public/log/mig_log.cc

core_interface.o:
	$(COMPILE) $(LIB_DIR)/public/comm/core_interface.cc
	
http_formate.o:
	$(COMPILE) $(LIB_DIR)/public/http/http_formate.cc

fcgimodule.o:
	$(COMPILE) $(BASE_DIR)/fcgimodule/fcgimodule.cc
