sharelib=back_share 
plugins=fuzzy_search kensho hot_suggest usersvc

all: $(sharelib) $(plugins) 
$(sharelib):
	cd ./plugins/$@ && $(MAKE) clean && $(MAKE)
	cd ./bin/$@  && ln -sf $@.so lib$@.so && ln -sf $@.so lib$@.so.1
$(plugins):
	@echo $@
	@echo `pwd`
	cd ./plugins/$@ && $(MAKE)  clean && $(MAKE)
