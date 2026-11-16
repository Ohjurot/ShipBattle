#include "pch.h"
#include "GShaderCompiler.h"

#include <slang.h>
#include <slang-com-ptr.h>

#include <filesystem>
#include <fstream>
#include <string>
#include <vector>
#include <cfloat>

namespace {
	// Slang module (file name without extension) and the entry points compiled from it
	const char* const c_slangModule = "ImageQuad";
	const wchar_t* const c_slangModuleFile = L"ImageQuad.slang";
	const char* const c_slangProfile = "sm_6_0";

	struct EntryPointDesc {
		const char* entry;
		const wchar_t* output;
	};
	const EntryPointDesc c_entryPoints[] = {
		{ "vsMain", L"ImageQuadVS.cso" },
		{ "psMain", L"ImageQuadPS.cso" },
	};
	const size_t c_entryPointCount = _countof(c_entryPoints);

	const wchar_t* const c_rootSignatureOutput = L"RootSignature.cso";

	void LogBlob(slang::IBlob* blob) {
		if (blob && blob->getBufferSize()) {
			GetLogger().log(std::string_view((const char*)blob->getBufferPointer(), blob->getBufferSize()));
		}
	}

	bool WriteFile(const std::filesystem::path& path, const void* data, size_t size) {
		std::ofstream file(path, std::ios::binary | std::ios::trunc);
		file.write((const char*)data, size);
		if (!file) {
			GetLogger().log("Failed to write shader binary: " + path.string());
			return false;
		}
		return true;
	}

	// The ONE root signature shared by all pipelines.
	// It is the C++ side of the binding contract declared in app/shaders/Bindings.slang
	// and must be kept in sync with that file by hand:
	//   Root parameter 0: descriptor table, SRV t0..t10 (space0)  -> textures[11]
	//   Static sampler:   s0 (space0), anisotropic / wrap         -> texSampler
	bool WriteRootSignature(const std::filesystem::path& outputPath) {
		D3D12_DESCRIPTOR_RANGE1 textureRange = {};
		textureRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
		textureRange.NumDescriptors = 11;
		textureRange.BaseShaderRegister = 0;
		textureRange.RegisterSpace = 0;
		textureRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

		D3D12_ROOT_PARAMETER1 rootParameters[1] = {};
		rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		rootParameters[0].DescriptorTable.NumDescriptorRanges = 1;
		rootParameters[0].DescriptorTable.pDescriptorRanges = &textureRange;
		rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

		D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
		staticSamplers[0].Filter = D3D12_FILTER_ANISOTROPIC;
		staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		staticSamplers[0].MaxAnisotropy = 16;
		staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
		staticSamplers[0].BorderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE;
		staticSamplers[0].MinLOD = 0.0f;
		staticSamplers[0].MaxLOD = FLT_MAX;
		staticSamplers[0].ShaderRegister = 0;
		staticSamplers[0].RegisterSpace = 0;
		staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

		D3D12_VERSIONED_ROOT_SIGNATURE_DESC desc = {};
		desc.Version = D3D_ROOT_SIGNATURE_VERSION_1_1;
		desc.Desc_1_1.NumParameters = _countof(rootParameters);
		desc.Desc_1_1.pParameters = rootParameters;
		desc.Desc_1_1.NumStaticSamplers = _countof(staticSamplers);
		desc.Desc_1_1.pStaticSamplers = staticSamplers;
		desc.Desc_1_1.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

		ScopedComPointer<ID3DBlob> blob;
		ScopedComPointer<ID3DBlob> errors;
		HRESULT hr = D3D12SerializeVersionedRootSignature(&desc, blob.to(), errors.to());
		if (FAILED(hr)) {
			GetLogger().log("D3D12SerializeVersionedRootSignature(...) failed!").log(hr);
			if (errors) {
				GetLogger().log(std::string_view((const char*)errors->GetBufferPointer(), errors->GetBufferSize()));
			}
			return false;
		}

		return WriteFile(outputPath, blob->GetBufferPointer(), blob->GetBufferSize());
	}

	bool CompileSlangShaders(const std::filesystem::path& dir) {
		Slang::ComPtr<slang::IGlobalSession> globalSession;
		if (SLANG_FAILED(slang::createGlobalSession(globalSession.writeRef()))) {
			GetLogger().log("slang::createGlobalSession(...) failed!");
			return false;
		}

		// Session targeting DXIL
		slang::TargetDesc targetDesc = {};
		targetDesc.format = SLANG_DXIL;
		targetDesc.profile = globalSession->findProfile(c_slangProfile);

		const std::string searchPath = dir.string();
		const char* searchPaths[] = { searchPath.c_str() };

		slang::SessionDesc sessionDesc = {};
		sessionDesc.targets = &targetDesc;
		sessionDesc.targetCount = 1;
		sessionDesc.searchPaths = searchPaths;
		sessionDesc.searchPathCount = 1;

		Slang::ComPtr<slang::ISession> session;
		if (SLANG_FAILED(globalSession->createSession(sessionDesc, session.writeRef()))) {
			GetLogger().log("Slang: createSession(...) failed!");
			return false;
		}

		// Load module
		Slang::ComPtr<slang::IBlob> diagnostics;
		slang::IModule* module = session->loadModule(c_slangModule, diagnostics.writeRef());
		LogBlob(diagnostics);
		if (!module) {
			GetLogger().log("Slang: failed to load shader module!");
			return false;
		}

		// Compile entry points
		bool ok = true;
		for (const EntryPointDesc& desc : c_entryPoints) {
			Slang::ComPtr<slang::IEntryPoint> entryPoint;
			if (SLANG_FAILED(module->findEntryPointByName(desc.entry, entryPoint.writeRef())) || !entryPoint) {
				GetLogger().log(std::string("Slang: entry point not found: ") + desc.entry);
				ok = false;
				continue;
			}

			slang::IComponentType* components[] = { module, entryPoint };
			Slang::ComPtr<slang::IComponentType> program;
			Slang::ComPtr<slang::IComponentType> linkedProgram;
			Slang::ComPtr<slang::IBlob> code;

			SlangResult result = session->createCompositeComponentType(components, 2, program.writeRef(), diagnostics.writeRef());
			LogBlob(diagnostics);
			if (SLANG_SUCCEEDED(result)) {
				result = program->link(linkedProgram.writeRef(), diagnostics.writeRef());
				LogBlob(diagnostics);
			}
			if (SLANG_SUCCEEDED(result)) {
				result = linkedProgram->getEntryPointCode(0, 0, code.writeRef(), diagnostics.writeRef());
				LogBlob(diagnostics);
			}
			if (SLANG_FAILED(result) || !code) {
				GetLogger().log(std::string("Slang: failed to compile entry point: ") + desc.entry);
				ok = false;
				continue;
			}

			ok &= WriteFile(dir / desc.output, code->getBufferPointer(), code->getBufferSize());
		}

		return ok;
	}
}

bool CompileShaders(const wchar_t* shaderDir) {
	const std::filesystem::path dir(shaderDir);
	std::error_code ec;

	// No sources (e.g. a build shipping only the .cso files): nothing to do
	if (!std::filesystem::exists(dir / c_slangModuleFile, ec)) {
		return true;
	}

	bool ok = CompileSlangShaders(dir);
	ok &= WriteRootSignature(dir / c_rootSignatureOutput);
	return ok;
}
