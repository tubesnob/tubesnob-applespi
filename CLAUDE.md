# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

This is the AppleSPI project, providing SPI-based networking capabilities for Apple IIGS computers and modern development platforms. The project implements a modular TCP/IP stack with hardware abstraction for the W5500 Ethernet chip.

## Build Commands

### Building for macOS/Unix (default)
```bash
# Build all libraries and tests
make

# Clean all build artifacts  
make clean

# Debug build environment settings
make dumpenv
```

### Building for Apple IIGS
```bash
# Build for Apple IIGS using ORCA/M toolchain
make DOIIX=true

# Clean Apple IIGS build artifacts
make clean DOIIX=true
```

### Running Tests
```bash
# After building, run individual test suites:
./tslib_test/bin/tslib_test_main      # Core library tests
./tsspilib_test/bin/tsspilib_test     # SPI library tests  
./tssocketlib_test/bin/tssocketlib_test_main  # Socket library tests
```

## Architecture Overview

### Library Stack (bottom to top)
1. **tslib** - Core utilities (memory, strings, lists, logging, CRC16, INI parsing)
2. **tsspilib** - SPI abstraction layer with pluggable drivers
   - `tsspilib_driver_ftdi` - FTDI USB-to-SPI for development
   - `tsspilib_driver_a2gpio` - Apple IIGS GPIO SPI driver
3. **tssocketlib** - Socket abstraction using W5500 Ethernet chip
4. **tsiplib** - Lightweight TCP/IP stack (replacing lwIP integration)

### Applications
- **spidl** - File download client over TCP/IP
- **spiws** - HTTP web server (currently disabled in build)
- **spidlhost** - C# server for file transfers (in spidlhost/)

### Platform Support
- Dual-platform codebase controlled by preprocessor defines:
  - `__MACOS__` - macOS/Unix builds using clang
  - `__APPLE2GS__` - Apple IIGS builds using iix toolchain
- Platform selection controlled by `DOIIX` make variable

### Current Development Status
- Branch: `lwip-integration` - Migrating from lwIP to custom TCP/IP stack
- Active refactoring: lwIP files removed, new tsiplib implementation added

## Key Development Patterns

### Module Structure
Each library follows consistent patterns:
- `src/` - Source files and Makefile
- `include/` - Public headers
- `bin/` - Build output (libraries and executables)

### Error Handling
- Libraries use `tslog` for logging
- Functions typically return status codes or NULL on error
- Resource cleanup follows init/shutdown pattern

### Testing Approach
- Simple executable tests without framework
- Tests initialize libraries, perform operations, and clean up
- Use logging to report results

## Configuration Files

### Network Configuration (spidl.cfg)
```ini
[source]
ip=10.0.0.98
mask=255.255.255.0  
gw=10.0.0.1
mac=70:60:50:40:30:20
port=3456

[dest]
ip=10.0.0.94
port=8888
```

### Build System
- Makefiles use recursive make for each component
- `Makefile.env.include` - Compiler and toolchain configuration
- Each component has independent Makefile following same patterns