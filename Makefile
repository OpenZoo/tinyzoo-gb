#
# A Makefile that compiles all .c and .s files in "src" and "res" 
# subdirectories and places the output in a "obj" subdirectory
#

# If you move this project you can change the directory 
# to match your GBDK root directory (ex: GBDK_HOME = "C:/GBDK/"
GBDK_HOME = /opt/gbdk/

LCC = $(GBDK_HOME)bin/lcc 

# You can set flags for LCC here
# For example, you can uncomment the line below to turn on debug output
LCCFLAGS = -debug -Wm-yt27 -Wm-ya16 -Wm-yo4 -DGBZ80
# optimizations
LCCFLAGS += -Wf--max-allocs-per-node50000 -Wf--opt-code-speed
# profiling
# LCCFLAGS += -DPROFILING -Wf--profile
# ASM
LCCFLAGS += -Wa-I$(GBDK_HOME)lib/small/asxxxx/ -Wa-l
# GBC
LCCFLAGS += -Wm-yc -Wm-ys -Wm-yngbzoo

# You can set the name of the .gb ROM file here
PROJECTNAME    = gbzoo

SRCDIR      = src
OBJDIR      = obj
RESDIR      = res
BINS	    = $(OBJDIR)/$(PROJECTNAME).gb
CSOURCES    = $(foreach dir,$(SRCDIR),$(notdir $(wildcard $(dir)/*.c))) \
	$(foreach dir,$(SRCDIR)/elements,$(notdir $(wildcard $(dir)/*.c))) \
	$(foreach dir,$(RESDIR),$(notdir $(wildcard $(dir)/*.c)))
ASMSOURCES  = $(foreach dir,$(SRCDIR),$(notdir $(wildcard $(dir)/*.s))) \
	$(foreach dir,$(SRCDIR)/elements,$(notdir $(wildcard $(dir)/*.s)))
OBJS       = $(CSOURCES:%.c=$(OBJDIR)/%.o) $(ASMSOURCES:%.s=$(OBJDIR)/%.o)

all:	prepare $(BINS)

make.bat: Makefile
	@echo "REM Automatically generated from Makefile" > make.bat
	@make -sn | sed y/\\//\\\\/ | grep -v make >> make.bat

# Compile .c files in "src/" to .o object files
$(OBJDIR)/%.o:	$(SRCDIR)/%.c
	$(LCC) $(LCCFLAGS) -c -o $@ $<

# Compile .c files in "src/elements/" to .o object files
$(OBJDIR)/%.o:	$(SRCDIR)/elements/%.c
	$(LCC) $(LCCFLAGS) -c -o $@ $<

# Compile .c files in "res/" to .o object files
$(OBJDIR)/%.o:	$(RESDIR)/%.c
	$(LCC) $(LCCFLAGS) -c -o $@ $<

# Compile .s assembly files in "src/" to .o object files
$(OBJDIR)/%.o:	$(SRCDIR)/%.s
	$(LCC) $(LCCFLAGS) -c -o $@ $<

# If needed, compile .c files i n"src/" to .s assembly files
# (not required if .c is compiled directly to .o)
$(OBJDIR)/%.s:	$(SRCDIR)/%.c
	$(LCC) $(LCCFLAGS) -S -o $@ $<

# Link the compiled object files into a .gb ROM file
$(BINS):	$(OBJS)
	$(LCC) $(LCCFLAGS) -o $(BINS) $(OBJS)

prepare:
	mkdir -p $(OBJDIR)

clean:
#	rm -f  *.gb *.ihx *.cdb *.adb *.noi *.map
	rm -f  $(OBJDIR)/*.*

