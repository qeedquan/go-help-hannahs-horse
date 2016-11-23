# Makefile for "Help Hannah's Horse"
# This Makefile kindly contributed by Eero Tamminen (Thanks Eero!)

# Install options
#
# For local install, comment the next line and uncomment the one after
DATA_INSTALL_DIR=/usr/local/games/hhh
#DATA_INSTALL_DIR=.

# Where the binary goes (you might want /usr/bin instead)
BIN_DIR=/usr/local/bin

# Where the hiscore table is saved: user's home dir/.hannah
#SCORE_DIR=${HOME}/.hannah
#SCORE_DIR=.


# compiler options
CXX       = g++
CPPFLAGS = $(shell sdl-config --cflags) -DDATA_INSTALL_DIR=\"${DATA_INSTALL_DIR}\" #-DSCORE_DIR=\"${SCORE_DIR}\"
CXXFLAGS   = -O2 -Wall -g
LIBS     = $(shell sdl-config --libs) -lSDL_image -lSDL_ttf -lSDL_mixer

# target binary and directory name
OUT = hhh
# source and object files
SRCS = $(wildcard *.cc)
OBJS = $(patsubst %.cc, %.o, $(SRCS))

# what to build
$(OUT): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(OUT) $(OBJS) $(LIBS)

# how to compile source code to object files
.cc.o:
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

# automatic dependencies
deps: $(SRCS)
	gcc $(CPPFLAGS) -MM $^ > deps

-include deps


install: hhh	
	install -m 0755 -s hhh $(BIN_DIR)
	install -d -D -m 0755 $(DATA_INSTALL_DIR)/hannah/default $(DATA_INSTALL_DIR)/hannah/die $(DATA_INSTALL_DIR)/hannah/invis $(DATA_INSTALL_DIR)/red/flash $(DATA_INSTALL_DIR)/red/default $(DATA_INSTALL_DIR)/pink/flash $(DATA_INSTALL_DIR)/pink/default $(DATA_INSTALL_DIR)/blue/default $(DATA_INSTALL_DIR)/blue/flash $(DATA_INSTALL_DIR)/images  $(DATA_INSTALL_DIR)/maps $(DATA_INSTALL_DIR)/sounds $(DATA_INSTALL_DIR)/carrot/default $(DATA_INSTALL_DIR)/stop/default $(DATA_INSTALL_DIR)/freeze/default $(DATA_INSTALL_DIR)/invis/default $(DATA_INSTALL_DIR)/invert/default 
	install -m 0755 hannah/default/* $(DATA_INSTALL_DIR)/hannah/default
	install -m 0755 hannah/die/* $(DATA_INSTALL_DIR)/hannah/die
	install -m 0755 hannah/invis/* $(DATA_INSTALL_DIR)/hannah/invis
	install -m 0755 red/default/* $(DATA_INSTALL_DIR)/red/default
	install -m 0755 red/flash/* $(DATA_INSTALL_DIR)/red/flash
	install -m 0755 pink/default/* $(DATA_INSTALL_DIR)/pink/default
	install -m 0755 pink/flash/* $(DATA_INSTALL_DIR)/pink/flash
	install -m 0755 blue/default/* $(DATA_INSTALL_DIR)/blue/default
	install -m 0755 blue/flash/* $(DATA_INSTALL_DIR)/blue/flash
	install -m 0755 carrot/default/* $(DATA_INSTALL_DIR)/carrot/default
	install -m 0755 freeze/default/* $(DATA_INSTALL_DIR)/freeze/default
	install -m 0755 invis/default/* $(DATA_INSTALL_DIR)/invis/default
	install -m 0755 invert/default/* $(DATA_INSTALL_DIR)/invert/default
	install -m 0755 sounds/* $(DATA_INSTALL_DIR)/sounds
	install -m 0755 images/* $(DATA_INSTALL_DIR)/images
	install -m 0755 maps/* $(DATA_INSTALL_DIR)/maps
	install -m 0755 *.ttf *.ogg  $(DATA_INSTALL_DIR)/
	#cp -p hiscore.dat $(SCORE_DIR)/


# package name and what to exclude from it
PKG_NAME = $(OUT)-$(shell date -I)
EXCLUDE = --exclude CVS --exclude "*.bak"

# rename source dir to pkg-dir, package, rename back
package: veryclean
	cd ..; \
	mv $(OUT) $(PKG_NAME); \
	tar -zcvf $(PKG_NAME).tar.gz $(EXCLUDE) $(PKG_NAME); \
	mv $(PKG_NAME) $(OUT)


# remove temporary files
clean:
	$(RM) *.o *~

# remove also the built binary
veryclean: clean
	$(RM) $(OUT)

# remove also sources
megaclean: veryclean
	$(RM) *.cc *.h

uninstall:
	rm -rf $(DATA_INSTALL_DIR) $(BIN_DIR)/hhh

love:
	@echo "Sorry, I'm not equipped for that"

war:
	@echo "Make love, not war!"

.PHONY:	install package clean veryclean megaclean love war uninstall
