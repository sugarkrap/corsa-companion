#!/usr/bin/env python3
"""
Bluetooth OTA Upload Helper for micro:bit
Scans for DFU devices and uploads firmware package

Usage:
    python tools/ble_ota_upload.py <firmware.zip>
    
Or in platformio.ini:
    upload_command = python tools/ble_ota_upload.py $SOURCE
"""

import sys
import subprocess
import argparse
from pathlib import Path

# DFU Target service name/UUID
DFU_TARGET_NAME = "DfuTarg"
DFU_SERVICE_UUID = "FE59"

def find_dfu_devices():
    """
    Scan for Bluetooth DFU devices
    Returns list of (name, address) tuples
    """
    print("🔍 Scanning for DFU devices...")
    
    try:
        # Try using nrfutil device discovery
        result = subprocess.run(
            ["nrfutil", "device", "list"],
            capture_output=True,
            text=True,
            timeout=10
        )
        
        devices = []
        for line in result.stdout.splitlines():
            if DFU_TARGET_NAME in line or "DFU" in line.upper():
                # Parse device info (format varies)
                devices.append(line.strip())
        
        return devices
        
    except subprocess.TimeoutExpired:
        print("⚠️  Device scan timed out")
        return []
    except FileNotFoundError:
        print("❌ nrfutil not found. Install: pip install nrfutil")
        return []

def upload_via_ble(firmware_path: Path, device_address: str = None):
    """
    Upload firmware package over BLE using nrfutil
    """
    if not firmware_path.exists():
        print(f"❌ Firmware not found: {firmware_path}")
        return False
    
    print(f"📦 Firmware: {firmware_path}")
    print(f"📡 Uploading via Bluetooth...")
    
    # Build nrfutil dfu command
    cmd = [
        "nrfutil", "dfu", "ble",
        "-pkg", str(firmware_path),
    ]
    
    if device_address:
        cmd.extend(["-a", device_address])
    else:
        # Use device name
        cmd.extend(["-n", DFU_TARGET_NAME])
    
    print(f"   Command: {' '.join(cmd)}")
    
    try:
        result = subprocess.run(cmd, check=True)
        print("✅ Upload complete! Device will reboot...")
        return True
        
    except subprocess.CalledProcessError as e:
        print(f"❌ Upload failed: {e}")
        return False

def main():
    parser = argparse.ArgumentParser(
        description="Upload firmware to micro:bit via Bluetooth OTA"
    )
    parser.add_argument(
        "firmware",
        type=Path,
        help="DFU package (.zip) or hex file"
    )
    parser.add_argument(
        "-a", "--address",
        help="BLE device address (auto-detect if not specified)"
    )
    parser.add_argument(
        "-s", "--scan",
        action="store_true",
        help="Only scan for devices, don't upload"
    )
    
    args = parser.parse_args()
    
    # Scan for devices first
    devices = find_dfu_devices()
    
    if args.scan:
        if devices:
            print(f"\n📱 Found {len(devices)} DFU device(s):")
            for dev in devices:
                print(f"   • {dev}")
        else:
            print("\n⚠️  No DFU devices found")
            print("   Make sure micro:bit is in DFU mode (hold buttons while powering on)")
        return
    
    # Convert hex to zip if needed
    firmware = args.firmware
    if firmware.suffix == ".hex":
        zip_path = firmware.with_suffix(".zip")
        if not zip_path.exists():
            print(f"❌ DFU package not found: {zip_path}")
            print(f"   Build with: pio run -e bbcmicrobit-ota")
            return 1
        firmware = zip_path
    
    # Upload
    success = upload_via_ble(firmware, args.address)
    return 0 if success else 1

if __name__ == "__main__":
    sys.exit(main())
