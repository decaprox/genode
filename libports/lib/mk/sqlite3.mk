include $(REP_DIR)/ports/sqlite3.inc

SQLITE_DIR = $(REP_DIR)/contrib/$(SQLITE)
LIBS    += libc
INC_DIR += $(SQLITE_DIR)
SRC_C    = sqlite3.c
CC_WARN  =

vpath %.c $(SQLITE_DIR)

SHARED_LIB = yes
