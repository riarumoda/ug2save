# ug2save - NFSU2 profile creator
#
#   make              build out/ug2save
#   make check        build and run the test suite
#   make clean        remove object files (keeps the built binary)
#   make mrproper     remove everything generated (the whole out/ directory)
#   make V=1          show full compiler command lines
#   make OUT=build    build somewhere else

NAME    := ug2save
OUT     ?= out
OBJDIR  := $(OUT)/obj
CC      ?= gcc

ifeq ($(OS),Windows_NT)
EXE := .exe
endif
TARGET  := $(OUT)/$(NAME)$(EXE)

PREFIX  ?= /usr/local
CFLAGS  ?= -O2
override CFLAGS += -Wall -Wextra -I. -MMD -MP

SRCS := main.c \
        functions/cli.c \
        functions/cars.c \
        functions/parts-data.c \
        functions/hex-utils.c \
        functions/save-modifier.c
OBJS := $(SRCS:%.c=$(OBJDIR)/%.o)
DEPS := $(OBJS:.o=.d)

ifeq ($(V),1)
Q :=
else
Q := @
endif

.PHONY: all check clean mrproper install uninstall help

all: $(TARGET)

$(TARGET): $(OBJS)
	@mkdir -p $(dir $@)
	@echo "  LD      $@"
	$(Q)$(CC) $(LDFLAGS) -o $@ $(OBJS) $(LDLIBS)

$(OBJDIR)/%.o: %.c
	@mkdir -p $(dir $@)
	@echo "  CC      $<"
	$(Q)$(CC) $(CFLAGS) -c $< -o $@

check: $(TARGET)
	@sh tests/check.sh $(TARGET) $(OUT)/check

# Remove intermediate files only; the binary in $(OUT)/ is kept.
clean:
	@echo "  CLEAN   $(OBJDIR) $(OUT)/check"
	$(Q)rm -rf $(OBJDIR) $(OUT)/check

# Remove everything that was generated, including the output directory.
mrproper:
	@case "$(OUT)" in ""|"."|".."|"/"|*/..) echo "refusing to remove OUT='$(OUT)'"; exit 1;; esac
	@echo "  MRPROPER $(OUT)"
	$(Q)rm -rf $(OUT)

install: $(TARGET)
	install -d $(DESTDIR)$(PREFIX)/bin
	install -m 755 $(TARGET) $(DESTDIR)$(PREFIX)/bin/$(NAME)$(EXE)

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/$(NAME)$(EXE)

help:
	@echo "Targets: all (default), check, clean, mrproper, install, uninstall"
	@echo "Variables: OUT=$(OUT) CC=$(CC) PREFIX=$(PREFIX) V=0|1"

-include $(DEPS)
