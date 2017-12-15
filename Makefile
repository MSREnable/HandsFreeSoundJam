default: ; @echo "Usage: make [windows|windows-highres|linux|osx]"
.PHONY: transfer windows windows-highres linux osx

DSPDIR=src/dsp
LIBDIR=lib

NAME=MicrosoftHandsFreeSoundJam

CFLAGS = -I$(LIBDIR)/nanovg/src/ -Wall
CFLAGS += -DGLFW_INCLUDE_EXT -DNANOVG_GL2_IMPLEMENTATION 
CXXFLAGS += -I$(LIBDIR)/rtaudio
UI_OBJ += audio.o hit.o timer.o button.o ui.o btnreg.o gaze.o
UI_OBJ += piano.o editpanel.o pianoroll.o toys.o config.o presets.o 
UI_OBJ += loopmode.o

MAIN_O = src/ui/main.o

# DSP flags 

CFLAGS += -DPD -O3 -funroll-loops -fomit-frame-pointer -Wall -fPIC -g
CFLAGS += -I. -I$(LIBDIR)/loadflac/ -I$(LIBDIR)/sqlite
CFLAGS += -I/usr/local/include 

DSP_OBJ= $(addprefix src/dsp/, trinity/trinity.o\
	 jam/tracks.o\
	 jam/eyejam.o\
	 jam/mixer.o\
	 jam/reverb.o\
	 jam/delay.o\
	 jam/clip.o\
	 jam/db.o\
	 jam/xy.o\
	 jam/edit.o\
	 drumkit/drumkit.o\
	 presets.o\
	 surgeon/surgeon.o\
	 surgeon/surgeon_presets.o\
	 sp.o)

CXXFLAGS += -I$(DSPDIR) -Isrc
CFLAGS += -I$(DSPDIR) -Isrc

OBJ += $(addprefix src/ui/, $(UI_OBJ)) $(DSP_OBJ)

#CFLAGS += -DFULLSCREEN

CFLAGS += -I$(LIBDIR)/SF1eFilter

LIB_OBJ += $(LIBDIR)/nanovg/src/nanovg.o \
       $(LIBDIR)/rtaudio/RtAudio.o \
       $(LIBDIR)/loadflac/loadflac.o\
       $(LIBDIR)/sqlite/sqlite3.o\

OBJ+=$(LIB_OBJ)

# TODO: merge respective object files into DSP_OBJ and UI_OBJ categories
# this needs to be done to make it easier to make offline tests
MOONGAZE_OBJ = $(addprefix src/moongazing/, draw.o moon.o)
OBJ += $(MOONGAZE_OBJ)
DSP_OBJ += src/moongazing/synth.o

ARACHNOID_OBJ = $(addprefix src/arachnoid/, circ.o draw.o)
OBJ += $(ARACHNOID_OBJ)
DSP_OBJ += src/arachnoid/audio.o

ifdef USE_ONEEURO
OBJ += $(LIBDIR)/SF1eFilter/SF1eFilter.o
endif

AUX_OBJ+=$(DSP_OBJ) $(LIB_OBJ) 

clean:
	rm -rf $(OBJ) $(NAME) dsp_test jamedit

$(NAME): $(OBJ) $(MAIN_O)
	$(CC) $(CFLAGS) -DBUILD_MAIN $(MAIN_O) $(OBJ) -o $@ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@ 

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@ 

dsp_test: src/dsp/dsp_test.c $(AUX_OBJ)
	$(CC) $(CFLAGS) $< -o $@ $(AUX_OBJ) $(LDFLAGS) 

windows:
	make -f Makefile -f Makefile.windows $(NAME)

windows-highres:
	HIGHRES=1 make -f Makefile -f Makefile.windows $(NAME)

linux:
	make -f Makefile -f Makefile.linux $(NAME)

osx:
	make -f Makefile -f Makefile.osx $(NAME)

linux_debug:
	make -f Makefile -f Makefile.linux debug

# to make building jamedit easier on linux, "je" phony target is used 
je: 
	make -f Makefile -f Makefile.linux jamedit

jamedit: src/dsp/runt/jamedit.c $(AUX_OBJ)
	$(CC) $(CFLAGS) -I$(HOME)/.runt/include \
		$< -o $@ $(AUX_OBJ) -L$(HOME)/.runt/lib $(LDFLAGS) -lrunt 

transfer: 
	rsync -rvt src/dsp/samples .
	rsync -rvt $(LIBDIR)/tobii_stream_engine.dll .

