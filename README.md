# basstap

██████╗  █████╗ ███████╗███████╗████████╗ █████╗  ██████╗  
██╔══██╗██╔══██╗██╔════╝██╔════╝╚  ██╔══╝██╔══██╗ ██╔══██╗ 
██████╔╝███████║███████╗███████╗   ██║   ███████║ ██████╔╝ 
██╔══██╗██╔══██║╚════██║╚════██║   ██║   ██╔══██║ ██╔═══╝ 
██████╔╝██║  ██║███████║███████║   ██║   ██║  ██║ ██║ 
╚═════╝ ╚═╝  ╚═╝╚══════╝╚══════╝   ╚═╝   ╚═╝  ╚═╝ ╚═╝    
A terminal-based real-time audio spectrum visualizer written in C for PulseAudio.

## Features

- Captures live system audio output using PulseAudio.
- Calculates real-time FFT frequency spectrum analysis.
- Displays audio visualization directly inside the terminal interface.

## Prerequisites

Ensure you have the following dependencies installed on your Linux distribution:

- GCC compiler
- Make
- PulseAudio development libraries (`libpulse-dev` on Debian/Ubuntu or `pulseaudio-libs-devel` on Fedora/RHEL)
- `playerctl` (for fetching media and track metadata)

### Installing Prerequisites

Debian / Ubuntu:

```bash
sudo apt install build-essential libpulse-dev playerctl
```

## Building

Compile the project by running:

```bash
make
```

This will produce the executable `basstap`.

To remove built binaries:

```bash
make clean
```

## Usage

Run the compiled executable:

```bash
./basstap
```

Press `Ctrl+C` to quit the visualizer and restore terminal output parameters.