# Pre-build: put the bundled Arduino-ESP32 FS library's source on the include path.
# The bundled USB library compiles its mass-storage sources (USBMSCFS.cpp), which include
# "FS.h", but PlatformIO's LDF does not honor the USB library's depends=FS. Resolving the
# framework package directory here keeps this portable across machines and core versions.
import os

Import("env")  # noqa: F821  (injected by PlatformIO/SCons)

framework_dir = env.PioPlatform().get_package_dir("framework-arduinoespressif32")
fs_src = os.path.join(framework_dir, "libraries", "FS", "src")
if os.path.isdir(fs_src):
    env.Append(CPPPATH=[fs_src])
else:
    print("add_fs_include.py: FS source not found at %s" % fs_src)
