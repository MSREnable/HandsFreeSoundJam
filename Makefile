default: ; @echo "Usage: make [windows|windows-highres|linux|osx]"
.PHONY: transfer windows windows-highres linux osx

DSPDIR=src/dsp
LIBDIR=lib

NAME=MicrosoftHandsFreeSoundJam

CFLAGS = -I$(LIBDIR)/nanovg/src/ -Wall
CFLAGS += -DGLFW_INCLUDE_EXT -DNANOVG_GL2_IMPLEMENTATION 
CXXFLAGS += -I$(LIBDIR)/rtaudio
UI_OBJ += main.o audio.o hit.o timer.o button.o ui.o btnreg.o gaze.o
UI_OBJ += piano.o editpanel.o pianoroll.o toys.o


# DSP flags 

CFLAGS += -DPD -O3 -funroll-loops -fomit-frame-pointer -Wall -fPIC -g
CFLAGS += -I. -I$(LIBDIR)/loadflac/ -I$(LIBDIR)/sqlite
CFLAGS += -I/usr/local/include 

DSP_OBJ= trinity/trinity.o\
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
	 sp.o\

CXXFLAGS += -I$(DSPDIR) -Isrc
CFLAGS += -I$(DSPDIR) -Isrc

OBJ += $(addprefix src/ui/, $(UI_OBJ)) $(addprefix src/dsp/, $(DSP_OBJ))

#CFLAGS += -DFULLSCREEN

CFLAGS += -I$(LIBDIR)/SF1eFilter

OBJ += $(LIBDIR)/nanovg/src/nanovg.o \
       $(LIBDIR)/rtaudio/RtAudio.o \
       $(LIBDIR)/loadflac/loadflac.o\
       $(LIBDIR)/sqlite/sqlite3.o\


MOONGAZE_OBJ = draw.o moon.o synth.o

OBJ += $(addprefix src/moongazing/, $(MOONGAZE_OBJ))

ARACHNOID_OBJ = audio.o circ.o draw.o
OBJ += $(addprefix src/arachnoid/, $(ARACHNOID_OBJ))

ifdef USE_ONEEURO
OBJ += $(LIBDIR)/SF1eFilter/SF1eFilter.o
endif

clean:
	rm -rf $(OBJ) $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) -DBUILD_MAIN $(OBJ) -o $@ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@ 

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@ 

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

transfer: 
	rsync -rvt src/dsp/samples .
	rsync -rvt $(LIBDIR)/tobii_stream_engine.dll .

