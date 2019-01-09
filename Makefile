TARGET		:= HEXGEM
TITLE	    := HEXGEM001
SRCDIR		:= src
BINDIR  	:= ./bin
INCLUDES	:= .



LIBS =  -lSDL2_mixer -lSDL2  -lvita2d \
	-lvorbisfile -lvorbis -logg -lmpg123 -lmikmod -lstdc++ \
	-lm -lSceCtrl_stub -lScePower_stub -lSceCommonDialog_stub  \
	-lSceGxm_stub -lSceSysmodule_stub -lSceHid_stub -lFLAC -lSceAudio_stub \
	-lSceDisplay_stub -lSceTouch_stub -lc


SRCS := \
src/board.o \
src/fskip.o \
src/hiscore.o \
src/main.o \
src/random.o \
src/screen.o \
src/sound.o

RSOBJS := $(RSSRCS:.cpp=.o)
RSOBJS := $(foreach f,$(RSOBJS),$(BINDIR)/$(f))
	
CFILES   := $(foreach dir,$(SOURCES), $(wildcard $(dir)/*.c))
CPPFILES   := $(SRCS)
BINFILES := $(foreach dir,$(DATA), $(wildcard $(dir)/*.bin))
OBJS     := $(addsuffix .o,$(BINFILES)) $(CFILES:.c=.o) $(CPPFILES:.cpp=.o) 

export INCLUDE	:= $(foreach dir,$(INCLUDES),-I$(CURDIR)/$(dir))


PREFIX  = arm-vita-eabi
CC      = $(PREFIX)-gcc
CXX      = $(PREFIX)-g++
CFLAGS  = $(INCLUDE) -g -Wl,-q -O2 -fsigned-char -w
CFLAGS	+= $(INCLUDE) -D__vita__

# includes ...
CFLAGS += -I$(SRCDIR)
CFLAGS += -I$(VITASDK)/$(PREFIX)/include/SDL2


export INCLUDE	:= $(foreach dir,$(INCLUDES),-I$(CURDIR)/$(dir))



CXXFLAGS  = $(CFLAGS) -fno-exceptions -std=gnu++11 -fpermissive
ASFLAGS = $(CFLAGS)

all: $(TARGET).vpk

$(TARGET).vpk: $(TARGET).velf
	vita-make-fself -s $< build/eboot.bin
	vita-mksfoex -s TITLE_ID=$(TITLE) "$(TARGET)" param.sfo
	cp -f param.sfo sce_sys/param.sfo
	
	#------------ Comment this if you don't have 7zip ------------------
	7z a -tzip ./$(TARGET).vpk -r ./sce_sys ./build/eboot.bin
	#-------------------------------------------------------------------

%.velf: %.elf
	cp $< $<.unstripped.elf
	$(PREFIX)-strip -g $<
	vita-elf-create $< $@

$(TARGET).elf: $(OBJS)
	$(CXX) $(CXXFLAGS) $^ $(LIBS) -o $@

clean:
	@rm -rf $(TARGET).velf $(TARGET).elf $(TARGET).vpk $(TARGET).elf.unstripped.elf $(OBJS)

