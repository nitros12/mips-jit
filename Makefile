##
# Mips Jit
#
# @file
# @version 0.1

CC = clang

EXE = mips_jit

SRC_DIR = src
OBJ_DIR = obj

MKDIR_P = mkdir -p

SRC = $(wildcard $(SRC_DIR)/*.c)
OBJ = $(SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

CFLAGS += -Wall -flto
LDFLAGS += -fuse-ld=lld

.PHONY: ensuredirs all clean

all: ensuredirs $(EXE)

$(EXE): $(OBJ)
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

clean:
	$(RM) $(OBJ)

ensuredirs: ${OBJ_DIR}

${OBJ_DIR}:
	${MKDIR_P} ${OBJ_DIR}

# end
