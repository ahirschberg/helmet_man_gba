################################################################################
# These are variables for the GBA toolchain build
# You can add others if you wish to
# ***** YOUR NAME HERE *****
################################################################################

# The name of your desired GBA game
# This should be a just a name i.e MyFirstGBAGame
# No SPACES AFTER THE NAME.
PROGNAME = CoolGame
SRCDIR = src
BINDIR = bin

# The object files you want to compile into your program
CFILES = $(wildcard $(SRCDIR)/*.c) $(SRCDIR)/bios.c

# The header files you have created.
HFILES = $(wildcard $(SRCDIR)/*.h)
################################################################################
# These are various settings used to make the GBA toolchain work
# DO NOT EDIT BELOW.
################################################################################

TOOLDIR  = /usr/local/cs2110-tools
ARMLIB   = $(TOOLDIR)/arm-thumb-eabi/lib
CFLAGS   = -Wall -Werror -std=c99 -pedantic -Wextra
CFLAGS   += -mthumb-interwork -mlong-calls -nostartfiles -MMD -MP -I $(TOOLDIR)/include
LDFLAGS = -L $(TOOLDIR)/lib \
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

################################################################################
# These are the targets for the GBA build system
################################################################################

all : CFLAGS += $(CRELEASE)
all : $(PROGNAME).gba 
	@echo "[FINISH] Created $(PROGNAME).gba"
all : vba

.PHONY : all clean mkbin

$(PROGNAME).gba : mkbin $(BINDIR)/$(PROGNAME).elf
	@echo "[LINK] Linking objects together to create $(PROGNAME).gba"
	@$(OBJCOPY) -O binary $(BINDIR)/$(PROGNAME).elf $(PROGNAME).gba

$(BINDIR)/$(PROGNAME).elf : $(BINDIR)/crt0.o $(OFILES)
	@$(LD) $(LDFLAGS) -o $(BINDIR)/$(PROGNAME).elf $^ -lgcc -lc -lgcc $(LDDEBUG)
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
	@rm -rf $(BINDIR)

vba : CFLAGS += $(CRELEASE)
vba : $(PROGNAME).gba
	@echo "[EXECUTE] Running Emulator VBA-M"
	@vbam $(VBAOPT) $(PROGNAME).gba > /dev/null 2> /dev/null

mkbin :
	@mkdir -p $(BINDIR)

med : CFLAGS += $(CRELEASE)
med : $(PROGNAME).gba
	@echo "[EXECUTE] Running emulator Mednafen"
	@mednafen $(PROGNAME).gba > /dev/null 2>&1

-include $(CFILES:%.c=%.d)

