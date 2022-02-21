CC = gcc
CXX = g++
RM = rm -rf
MKDIR = mkdir -p
C_FLAGS = -O3 -fPIC -w
CXX_FLAGS = -O3 -fPIC -w -std=c++17
LD_FLAGS =
LD_SHARED_FLAGS = -fPIC -shared
LD_LIBS  =

TEST_EXEC = binom_test
LIB_TARGET = libbinom

BUILD_DIR = build
TEST_DIR = $(BUILD_DIR)/test
INCLUDE_PATH = -I./
SOURCES_PATH = ./libbinom/source/
TEST_SOURCES_PATH = ./test

SOURCES = $(shell find $(SOURCES_PATH) -name "*.cxx")
OBJECTS = $(addprefix $(BUILD_DIR)/, $(notdir $(SOURCES:.cxx=.o)))
OBJECTS_CLEAN = $(shell find $(BUILD_DIR) -name "*.o")

TEST_SOURCES = $(shell find $(TEST_SOURCES_PATH) -name "*.cxx")
TEST_OBJECTS = $(addprefix $(BUILD_DIR)/test/, $(notdir $(TEST_SOURCES:.cxx=.o)))

all: test lib

test: $(TEST_DIR)/$(TEST_EXEC)
	$(TEST_DIR)/$(TEST_EXEC)

lib: $(OBJECTS) $(BUILD_DIR)/$(LIB_TARGET).a $(BUILD_DIR)/$(LIB_TARGET).so

$(BUILD_DIR)/$(LIB_TARGET).a:
	ar cr $@ $(OBJECTS)

$(BUILD_DIR)/$(LIB_TARGET).so:
	$(CXX) $(LD_SHARED_FLAGS) $(LD_LIBS) -o $@ $(OBJECTS)

$(TEST_DIR)/$(TEST_EXEC): $(OBJECTS) $(TEST_OBJECTS)
	$(CXX) $(OBJECTS) $(TEST_OBJECTS) -o $@ $(LD_FLAGS) $(LD_LIBS)

$(OBJECTS): $(SOURCES)
	$(MKDIR) $(dir $@)
	$(CXX) $(INCLUDE_PATH) $(CXX_FLAGS) -c $(filter %$(subst .o,.cxx,$(notdir $@)), $(SOURCES)) -o $@

$(TEST_OBJECTS): $(TEST_SOURCES)
	$(MKDIR) $(dir $@)
	$(CXX) $(INCLUDE_PATH) $(CXX_FLAGS) -c $(filter %$(subst .o,.cxx,$(notdir $@)), $(TEST_SOURCES)) -o $@

.PHONY: clean
clean:
	$(RM) $(BUILD_DIR)

clean_obj:
	$(RM) $(OBJECTS_CLEAN)
