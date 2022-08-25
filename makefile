CC = gcc-11
CXX = g++-11
RM = rm -rf
MKDIR = mkdir -p
DEBUG_FLAGS = -g -Wall -Wextra
# -Wshadow -Wformat=2 -Wfloat-equal -Wconversion -Wlogical-op -Wshift-overflow=2 -Wduplicated-cond -Wcast-qual -Wcast-align -D_GLIBCXX_DEBUG -D_GLIBCXX_DEBUG_PEDANTIC -D_FORTIFY_SOURCE=2
NO_DEBUG_FLAGS = -w
C_FLAGS = -O3 -fPIC $(DEBUG_FLAGS)
CXX_FLAGS = -O3 -fPIC $(DEBUG_FLAGS) -std=c++20
LD_FLAGS =
LD_SHARED_FLAGS = -fPIC -shared
LD_LIBS  = -lpthread

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

all: lib build_test

build_test: $(TEST_DIR)/$(TEST_EXEC)

test: $(TEST_DIR)/$(TEST_EXEC)
	$(TEST_DIR)/$(TEST_EXEC)

lib: $(OBJECTS) $(BUILD_DIR)/$(LIB_TARGET).a $(BUILD_DIR)/$(LIB_TARGET).so

$(BUILD_DIR):
	$(MKDIR) $@

$(TEST_DIR):
	$(MKDIR) $@

$(BUILD_DIR)/$(LIB_TARGET).a:
	ar cr $@ $(OBJECTS)

$(BUILD_DIR)/$(LIB_TARGET).so:
	$(CXX) $(LD_SHARED_FLAGS) $(LD_LIBS) -o $@ $(OBJECTS)

$(TEST_DIR)/$(TEST_EXEC): $(OBJECTS) $(TEST_OBJECTS)
	$(CXX) $(OBJECTS) $(TEST_OBJECTS) -o $@ $(LD_FLAGS) $(LD_LIBS)

$(OBJECTS): $(BUILD_DIR) $(SOURCES)
	$(CXX) $(INCLUDE_PATH) $(CXX_FLAGS) -c $(filter-out %_$(subst .o,.cxx,$(notdir $@)), $(filter %$(subst .o,.cxx,$(notdir $@)), $(SOURCES))) -o $@

$(TEST_OBJECTS): $(TEST_DIR) $(TEST_SOURCES)
	$(CXX) $(INCLUDE_PATH) $(CXX_FLAGS) -c $(filter-out %_$(subst .o,.cxx,$(notdir $@)), $(filter %$(subst .o,.cxx,$(notdir $@)), $(TEST_SOURCES))) -o $@

.PHONY: clean
clean:
	$(RM) $(BUILD_DIR)

clean_obj:
	$(RM) $(OBJECTS_CLEAN)
