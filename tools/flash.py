#!/usr/bin/env python3
"""Buttonless flash for esptest (Waveshare ESP32-S3-Zero, TinyUSB/OTG mode).

In OTG mode the stock USB-Serial/JTAG auto-reset is gone, so esptool cannot drop a
running firmware into the ROM bootloader on its own. The Arduino-ESP32 USB-CDC stack,
however, reboots into the bootloader when the host opens the port at 1200 baud with
DTR de-asserted. This script performs that "1200bps touch" with controlled timing,
waits for the download port to re-enumerate, then runs `pio run -e dev -t upload`.

Usage:  python tools/flash.py            (build is implicit in the pio upload)
Fallback if the touch fails (e.g. a crashed/old firmware that can't touch): enter the
ROM bootloader manually — unplug, hold BOOT, replug, release BOOT — then run this again.
"""
import sys
import time
import subprocess

try:
    import serial
    from serial.tools import list_ports
except ImportError:
    print("[flash] pyserial not found. Run: python -m pip install pyserial")
    sys.exit(2)

VID, PID = 0x303A, 0x1001
ENV = "dev"


def esp_serial_ports():
    return [p for p in list_ports.comports() if p.vid == VID and p.pid == PID and p.device]


def touch_1200(port):
    print(f"[flash] 1200bps touch on {port} -> reboot into bootloader")
    try:
        s = serial.Serial()
        s.port = port
        s.baudrate = 1200
        s.dtr = False
        s.rts = False
        s.open()
        time.sleep(0.25)
        s.close()
    except Exception as exc:  # port may vanish mid-touch; that is expected
        print(f"[flash] touch note: {exc}")


def main():
    ports_before = esp_serial_ports()
    if not ports_before:
        print("[flash] No 303A:1001 device present. Plug in the board.")
        return 2
    if len(ports_before) > 1:
        names = ", ".join(p.device for p in ports_before)
        print("[flash] Multiple 303A:1001 devices found (%s)." % names)
        print("[flash] Refusing an ambiguous target: the 1200-touch and the hwgrep upload could")
        print("[flash] hit the wrong board. Disconnect the others (or flash with an explicit")
        print("[flash] upload_port) and retry.")
        return 2

    app_port = ports_before[0].device
    touch_1200(app_port)

    print("[flash] waiting for the ROM download port to re-enumerate...")
    deadline = time.time() + 15
    dl_port = None
    while time.time() < deadline:
        time.sleep(0.5)
        now = esp_serial_ports()
        if not now:
            continue
        # After the touch the device re-enumerates; take whatever 303A:1001 serial port is present.
        dl_port = now[0].device
        # Prefer a port that differs from the one we touched (fresh enumeration).
        fresh = [p.device for p in now if p.device != app_port]
        if fresh:
            dl_port = fresh[0]
            break
    if dl_port is None:
        print("[flash] download port did not appear; the firmware may not support the touch.")
        print("[flash] Enter the bootloader manually: unplug, hold BOOT, replug, release BOOT, re-run.")
        return 1

    print(f"[flash] uploading via {dl_port}")
    time.sleep(0.6)  # let Windows settle the new port
    rc = subprocess.call(["pio", "run", "-e", ENV, "-t", "upload"], shell=True)
    if rc != 0:
        print("[flash] upload failed. If it says 'no serial data', enter the bootloader manually and re-run.")
    return rc


if __name__ == "__main__":
    sys.exit(main())
