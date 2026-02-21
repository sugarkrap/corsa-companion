"""
PlatformIO Extra Script: DFU Packager
Converts firmware hex to Nordic DFU package for Bluetooth OTA

Usage: Automatically runs after build in bbcmicrobit-ota environment
Requirements: pip install nrfutil
"""

import os
import subprocess
from pathlib import Path

Import("env")

# DFU configuration
DFU_CONFIG = {
    "hw_version": 52,          # Hardware version (nRF52 = 52, nRF51 = 51)
    "sd_req": [0x00],          # SoftDevice requirements (0x00 = no SD required)
    "app_version": 1,          # Application version
    "bootloader_version": 1,   # Bootloader version
}

def get_firmware_path():
    """Get the path to the built firmware hex file"""
    firmware_dir = env.subst("$BUILD_DIR")
    firmware_name = env.subst("${PROGNAME}.hex")
    return Path(firmware_dir) / firmware_name

def create_dfu_package(source, target, env):
    """
    Post-build action: Convert hex to DFU package
    """
    firmware_hex = get_firmware_path()
    
    if not firmware_hex.exists():
        print(f"Warning: Firmware not found at {firmware_hex}")
        return
    
    # Output DFU package path
    dfu_package = firmware_hex.with_suffix(".zip")
    
    # Build nrfutil command
    cmd = [
        "nrfutil",
        "pkg", "generate",
        "--hw-version", str(DFU_CONFIG["hw_version"]),
        "--sd-req", ",".join(f"0x{req:02X}" for req in DFU_CONFIG["sd_req"]),
        "--application", str(firmware_hex),
        "--application-version", str(DFU_CONFIG["app_version"]),
        str(dfu_package)
    ]
    
    print(f"Creating DFU package: {dfu_package}")
    print(f"Command: {' '.join(cmd)}")
    
    try:
        result = subprocess.run(
            cmd,
            capture_output=True,
            text=True,
            check=True
        )
        print(f"✅ DFU package created: {dfu_package}")
        print(f"   Size: {dfu_package.stat().st_size} bytes")
        
        # Store path for upload command
        env.Replace(DFU_PACKAGE_PATH=str(dfu_package))
        
    except subprocess.CalledProcessError as e:
        print(f"❌ Failed to create DFU package")
        print(f"   stdout: {e.stdout}")
        print(f"   stderr: {e.stderr}")
        print(f"\nMake sure nrfutil is installed: pip install nrfutil")
    except FileNotFoundError:
        print(f"❌ nrfutil not found. Install it with: pip install nrfutil")

# Register post-build action
env.AddPostAction("$BUILD_DIR/${PROGNAME}.hex", create_dfu_package)

print("DFU Packager script loaded. Will create .zip package after build.")
