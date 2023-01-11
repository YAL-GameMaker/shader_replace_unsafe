#pragma once
#include "stdafx.h"
#include "gml_internals.h"
#include "gml_d3d.h"
#include <string>
#include <vector>
#include <d3dcompiler.h>
#include "YYRunnerInterface.h"
extern YYRunnerInterface yyRunnerInterface;

template<typename T> void yySetArrayWithCountToVector(ArrayOf<T>& out_items, int& out_count, std::vector<T>& in_vec) {
	auto count = in_vec.size();
	auto items = new T[count];
	memcpy_arr(items, in_vec.data(), count);
	out_count = (int)count;
	out_items = items;
}

template<typename T> struct PtrArray {
	int count = 0;
	ArrayOf<T*> items = nullptr;
	T* get(int index) {
		if (index < 0 || index >= count) return nullptr;
		return items[index];
	}
};
template<typename T> struct PtrArrayPtr {
	PtrTo<int> pCount = nullptr;
	PtrTo<ArrayOf<T*>> pItems = nullptr;
	int size() {
		return *pCount;
	}
	T* get(int index) {
		if (index < 0 || index >= *pCount) return nullptr;
		return (*pItems)[index];
	}
	void set(int index, T* value) {
		if (index < 0 || index >= *pCount) return;
		(*pItems)[index] = value;
	}
	inline int add(T* value) {
		auto newID = *pCount;
		auto newCount = newID + 1;
		*pCount = newCount;
		auto newItems = realloc_arr(*pItems, (size_t)newCount);
		*pItems = newItems;
		newItems[newID] = value;
		return newID;
	}
};

struct YYOriginalShaders {
	PtrArray<YYShader> wrappers;
	PtrArray<YYNativeShader> native;
};
struct YYShaders {
	PtrArrayPtr<YYShader> wrappers;
	PtrArrayPtr<YYNativeShader> native;
};
extern YYOriginalShaders gmlOriginal;
extern YYShaders gmlShaders;
extern bool trouble;
#define trouble_check(_ret) if (trouble) { shader_last_error = "The extension could not initialize."; return (_ret); }

struct CustomShaderBlob {
	ID3DBlob* codeBlob = nullptr;
	ID3D11ShaderReflection* reflection = nullptr;
	void release() {
		if (codeBlob) {
			codeBlob->Release();
			codeBlob = nullptr;
		}
		if (reflection) {
			reflection->Release();
			reflection = nullptr;
		}
	}
};
struct CustomShader {
	// a pointer to this goes into wrapped->name
	std::string name = "";
	CustomShaderBlob vertex{};
	CustomShaderBlob pixel{};
	YYShader* wrapped = nullptr;
	YYNativeShader* native = nullptr;
	void release() {
		#define X(arr) if (arr) { delete[] arr; arr = nullptr; }
		if (wrapped) {
			delete wrapped;
		}
		if (native) {
			X(native->constBuffers);
			X(native->constBufVars);
			X(native->inputs);
			X(native->samplers);
			delete native;
		}
		vertex.release();
		pixel.release();
		#undef X
	}
	bool isDestroyed = false;
};

extern std::string shader_model;

extern std::vector<CustomShader*> customShaders;
inline CustomShader* getCustomShader(int id) {
	return id >= 0 && id < customShaders.size() ? customShaders[id] : nullptr;
}
inline bool setCustomShader(int id, CustomShader* sh) {
	if (id < 0) return false;
	if (customShaders.size() <= id) customShaders.resize(id + 1);
	customShaders[id] = sh;
	return true;
}

extern std::string shader_path;
extern std::string shader_last_error;