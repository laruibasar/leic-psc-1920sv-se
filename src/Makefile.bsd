CC = cc
 
APP = rates
LIB = librates.so
LIB_NAME = rates

CFLAGS = -c -Wall -Werror -Og -g
LIB_CFLAGS = -fpic

LDFLAGS = 
LIB_LDFLAGS = -shared

DEPS = rates.h
OBJ = http_get_data.o get_rates.o

TEST_OBJ = test1.o test2.o

CURL_FLAGS != curl-config --cflags
CURL_LDFLAGS != curl-config --libs

JSON_CFLAGS != pkg-config --cflags jansson
JSON_LDFLAGS != pkg-config --libs jansson

CFLAGS += $(CURL_CFLAGS) $(JSON_CFLAGS)
LDFLAGS += $(CURL_LDFLAGS) $(JSON_LDFLAGS)

DIR_TEST = ../tests
DIR_BUILD = ../build
DIR_LIB = $(DIR_BUILD)/lib

APP_LDFLAGS = -L$(PWD)/$(DIR_LIB) -l$(LIB_NAME) -R$(PWD)/$(DIR_LIB)

.PHONY: all
all: lib app tests

.PHONY: lib
lib: $(OBJ)
	@echo "Create directory" $(DIR_LIB)
	mkdir -p $(DIR_LIB)
	@echo "Linking and generating" $(LIB)
	$(CC) $(LIB_LDFLAGS) $(LDFLAGS) -o $(DIR_LIB)/$(LIB) $(OBJ)

.PHONY: app
app: lib $(APP)

.PHONY: clean
clean: clean-lib clean-app clean-tests

clean-tests:  
	@echo "Cleaning tests *.o"
	rm -f $(TEST_OBJ)
	@echo "Cleaning executable tests files"
	rm -f $(DIR_TEST)/*

clean-lib:
	@echo "Cleaning lib"
	rm -f $(DIR_LIB)/$(LIB)
	@echo "Cleaning lib *.o"
	rm -f $(OBJ)

clean-app:
	@echo "Cleaning app"
	rm -f $(DIR_BUILD)/$(APP)
	@echo "Cleaning app *.o"
	rm -f rates.o

.PHONY: tests
tests: test1 test2

test1: test1.o $(OBJ)
	@echo "Create directory" $(DIR_TEST)
	mkdir -p $(DIR_TEST)
	@echo "Linking and create executable" $@
	$(CC) -o $(DIR_TEST)/$@ $(LDFLAGS) $(OBJ) test1.o

test1.o: test1.c $(DEPS)
	@echo "Compiling" $<
	$(CC) $(CFLAGS) -o $@ $<

test2: test2.o $(OBJ)
	@echo "Create directory" $(DIR_TEST)
	mkdir -p $(DIR_TEST)
	@echo "Linking and create executable" $@
	$(CC) -o $(DIR_TEST)/$@ $(LDFLAGS) $(OBJ) test2.o

test2.o: test2.c $(DEPS)
	@echo "Compiling" $<
	$(CC) $(CFLAGS) -o $@ $<

http_get_data.o: http_get_data.c $(DEPS)
	@echo "Compiling" $<
	$(CC) $(CFLAGS) $(LIB_CFLAGS) -o $@ $<

get_rates.o: get_rates.c $(DEPS)
	@echo "Compiling" $<
	$(CC) $(CFLAGS) $(LIB_CFLAGS) -o $@ $<

rates: rates.o
	@echo "Create directory " $(DIR_BUILD)
	mkdir -p $(DIR_BUILD)
	@echo "Linking and creating executable " $@
	$(CC) $(APP_LDFLAGS) -o $(DIR_BUILD)/$@ rates.o

rates.o: rates.c $(DEPS)
	@echo "Compiling " $< 
	$(CC) $(CFLAGS) -o $@ $<

