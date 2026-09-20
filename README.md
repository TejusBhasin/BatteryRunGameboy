# Battery Run — OG Game Boy Homebrew

**Retro console:** Nintendo Game Boy (DMG / original Game Boy)

Battery Run is a small maze-action homebrew game made specifically for the original Game Boy. The player collects six battery cells, avoids security drones, and reaches the exit before being caught.

## Controls

- **D-pad:** Move one tile at a time
- **A:** Activate a one-hit shield
- **B:** Reset the run
- **START:** Start/restart from title and result screens

## Goal

Collect all 6 battery cells, then reach the exit in the upper-right corner. Drones move through the maze and chase the player. A shield absorbs one collision.

## Original Game Boy compatibility

The game targets the original DMG Game Boy with GBDK-2020. It uses the DMG's 160×144 display, 8×8 tile graphics, hardware sprites, joypad input, and four-shade DMG palettes. It does not require Game Boy Color features.

The source contains all gameplay code and graphics needed to build the ROM. There are no copyrighted third-party game assets or external art dependencies.

## Build

Install GBDK-2020, then set `GBDK_HOME` to its directory:

```sh
export GBDK_HOME=/path/to/gbdk
make
```

The ROM is written to:

```text
build/battery_run.gb
```

It can be tested in an original Game Boy emulator and, with suitable flash hardware, on original Game Boy hardware.

## GitHub Actions

The included workflow downloads GBDK-2020 and builds the `.gb` ROM on every push or manual workflow run. The resulting ROM is uploaded as a workflow artifact.

## Project files

- `src/main.c` — game, map, graphics, input, enemies, collision, and screens
- `Makefile` — local build
- `.github/workflows/build.yml` — automated ROM build
- `LICENSE` — MIT license
