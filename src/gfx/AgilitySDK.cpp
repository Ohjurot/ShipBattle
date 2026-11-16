#include "pch.h"

// DirectX 12 Agility SDK selection.
// The D3D12 loader reads these two exports from the exe and then loads D3D12Core.dll
// (and d3d12SDKLayers.dll for the debug layer) from D3D12SDKPath instead of the system copy.
// D3D12_SDK_VERSION comes from the Agility SDK's d3d12.h (see dependencies.yml), so the
// version always matches the headers the game was compiled against.
extern "C" { __declspec(dllexport) extern const UINT D3D12SDKVersion = D3D12_SDK_VERSION; }
extern "C" { __declspec(dllexport) extern const char* D3D12SDKPath = ".\\"; }
