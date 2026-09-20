# Battery Run — original Nintendo Game Boy homebrew
# Requires GBDK-2020 and GNU make.

GBDK_HOME ?= /opt/gbdk
LCC = $(GBDK_HOME)/bin/lcc
BUILD = build
ROM = $(BUILD)/battery_run.gb
SRC = src/main.c

all: $(ROM)

$(BUILD):
	mkdir -p $(BUILD)

$(ROM): $(SRC) | $(BUILD)
	$(LCC) -msm83:gb -Wl-yt0x01 -Wl-yo4 -o $@ $<

clean:
	rm -rf $(BUILD)

.PHONY: all clean
