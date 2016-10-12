BIN_DIR = bin

EXE1 = $(BIN_DIR)/dump_error_files_piggy
EXE2 = $(BIN_DIR)/dump_error_files_uTCA

SRC_DIR = src
SRC_DIR1 = $(SRC_DIR)/VME_piggy
SRC_DIR2 = $(SRC_DIR)/uTCA

OBJS = $(SRC_DIR)/dump_error_files.o $(SRC_DIR)/process_file.o
OBJS1 = $(SRC_DIR1)/decoder.o
OBJS2 = $(SRC_DIR2)/decoder.o

INC = include

CXXFLAGS += --std=c++0x \
-pedantic -Wunused-variable -Werror=sign-compare \
-Wunused-but-set-variable -Werror=return-type -Werror=missing-braces \
-fPIC -O \
-I $(INC) -I $(SRC_DIR)
# add for newer compilers:
#-Werror=maybe-uninitialized
#-Werror=delete-non-virtual-dtor

# CXXFLAGS += -D OLD_COMPILER


################################################

all: $(BIN_DIR) $(EXE1) $(EXE2)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(EXE1): $(OBJS) $(OBJS1)
	$(CXX) $(CXXFLAGS) -o $(EXE1) $(OBJS) $(OBJS1)

$(EXE2): $(OBJS) $(OBJS2)
	$(CXX) $(CXXFLAGS) -o $(EXE2) $(OBJS) $(OBJS2)

clean:
	rm -f $(EXE1); \
	rm -f $(OBJS1); \
	rm -f $(EXE2); \
	rm -f $(OBJS2)
