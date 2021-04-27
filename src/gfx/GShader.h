#pragma once
#include "pch.h"

class GShader {
	public:
		GShader(LPCWSTR path);
		~GShader();

		void* ptr();
		UINT64 size();

	private:
		void* m_memory;
		UINT64 m_size;
};