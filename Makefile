#
# A Makefile that compiles all .c and .s files in "src" and "res" 
# subdirectories and places the output in a "obj" subdirectory
#

# If you move this project you can change the directory 
# to match your GBDK root directory (ex: GBDK_HOME = "C:/GBDK/"
GBDK_HOME = /opt/gbdk/
LCC = $(GBDK_HOME)bin/lcc
TARGETS = gb gb_tpp1 pocket gg

EXT_gb = gb
EXT_gb_tpp1 = gb
EXT_pocket = pocket
EXT_gg = gg
PLATDIR_gb = platform_gb
PLATDIR_gb_tpp1 = platform_gb
PLATDIR_gb_pocket = platform_gb
PLATDIR_gg = platform_gg
LCCFLAGS_gb = -Wm-yt27 -DSM83 -D__GB__
LCCFLAGS_gb_tpp1 = -Wm-yt27 -DSM83 -D__GB__ -D__TPP1__
LCCFLAGS_pocket = -Wm-yt27 -DSM83 -D__POCKET__
LCCFLAGS_gg = -DZ80 -D__GG__

# You can set flags for LCC here
# For example, you can uncomment the line below to turn on debug output
LCCFLAGS = -debug -Wm-ya16 -Wm-yo4
# optimizations
LCCFLAGS += -Wf--max-allocs-per-node50000 -Wf--opt-code-speed
# profiling
# LCCFLAGS += -DPROFILING -Wf--profile
# ASM
LCCFLAGS += -Wa-I$(GBDK_HOME)lib/small/asxxxx/ -Wa-l
# GBC
LCCFLAGS += -Wm-yc -Wm-ys -Wm-yngbzoo

EXT = $(EXT_$(TARGET))
PLATDIR = $(PLATDIR_$(TARGET))
LCCFLAGS += $(LCCFLAGS_$(TARGET)) -I$(SRCDIR)/$(PLATDIR) -I$(SRCDIR)

# You can set the name of the .gb ROM file here
PROJECTNAME    = gbzoo

SRCDIR      = src
OBJDIR      = obj/$(TARGET)
RESDIR      = res
MKDIRS      = $(OBJDIR)
BINS	    = $(OBJDIR)/$(PROJECTNAME).$(EXT)
CSOURCES    = $(foreach dir,$(SRCDIR),$(notdir $(wildcard $(dir)/*.c))) \
	$(foreach dir,$(SRCDIR)/elements,$(notdir $(wildcard $(dir)/*.c))) \
	$(foreach dir,$(SRCDIR)/$(PLATDIR),$(notdir $(wildcard $(dir)/*.c))) \
	$(foreach dir,$(RESDIR),$(notdir $(wildcard $(dir)/*.c)))
ASMSOURCES  = $(foreach dir,$(SRCDIR),$(notdir $(wildcard $(dir)/*.s))) \
	$(foreach dir,$(SRCDIR)/elements,$(notdir $(wildcard $(dir)/*.s))) \
	$(foreach dir,$(SRCDIR)/$(PLATDIR),$(notdir $(wildcard $(dir)/*.s)))
OBJS       = $(CSOURCES:%.c=$(OBJDIR)/%.o) $(ASMSOURCES:%.s=$(OBJDIR)/%.o)

all: $(TARGETS)

make.bat: Makefile
	@echo "REM Automatically generated from Makefile" > make.bat
	@make -sn | sed y/\\//\\\\/ | grep -v make >> make.bat

# Compile .c files in "src/" to .o object files
$(OBJDIR)/%.o:	$(SRCDIR)/%.c
	$(LCC) $(LCCFLAGS) -c -o $@ $<

# Compile .c files in "src/elements/" to .o object files
$(OBJDIR)/%.o:	$(SRCDIR)/elements/%.c
	$(LCC) $(LCCFLAGS) -c -o $@ $<

$(OBJDIR)/%.o:	$(SRCDIR)/$(PLATDIR)/%.c
	$(LCC) $(LCCFLAGS) -c -o $@ $<

# Compile .c files in "res/" to .o object files
$(OBJDIR)/%.o:	$(RESDIR)/%.c
	$(LCC) $(LCCFLAGS) -c -o $@ $<

# Compile .s assembly files in "src/" to .o object files
$(OBJDIR)/%.o:	$(SRCDIR)/%.s
	$(LCC) $(LCCFLAGS) -c -o $@ $<

$(OBJDIR)/%.o:	$(SRCDIR)/$(PLATDIR)/%.s
	$(LCC) $(LCCFLAGS) -c -o $@ $<

# If needed, compile .c files i n"src/" to .s assembly files
# (not required if .c is compiled directly to .o)
$(OBJDIR)/%.s:	$(SRCDIR)/%.c
	$(LCC) $(LCCFLAGS) -S -o $@ $<

# Link the compiled object files into a .gb ROM file
$(BINS):	$(OBJS)
	$(LCC) $(LCCFLAGS) -o $(BINS) $(OBJS)

clean:
	@echo Cleaning
	@for target in $(TARGETS); do \
		$(MAKE) $$target-clean; \
	done

include Makefile.targets

# create necessary directories after Makefile is parsed but before build
# info prevents the command from being pasted into the makefile
ifneq ($(strip $(TARGET)),)           # Only make the directories if EXT has been set by a target
$(info $(shell mkdir -p $(MKDIRS)))
endif
