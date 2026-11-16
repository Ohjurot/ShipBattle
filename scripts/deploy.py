"""
ShipBattle deployment. Produces three artifacts in ./deploy:
  1. Source zip        (git archive of HEAD)
  2. Portable game zip (unzip and run)
  3. Windows installer (NSIS)

The game packages contain:
  - ShipBattle.exe and all DLLs of the build output
  - res/      the assets from ./app/res
  - shaders/  ONLY the compiled shader blobs (*.cso) from ./app/shaders
  - LICENSE.html

Shader sources are not shipped. The .cso blobs are written by
the game itself when it is started from a dev checkout, so they have to exist on disk
before deploying: the script fails with an error when one is missing.

The script is by default called with "Release" in the first argument

Copyright (c) 2026 Moxibyte GmbH

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
"""
import os
import mox
import sys
import glob
import platform
import argparse

# Compiled shader blobs the game loads at startup (see src/wWinMain.cpp)
REQUIRED_SHADER_BLOBS = (
    "RootSignature.cso",
    "ImageQuadVS.cso",
    "ImageQuadPS.cso",
)

ASSET_DIR = "./app/res"
SHADER_DIR = "./app/shaders"


def Fail(message):
    print(f"ERROR: {message}", file=sys.stderr)
    sys.exit(1)


def CollectGameFiles(bin_folder, exe_name):
    """Return the (source, name-in-package) pairs shared by the zip and the installer."""
    files = []

    # Executable
    exe_path = f"{bin_folder}/{exe_name}"
    if not os.path.isfile(exe_path):
        Fail(f'Executable not found: {exe_path} (run "mox build" for this configuration first)')
    files.append((exe_path, exe_name))

    # All DLLs of the build output (top level of the output folder)
    for dll in sorted(glob.glob(f"{bin_folder}/*.dll")):
        files.append((dll, os.path.basename(dll)))

    # Assets (whole tree, without dot files such as .gitattributes)
    if not os.path.isdir(ASSET_DIR):
        Fail(f"Asset folder not found: {ASSET_DIR}")
    for root, dirs, names in os.walk(ASSET_DIR):
        rel = os.path.relpath(root, ASSET_DIR).replace("\\", "/")
        dst_dir = "res" if rel == "." else f"res/{rel}"
        for name in sorted(names):
            if not name.startswith("."):
                files.append((os.path.join(root, name), f"{dst_dir}/{name}"))

    # Compiled shaders: must exist, sources are never shipped
    missing = [blob for blob in REQUIRED_SHADER_BLOBS if not os.path.isfile(f"{SHADER_DIR}/{blob}")]
    if missing:
        Fail(
            f'Compiled shader(s) missing in {SHADER_DIR}: {", ".join(missing)}\n'
            f"       They are generated when the game runs from the dev checkout. Start the game once, then deploy again."
        )
    blob_times = [os.path.getmtime(f"{SHADER_DIR}/{blob}") for blob in REQUIRED_SHADER_BLOBS]
    source_times = [os.path.getmtime(src) for src in glob.glob(f"{SHADER_DIR}/*.slang")]
    if source_times and max(source_times) > min(blob_times):
        print("WARNING: A shader source is newer than the compiled .cso files. Start the game once to recompile them.")
    for blob in REQUIRED_SHADER_BLOBS:
        files.append((f"{SHADER_DIR}/{blob}", f"shaders/{blob}"))

    # License report
    if not os.path.isfile("./LICENSE.html"):
        Fail('./LICENSE.html not found (run "mox init" first)')
    files.append(("./LICENSE.html", "LICENSE.html"))

    return files


if __name__ == '__main__':
    # Configuration for the build
    p = argparse.ArgumentParser(prog="deploy.py", allow_abbrev=False)
    p.add_argument("--conf", default="Release", help="Build configuration (default: Release)")
    p.add_argument("--arch", default=platform.machine().lower(), help="Alternative (cross compile) architecture")
    args = p.parse_args()

    # Detect version and architecture
    appName = mox.ExtractLuaDef("./mox.lua", "cmox_product_name")
    version = mox.GetAppVersion()
    arch = mox.GetPlatformInfo(args.arch)["premake_arch"]
    build_folder = f'./build/{arch}-{args.conf}'
    exeName = f"{appName}.exe"

    # Collect and validate everything BEFORE the first artifact is written
    gameFiles = CollectGameFiles(f"{build_folder}/bin", exeName)
    print(f"Packaging {len(gameFiles)} game files:")
    for src, name in gameFiles:
        print(f"  {name}")

    # == 1. Source archive ==
    srcArchive = mox.MDSrc(mox.AutomaticFilename(f"{appName}_src", version, args.conf, args.arch, "zip"))
    srcArchive.Deploy()

    # == 2. Portable game zip ==
    zipArchive = mox.MDZip(mox.AutomaticFilename(f"{appName}", version, args.conf, args.arch, "zip"))
    for src, name in gameFiles:
        zipArchive.AddFile(src, name)
    zipArchive.Deploy()

    # == 3. Windows installer (NSIS, Modern UI 2) ==
    # The first call bootstraps a portable NSIS into ./dependencies/nsis. See scripts/mox/mdnsis.py for the full API.
    installer = mox.MDNSIS(mox.AutomaticFilename(f"{appName}", version, args.conf, args.arch, "exe"), appName, version, args.arch)
    installer.SetMeta(publisher="Ludwig Fuechsl", description=f"{appName} Installer", url="https://github.com/Ohjurot/ShipBattle")
    installer.SetMainExecutable(exeName)                     # Used for the display icon and run after install
    installer.SetLicense("./LICENSE.html")
    installer.SetInstallScopes(("machine", "user"))          # The end user chooses inside the installer
    installer.SetRunAfterInstall(True)

    installer.StartComponent("Game", required=True, description=f"{appName} game files")
    for src, name in gameFiles:
        installer.AddFile(src, name)

    installer.StartComponent("Shortcuts/Start Menu", description="Start menu entries")
    installer.AddShortcut(exeName, appName, "startmenu")
    installer.AddShortcut("Uninstall.exe", f"Uninstall {appName}", "startmenu")

    installer.StartComponent("Shortcuts/Desktop", default=False, description="Desktop shortcut")
    installer.AddShortcut(exeName, appName, "desktop")
    installer.Deploy()
