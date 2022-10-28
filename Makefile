TARGET_EXEC := find_dependencies

BUILD_DIR := ./build
SRC_DIRS := ./src

SRCS := $(shell find $(SRC_DIRS) -name '*.cpp')

OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

INC_DIRS := $(shell find $(SRC_DIRS) -type d)
INC_DIRS += /home/eliyahub/work/spot/include
INC_FLAGS := $(addprefix -I,$(INC_DIRS))

CPPFLAGS := -std=c++17 $(INC_FLAGS) -MMD -MP

# TODO: find a better way to do it...
LDFLAGS += -L/home/*****/work/spot/lib '-Wl,-R$$ORIGIN' -lbddx -lspot

$(BUILD_DIR)/$(TARGET_EXEC): $(OBJS)
	$(CXX) $(OBJS) -o $@ $(LDFLAGS)

$(BUILD_DIR)/%.cpp.o: %.cpp
	mkdir -p $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -r $(BUILD_DIR)


