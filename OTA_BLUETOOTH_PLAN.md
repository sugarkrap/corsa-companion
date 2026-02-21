# Bluetooth OTA (DFU) Plan for micro:bit

## Overview
micro:bit uses Nordic nRF51/52 chips which support **DFU (Device Firmware Update)** over Bluetooth Low Energy.

## Architecture

```
┌─────────────┐      BLE       ┌─────────────┐
│  Web/Mobile │  ═══════════►  │   micro:bit │
│    App      │   DFU Service  │  (nRF51/52) │
└─────────────┘                └─────────────┘
                                    │
                                    ▼
                              ┌─────────────┐
                              │ DFU Bootloader│
                              │  (receives   │
                              │   firmware)  │
                              └─────────────┘
```

## Components Needed

### 1. DFU Bootloader
- Use Nordic's **Open Bootloader** or **Secure Bootloader**
- Pre-flashed on micro:bit or needs custom flashing
- Supports receiving firmware over BLE

### 2. Firmware Packaging
- Build firmware with PlatformIO
- Package as `.zip` with `.bin` and `.dat` (init packet)
- Use `nrfutil` tool from Nordic

### 3. Web/Mobile Interface
- **Web Bluetooth API** (Chrome/Edge) - no app install needed!
- Or Nordic's nRF Connect mobile app
- Upload interface for `.zip` file

## Implementation Steps

1. **Setup DFU Bootloader** on micro:bit
2. **Build firmware** with PlatformIO (generates `.hex`)
3. **Convert to DFU package** using `nrfutil pkg generate`
4. **Create web interface** using Web Bluetooth to send DFU
5. **Upload firmware** over BLE

## Key Tools
- `nrfutil` - Nordic's CLI tool for DFU packaging
- Web Bluetooth API - for browser-based uploads
- Nordic DFU Service UUID: `0xFE59`

## Frontend Idea
A sleek web interface where you:
1. Pair with micro:bit via Web Bluetooth
2. Select firmware file
3. See progress bar during upload
4. Device auto-reboots into new firmware

Want me to build this?