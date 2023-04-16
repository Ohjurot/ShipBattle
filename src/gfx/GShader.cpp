#include "pch.h"
#include "GShader.h"

GShader::GShader(LPCWSTR path) {
	// Open file
	HANDLE hFile = CreateFile(path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE) {
		GetLogger().log("Failed to open shader file!");
		return;
	}

	// Get size and data
	m_size = GetFileSize(hFile, NULL);
	if (m_size == 0) {
		GetLogger().log("Shader file is empty!");
	}
	m_memory = malloc(m_size);
	if (!ReadFile(hFile, m_memory, m_size, NULL, NULL)) {
		GetLogger().log("Failed to read shader!");
	}
}

GShader::~GShader() {
	free(m_memory);
}

void* GShader::ptr() {
	return m_memory;
}

UINT64 GShader::size() {
	return m_size;
}
