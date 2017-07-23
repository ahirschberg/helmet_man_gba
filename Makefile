# No SPACES AFTER THE NAME.
PROGNAME = CoolGame
SRCDIR   = src
BINDIR   = bin

# We use globstar in most places, this list is for mkbin
# Pending future cleanup?
SRC_SUBDIRS = assets game render tonc_lib

# The object files you want to compile into your program
CFILES   = $(wildcard $(SRCDIR)/*.c) $(wildcard $(SRCDIR)/**/*.c) $(SRCDIR)/bios.c
HFILES   = $(wildcard $(SRCDIR)/*.h) $(wildcard $(SRCDIR)/**/*.h)

TOOLDIR  = /usr/local/cs2110-tools
ARMLIB   = $(TOOLDIR)/arm-thumb-eabi/lib
CFLAGS   = -Wall -Werror -std=c99 -pedantic -Wextra
CFLAGS  += -mthumb-interwork -mlong-calls -nostartfiles -MMD -MP -I $(TOOLDIR)/include
LDFLAGS  = -L $(TOOLDIR)/lib \
 		   -L $(TOOLDIR)/lib/gcc/arm-thumb-eabi/4.4.1/thumb \
		   -L $(ARMLIB) \
		   --script $(ARMLIB)/arm-gba.ld
CDEBUG   = -g -DDEBUG
CRELEASE = -O2
CC       = $(TOOLDIR)/bin/arm-thumb-eabi-gcc
AS       = $(TOOLDIR)/bin/arm-thumb-eabi-as
LD       = $(TOOLDIR)/bin/arm-thumb-eabi-ld
OBJCOPY  = $(TOOLDIR)/bin/arm-thumb-eabi-objcopy
GDB      = $(TOOLDIR)/bin/arm-thumb-eabi-gdb
OFILES   = $(CFILES:$(SRCDIR)/%.c=$(BINDIR)/%.o)
GAME_ELF = $(BINDIR)/$(PROGNAME).elf

all : CFLAGS += $(CRELEASE)
all : $(PROGNAME).gba
	@echo "[FINISH] Created $(PROGNAME).gba"
all : vba

.PHONY : all clean mkbin

$(PROGNAME).gba : mkbin $(GAME_ELF)
	@echo "[LINK] Linking objects together to create $(PROGNAME).gba"
	@$(OBJCOPY) -O binary $(GAME_ELF) $(PROGNAME).gba

$(GAME_ELF) : $(BINDIR)/crt0.o $(OFILES)
	@$(LD) $(LDFLAGS) -o $(GAME_ELF) $^ -lgcc -lc -lgcc $(LDDEBUG)
	@rm -f *.d

$(BINDIR)/crt0.o : $(ARMLIB)/crt0.s
	@$(AS) -mthumb-interwork $^ -o $(BINDIR)/crt0.o


$(BINDIR)/%.o : $(SRCDIR)/%.c
	@echo "[COMPILE] Compiling $<"
	@$(CC) $(CFLAGS) -c $< -o $@

$(BINDIR)/%.o : $(SRCDIR)/%.s
	@echo "[ASSEMBLE] Assembling $<"
	@$(AS) $< -o $@ -mthumb -mthumb-interwork

clean :
	@echo "[CLEAN] Removing all compiled files"
	@rm -rf $(BINDIR) $(PROGNAME).gba

vba : CFLAGS += $(CRELEASE)
vba : $(PROGNAME).gba
	@echo "[EXECUTE] Running Emulator VBA-M"
	@vbam $(VBAOPT) $(PROGNAME).gba > /dev/null 2> /dev/null

# Create all the necessary subdirectories from src
# TODO: make this cleaner
mkbin :
	$(foreach d,$(SRC_SUBDIRS), mkdir -p $(BINDIR)/$(d);)

med : CFLAGS += $(CRELEASE)
med : $(PROGNAME).gba
	@echo "[EXECUTE] Running emulator Mednafen"
	@mednafen $(PROGNAME).gba > /dev/null 2>&1

-include $(CFILES:%.c=%.d)

