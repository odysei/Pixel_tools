BIN_DIR = bin

EXE1 = $(BIN_DIR)/dump_error_files

SRC_DIR = src

OBJS1 = $(SRC_DIR)/dump_error_files_deco.o $(SRC_DIR)/dump_error_files.o

CXXFLAGS += --std=c++0x \
-pedantic -Wunused-variable -Werror=sign-compare \
-Wunused-but-set-variable -Werror=return-type -Werror=missing-braces \
#-Werror=delete-non-virtual-dtor -Werror=maybe-uninitialized \
-fPIC -O

# CXXFLAGS += -D OLD_COMPILER


################################################

all: $(BIN_DIR) $(EXE1)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(EXE1): $(OBJS1)
	$(CXX) $(CXXFLAGS) -o $(EXE1) $(OBJS1)

clean:
	rm -f $(OBJS1)
