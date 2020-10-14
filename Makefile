# Commands
CC = g++
LD = g++

#Flags
options = -lncurses -lm -g -lstdc++fs
flags = -std=c++17 -Wall

# Build directories
BUILD_DIR = build
TARGET_DIR = $(BUILD_DIR)/target
OBJECT_DIR = $(BUILD_DIR)/object

# Objects
objects = $(patsubst src/%.cpp,$(OBJECT_DIR)/%.o,$(wildcard src/*.cpp))
out = sysmon

# Make all of the build directories
$(shell mkdir -p $(TARGET_DIR) 2> /dev/null)
$(shell mkdir -p $(OBJECT_DIR) 2> /dev/null)

# Some targets don't create files
.PHONY : all clean install uninstall

# Build all
all : $(TARGET_DIR)/sysmon

$(TARGET_DIR)/sysmon : $(objects)
	$(LD) $(objects) -o $@  $(options)

# Compile source
$(OBJECT_DIR)/%.o : src/%.cpp src/*.h
	$(CC) $(flags) -c $< -o $@

# Clean up the build directories
clean :
	-rm -rf $(TARGET_DIR) $(OBJECT_DIR) 2> /dev/null

# Install the executables
install : all
	cp $(TARGET_DIR)/sysmon /usr/local/bin/

# Uninstall the executables
uninstall :
	-rm /usr/local/bin/sysmon 2> /dev/null
