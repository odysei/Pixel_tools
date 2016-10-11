BIN_DIR = bin

EXE1 = $(BIN_DIR)/dump_error_files

SRC_DIR = src
SRC_DIR1 = $(SRC_DIR)/VME_piggy

INC = include

OBJS = $(SRC_DIR)/dump_error_files.o $(SRC_DIR)/process_file.o
OBJS1 = $(SRC_DIR1)/decoder.o

CXXFLAGS += --std=c++0x \
-pedantic -Wunused-variable -Werror=sign-compare \
-Wunused-but-set-variable -Werror=return-type -Werror=missing-braces \
-Werror=maybe-uninitialized -fPIC -O \
-I $(INC) -I $(SRC_DIR)
#-Werror=delete-non-virtual-dtor

# CXXFLAGS += -D OLD_COMPILER


################################################

all: $(BIN_DIR) $(EXE1)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(EXE1): $(OBJS) $(OBJS1)
	$(CXX) $(CXXFLAGS) -o $(EXE1) $(OBJS) $(OBJS1)

clean:
	rm -f $(OBJS1)
