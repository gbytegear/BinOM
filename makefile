# User-defined arguments

## MODE:
### Debug - enable debug flags, disable optimisation
### Release - enable optimisation
MODE := Release
TEST_AUTORUN := Yes

# Programms
CC = gcc-11
CXX = g++-11
RM = rm -rf
MKDIR = mkdir -p

# Build flags
## C
C_FLAGS := -fPIC
ifeq ($(MODE),Debug) # Debug
	C_FLAGS = -O0 -g -Wall
else ifeq($(MODE),Release) # Release
	C_FLAGS = -O3 -w
endif
## C++
CXX_VERSION := c++20
CXX_FLAGS := -fPIC -std=$(CXX_VERSION)
ifeq ($(MODE),Debug) # Debug
	CXX_FLAGS += -O0 -g -Wall -fno-inline-small-functions
else ifeq($(MODE),Release) # Release
	CXX_FLAGS += -O3 -w
endif
## Linker flags
LD_FLAGS =
LD_SHARED_FLAGS = -fPIC -shared
## Linker libs flags
LD_LIBS  = -lpthread -lseccomp

# Targets
LIB_TARGET = libbinom
TEST_EXEC = binom_test

# Directories
## Build directories
BUILD_DIR = build
TEST_DIR = $(BUILD_DIR)/test
## Source directories
INCLUDE_PATH = -I./
SOURCES_PATH = ./libbinom/source/
TEST_SOURCES_PATH = ./test

# Files find rules
SOURCES = $(shell find $(SOURCES_PATH) -name "*.cxx")
OBJECTS = $(addprefix $(BUILD_DIR)/, $(notdir $(SOURCES:.cxx=.o)))
OBJECTS_CLEAN = $(shell find $(BUILD_DIR) -name "*.o")

TEST_SOURCES = $(shell find $(TEST_SOURCES_PATH) -name "*.cxx")
TEST_OBJECTS = $(addprefix $(BUILD_DIR)/test/, $(notdir $(TEST_SOURCES:.cxx=.o)))

# Public make flags
all: test lib

test: $(TEST_DIR)/$(TEST_EXEC)
ifeq ($(TEST_AUTORUN),Yes)
	$(TEST_DIR)/$(TEST_EXEC)
endif

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
