BUILD_ROOT = $(shell pwd)

OBJ_DIR = $(BUILD_ROOT)
BIN_DIR = $(BUILD_ROOT)

TARGET = $(sort $(subst .cpp, , $(shell ls *.cpp)))

### Flag 선언
CXXFLAGS = -Wall -W -g
CXXFLAGS += -I$(BUILD_ROOT)/include
CXXFLAGS += -std=c++11
CXXFLAGS += $(filter-out -Wunused-parameter, $(CXXFLASG))

### compiler 설정
CC = g++


.PHONY: all $(TARGET)

all: $(TARGET)

$(TARGET): %: %.cpp
	@echo "[Build... $(notdir $@)]"
	@$(CC) -o $(BIN_DIR)/$@ $? $(CXXFLAGS)

	
### Include dependencies
DEP_FILES := $(wildcard $(OBJ_DIR)/*.d)
ifneq ($(DEP_FILES),)
-include $(DEP_FILES)
endif
