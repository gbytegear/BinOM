# User-defined arguments

## MODE:
### Debug - enable debug flags, disable optimisation
### Release - enable optimisation
MODE := Release

## TEST_AUTORUN:
### Yes - automatic run test after build
TEST_AUTORUN := Yes

## ENABLE_WARNINGS:
### Yes - print all warnings
### No - disable all warnings
ENABLE_WARNINGS := No

# Directories
## Build directories
BUILD_DIR := build
TEST_DIR  := $(BUILD_DIR)/test

# Targets
## $(BUILD_DIR)/$(LIB_TARGET).a
## $(BUILD_DIR)/$(LIB_TARGET).so
LIB_TARGET := libbinom
## $(BUILD_DIR)/$(TEST_EXEC)
TEST_EXEC  := binom_test



# Programms
CC    := gcc-11
CXX   := g++-11
RM    := rm -rf
MKDIR := mkdir -p



# Build flags
## C
C_MODE_FLAGS := 
ifeq ($(MODE),Debug) # Debug
	C_MODE_FLAGS += -O0 -g
else ifeq($(MODE),Release) # Release
	C_MODE_FLAGS += -O3
endif

C_WARNINGS_FLAGS := 
ifeq ($(ENABLE_WARNINGS),Yes)
	C_WARNINGS_FLAGS = -Wall
else ifeq($(ENABLE_WARNINGS),No)
	C_WARNINGS_FLAGS = -w
endif

C_FLAGS := -fPIC $(C_MODE_FLAGS)


## C++
CXX_VERSION := c++20

CXX_MODE_FLAGS := 
ifeq ($(MODE),Debug) # Debug
	CXX_MODE_FLAGS += -O0 -g -fno-inline-small-functions
else ifeq($(MODE),Release) # Release
	CXX_MODE_FLAGS += -O3
endif

CXX_WARNINGS_FLAGS := 
ifeq ($(ENABLE_WARNINGS),Yes)
	CXX_WARNINGS_FLAGS = -Wall
else ifeq($(ENABLE_WARNINGS),No)
	CXX_WARNINGS_FLAGS = -w
endif

CXX_FLAGS := -fPIC -std=$(CXX_VERSION) $(CXX_MODE_FLAGS) $(CXX_WARNINGS_FLAGS)



## Linker flags
LD_FLAGS =
LD_SHARED_FLAGS = -fPIC -shared


## Linker libs flags
LD_LIBS  = -lpthread


## Source directories
INCLUDE_PATH = -I./
SOURCES_PATH = ./libbinom/source/
TEST_SOURCES_PATH = ./test



# Files find rules
## Build
SOURCES = $(shell find $(SOURCES_PATH) -name "*.cxx")
OBJECTS = $(addprefix $(BUILD_DIR)/, $(notdir $(SOURCES:.cxx=.o)))
OBJECTS_CLEAN = $(shell find $(BUILD_DIR) -name "*.o")


## Test
TEST_SOURCES = $(shell find $(TEST_SOURCES_PATH) -name "*.cxx")
TEST_OBJECTS = $(addprefix $(BUILD_DIR)/test/, $(notdir $(TEST_SOURCES:.cxx=.o)))



# Public make flags
all: lib test

test: $(TEST_DIR)/$(TEST_EXEC)
ifeq ($(TEST_AUTORUN),Yes)
	$(TEST_DIR)/$(TEST_EXEC)
endif

lib: $(BUILD_DIR)/$(LIB_TARGET).a $(BUILD_DIR)/$(LIB_TARGET).so

$(BUILD_DIR):
	$(MKDIR) $@

$(TEST_DIR):
	$(MKDIR) $@

$(BUILD_DIR)/$(LIB_TARGET).a: $(OBJECTS)
	ar cr $@ $(OBJECTS)

$(BUILD_DIR)/$(LIB_TARGET).so: $(OBJECTS)
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
