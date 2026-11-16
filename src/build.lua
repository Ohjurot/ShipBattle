-- Main executable
mox_project("ShipBattle")
mox_cpp()
mox_windowed()
uuid("8526cca7-6caa-405e-9340-90234e5ffb77")

-- Links to system lib
links {
    "dxgi",
    "ole32",
    "d3d12",
    "dxguid",
    "windowscodecs",
}
