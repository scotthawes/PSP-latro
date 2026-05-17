BUILD_PRX=0
DEBUG=1

.DEFAULT_GOAL := all

PSPSDK=$(shell psp-config --pspsdk-path)
PSPDIR=$(shell psp-config --psp-prefix)

TARGET = pspalatro
OBJS = src/main.o

CFLAGS = -Wall -Wno-unused-label -G0 -Ofast
ifeq ($(DEBUG),1)
CFLAGS += -DDEBUG
endif
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

LIBDIR =
LIBS = -lpspgum -lpspgu -lpspdmac -lpsppower -lpspaudiolib -lpspaudio -lvorbisenc -lvorbisfile -lvorbis -logg -lzip -lz -lbz2 -llzma -lmbedcrypto
LDFLAGS =

EXTRA_TARGETS = EBOOT.PBP
PSP_EBOOT_TITLE = PSPalatro
PSP_EBOOT_ICON = media/third-party/game_icon-0.png
PSP_EBOOT_PIC1 = media/third-party/image-1.png

# Unity-build dependency: main.c includes many src/*.c and src/*.h files.
# Rebuild main.o when any included source/header changes.
src/main.o: $(wildcard src/*.c src/*.h)

include $(PSPSDK)/lib/build.mak

# Keep build/ launch artifacts in sync with the freshly built EBOOT.
all: sync_build_artifacts

sync_build_artifacts: EBOOT.PBP
	@mkdir -p build
	@mkdir -p build/assets
	@cp EBOOT.PBP build/EBOOT.PBP
	@rm -rf build/assets
	@cp -r assets build/assets
	@cp assets/out/assets/editions.png build/ 2>/dev/null || true
	@mkdir -p $(HOME)/.config/ppsspp/PSP/GAME/PSPALATRO
	@cp EBOOT.PBP $(HOME)/.config/ppsspp/PSP/GAME/PSPALATRO/EBOOT.PBP 2>/dev/null || true
	@rm -rf $(HOME)/.config/ppsspp/PSP/GAME/PSPALATRO/assets 2>/dev/null || true
	@cp -r assets $(HOME)/.config/ppsspp/PSP/GAME/PSPALATRO/assets 2>/dev/null || true
	@cp settings.ini $(HOME)/.config/ppsspp/PSP/GAME/PSPALATRO/settings.ini 2>/dev/null || true

	@echo "Synced build and PPSSPP runtime artifacts."

# Deploy assets to build directory for PSP runtime
install: EBOOT.PBP
	@mkdir -p build/deploy/PSP/GAME/PSPALATRO
	@cp EBOOT.PBP build/deploy/PSP/GAME/PSPALATRO/
	@cp -r assets build/deploy/PSP/GAME/PSPALATRO/ 2>/dev/null || true
	@cp settings.ini build/deploy/PSP/GAME/PSPALATRO/ 2>/dev/null || true
	@echo "Deploy complete to build/deploy/PSP/GAME/PSPALATRO/"

.PHONY: install

