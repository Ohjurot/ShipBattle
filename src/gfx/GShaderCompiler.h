#pragma once
#include "pch.h"

// Produces the .cso files the game loads from shaderDir:
//   ImageQuad.slang -> ImageQuadVS.cso, ImageQuadPS.cso   (Slang -> DXIL)
//   RootSignature.cso                                     (hardcoded in GShaderCompiler.cpp,
//                                                          mirrors Bindings.slang)
// A .cso is only overwritten when its step succeeded. If the shader source is missing
// nothing is written, so a build shipping only the .cso files keeps working.
// Returns false if any step failed (details are written to the log).
bool CompileShaders(const wchar_t* shaderDir);
