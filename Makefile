# Compiler and flags
CXX := g++
CXXFLAGS := -Wall -Wextra -std=c++11

# Directories
SRCDIR := .
BUILDDIR := build
TARGET := output

# Find all C++ files
SRCS := $(shell find $(SRCDIR) -name '*.cc')
OBJS := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SRCS:.cc=.o))
DEPS := $(OBJS:.o=.d)

# Include directories
INC := -Iinclude

# Targets
$(TARGET): $(OBJS)
	@echo " Linking..."
	$(CXX) $^ -o $(TARGET)

# Compile
$(BUILDDIR)/%.o: $(SRCDIR)/%.cc
	@mkdir -p $(@D)
	@echo " Compiling $<..."
	$(CXX) $(CXXFLAGS) $(INC) -MMD -c -o $@ $<

# Clean
clean:
	@echo " Cleaning..."
	$(RM) -r $(BUILDDIR) $(TARGET)

# Include dependencies
-include $(DEPS)

.PHONY: clean