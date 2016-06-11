## -*- Makefile -*-
##

#### Compiler and tool definitions shared by all build targets #####
CCC = g++
CXX = g++
BASICOPTS = -g
CCFLAGS = $(BASICOPTS)
CXXFLAGS = $(BASICOPTS)  -std=c++11

# Define the target directories.
TARGET_DIR=Build

all: $(TARGET_DIR)/encode $(TARGET_DIR)/decode

## Target: encode
CPPFLAGS_encode = \
	-I.
OBJS_encode =  \
	$(TARGET_DIR)/encode.o \
	$(TARGET_DIR)/bwt.o \
	$(TARGET_DIR)/huffman.o \
	$(TARGET_DIR)/runlength.o \
	$(TARGET_DIR)/utils.o
SYSLIBS_encode = -lm 
USERLIBS_encode = $(SYSLIBS_encode) 
DEPLIBS_encode =  
LDLIBS_encode = $(USERLIBS_encode)


# Link or archive
$(TARGET_DIR)/encode: $(TARGET_DIR) $(OBJS_encode) $(DEPLIBS_encode)
	$(LINK.cc) $(CCFLAGS_encode) $(CPPFLAGS_encode) -o $@ $(OBJS_encode) $(LDLIBS_encode)

# Compile source files into .o files
$(TARGET_DIR)/encode.o: $(TARGET_DIR) encode.cpp
	$(COMPILE.cc) $(CCFLAGS_encode) $(CPPFLAGS_encode) -o $@ encode.cpp

$(TARGET_DIR)/bwt.o: $(TARGET_DIR) bwt.cpp
	$(COMPILE.cc) $(CCFLAGS_encode) $(CPPFLAGS_encode) -o $@ bwt.cpp

$(TARGET_DIR)/huffman.o: $(TARGET_DIR) huffman.cpp
	$(COMPILE.cc) $(CCFLAGS_encode) $(CPPFLAGS_encode) -o $@ huffman.cpp

$(TARGET_DIR)/runlength.o: $(TARGET_DIR) runlength.cpp
	$(COMPILE.cc) $(CCFLAGS_encode) $(CPPFLAGS_encode) -o $@ runlength.cpp

$(TARGET_DIR)/utils.o: $(TARGET_DIR) utils.cpp
	$(COMPILE.cc) $(CCFLAGS_encode) $(CPPFLAGS_encode) -o $@ utils.cpp


## Target: decode
CPPFLAGS_decode = \
	-I.
OBJS_decode =  \
	$(TARGET_DIR)/decode.o \
	$(TARGET_DIR)/bwt.o \
	$(TARGET_DIR)/huffman.o \
	$(TARGET_DIR)/runlength.o \
	$(TARGET_DIR)/utils.o
SYSLIBS_decode = -lm 
USERLIBS_decode = $(SYSLIBS_decode) 
DEPLIBS_decode =  
LDLIBS_decode = $(USERLIBS_decode)


$(TARGET_DIR)/decode: $(TARGET_DIR) $(OBJS_decode) $(DEPLIBS_decode)
	$(LINK.cc) $(CCFLAGS_decode) $(CPPFLAGS_decode) -o $@ $(OBJS_decode) $(LDLIBS_decode)
$(TARGET_DIR)/decode.o: $(TARGET_DIR) decode.cpp
	$(COMPILE.cc) $(CCFLAGS_encode) $(CPPFLAGS_encode) -o $@ decode.cpp


#### Clean target deletes all generated files ####
clean:
	rm -rf \
		$(TARGET_DIR)/encode \
		$(TARGET_DIR)/decode \
		$(TARGER_DIR)/*.o


# Create the target directory (if needed)
$(TARGET_DIR):
	mkdir -p $(TARGET_DIR)

# Enable dependency checking
.KEEP_STATE:
.KEEP_STATE_FILE:.make.state.GNU-amd64-Linux

