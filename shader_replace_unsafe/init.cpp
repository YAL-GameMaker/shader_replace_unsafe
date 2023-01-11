#include "stdafx.h"
#include "shared.h"
#include "gml_internals.h"
#include "gml_d3d.h"
#include "instr_tools.h"
#include "auto_cast.h"
#include <DbgEng.h>

///
enum class shader_replace_unsafe_init_error {
	success = 1,
	no_dll = -1,
	wrong_func_offset = -2,
	no_shader_array = -3,
	no_native_shader_array = -4,
	instr_init_failed = -5,
};

static bool canAccessMemory(const void* base, size_t size) {
	const auto pmask = PAGE_READONLY | PAGE_READWRITE | PAGE_WRITECOPY
		| PAGE_EXECUTE | PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY;
	::MEMORY_BASIC_INFORMATION mbi{};
	size_t steps = size > 0 ? 2 : 1;
	for (auto step = 0u; step < steps; step++) {
		const void* addr = ((cmbyte*)base) + step * (size - 1);
		if (!VirtualQuery(addr, &mbi, sizeof mbi)) return false;
		if (mbi.State != MEM_COMMIT) return false;
		if ((mbi.Protect & PAGE_GUARD) != 0) return false;
		if ((mbi.Protect & pmask) == 0) return false;
	}
	return true;
}
dllx double shader_replace_unsafe_init_offset(cmbyte* _f1, cmbyte* _f2) {
	ArrayOf<void*> f1 = auto_cast(_f1), f2 = auto_cast(_f2);
	ArrayOf<void*> fx[] = {f1, f2};
	::MEMORY_BASIC_INFORMATION mbi{};
	const auto pExec = PAGE_EXECUTE | PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY;
	for (auto i = 10u; i < 24; i++) {
		auto step = 0u;
		for (; step < 2; step++) {
			auto fi = fx[step];
			if (!canAccessMemory(fi + i - 1, sizeof(void*) * 3)) return -1;

			// should be NULL, <addr>, NULL
			if (fi[i - 1] != nullptr) break;
			if (fi[i] == nullptr) break;
			if (fi[i + 1] != nullptr) break;
			//trace("ind %d step %u: %p %p %p", i, step, *prev, *curr, *next);
		}
		if (step < 2u) continue;

		// destination address must match:
		auto dest = f1[i];
		if (dest != f2[i]) continue;

		// destination address must be executable:
		if (!VirtualQuery(dest, &mbi, sizeof mbi)) continue;
		if ((mbi.Protect & pExec) == 0) continue;
		
		return sizeof(void*) * i;
	}
	return -1;
}

cmbyte* csrGetCppAddr(cmbyte* _funcRef, int funcOffset) {
	auto funcRef = (CScriptRef*)_funcRef;
	bool suspiciousNeighbours;
	cmbyte* functionAddr;

	if (funcOffset > 0) {
		auto cppAt = (uint8_t*)funcRef + funcOffset;
		functionAddr = *(uint8_t**)cppAt;
		suspiciousNeighbours = *(void**)(cppAt - sizeof(void*)) != nullptr
			|| *(void**)(cppAt + sizeof(void*)) != nullptr;
	} else {
		functionAddr = (uint8_t*)funcRef->callCpp;
		suspiciousNeighbours = funcRef->callVM != nullptr || funcRef->callYYC != nullptr;
	}
	if (suspiciousNeighbours || functionAddr == nullptr) {
		trace("Offset is likely incorrect,"
			" target pointer (C++ function address) should be surrounded by two nullptrs (VM/YYC function addresses)."
			" Try adjusting it in 4/8 increments to see what works."
			" This function can be called again and returns whether successful.");
		return nullptr;
	}
	if (functionAddr[0] == 0xE9) {
		functionAddr = InstrTools::readRelCallTarget(functionAddr);
	}
	return functionAddr;
}

static int funcOffset;
dllx double shader_replace_unsafe_init_1(ID3D11Device* window_device, ID3D11DeviceContext* context, double _func_offset) {
	funcOffset = (int)_func_offset;
	d3dDevice = window_device;
	d3dContext = context;
	return true;
}

static cmbyte* YYGetInt32 = nullptr;
static cmbyte* YYGetString = nullptr;
int (*shader_get_uniform)(YYShader* shader, const char* name) = nullptr;
GMLFunc* f_shader_get_sampler_index = nullptr;

dllx double shader_replace_unsafe_init_2(cmbyte* q_event_perform, cmbyte* q_string_replace) {
	if (!InstrTools::init()) return (int)shader_replace_unsafe_init_error::instr_init_failed;

	auto f_event_perform = csrGetCppAddr(q_event_perform, funcOffset);
	if (f_event_perform == nullptr) return (int)shader_replace_unsafe_init_error::wrong_func_offset;
	YYGetInt32 = InstrTools::findFuncWithCallCount(f_event_perform, 256, 2);

	auto f_string_replace = csrGetCppAddr(q_string_replace, funcOffset);
	YYGetString = InstrTools::findFuncWithCallCount(f_string_replace, 256, 3);
	return true;
}

// Resolves shader array
dllx double shader_replace_unsafe_init_3(cmbyte* q_shader_get_name) {
	auto f_shader_get_name = csrGetCppAddr(q_shader_get_name, funcOffset);
	if (f_shader_get_name == nullptr) return (int)shader_replace_unsafe_init_error::wrong_func_offset;

	auto next = f_shader_get_name;
	auto till = next + 256;
	while (next < till) {
		auto p = next;
		if (InstrTools::stepOverCheck(&next)) break;
		if (InstrTools::isEnd(p)) break;

		if (!InstrTools::isRelCallOrJump(p)) continue;
		if (InstrTools::readRelCallTarget(p) != YYGetInt32) continue;

		// there should be a `CMP ?, dword ptr [ShaderCount]` shortly after YYGetInt32...
		cmbyte* pCount = nullptr;
		while (next < till) {
			p = next;
			if (InstrTools::stepOverCheck(&next)) break;
			if (InstrTools::isEnd(p)) break;
			if (p[0] != 0x3B/* CMP */) continue;
			pCount = InstrTools::readRelOffset(p + 2, p + 6);
			break;
		}
		if (pCount == nullptr) break;

		// and then there should be a `MOV ?, qword ptr [ShaderArray]`...
		cmbyte* pItems = nullptr;
		while (next < till) {
			p = next;
			if (InstrTools::stepOverCheck(&next)) break;
			if (InstrTools::isEnd(p)) break;
			if (p[0] != 0x48/* MOV */) continue;
			if ((next - p) != 7) continue; // there are a couple smaller MOVs there that we don't need
			pItems = InstrTools::readRelOffset(p + 3, p + 7);
			break;
		}
		if (pItems == nullptr) break;

		gmlShaders.wrappers.pCount = (PtrTo<int>)pCount;
		gmlShaders.wrappers.pItems = (PtrTo<ArrayOf<PtrTo<YYShader>>>)pItems;

		auto shaderCount = *pCount;
		gmlOriginal.wrappers.count = shaderCount;
		gmlOriginal.wrappers.items = *gmlShaders.wrappers.pItems;

		// replace shader array with our own so that we don't crash on a realloc:
		auto newArr = malloc_arr<YYShader*>((size_t)shaderCount);
		memcpy_arr(newArr, *gmlShaders.wrappers.pItems, (size_t)shaderCount);
		*gmlShaders.wrappers.pItems = newArr;

		trace("sname: %s", gmlShaders.wrappers.get(0)->name);

		if (customShaders.size() < shaderCount) {
			customShaders.resize(shaderCount);
		}

		return true;
	}
	return (int)shader_replace_unsafe_init_error::no_shader_array;
}

/*
	Resolves the native shader array.
	shader->shaderHandle points to index of these
*/
dllx double shader_replace_unsafe_init_4(cmbyte* q_shader_get_uniform) {
	auto f_shader_get_uniform = csrGetCppAddr(q_shader_get_uniform, funcOffset);
	if (f_shader_get_uniform == nullptr) return (int)shader_replace_unsafe_init_error::wrong_func_offset;

	// shader_get_uniform calls internal function with (pShader, YYGetString(uniform_name)),
	// so we can rely on the internal function being the next after YYGetString.
	auto impl = InstrTools::findFuncCallAfter(f_shader_get_uniform, 256, YYGetString);
	if (impl == nullptr) return (int)shader_replace_unsafe_init_error::no_native_shader_array;
	if (impl[0] == 0xE9) impl = InstrTools::readRelCallTarget(impl);
	shader_get_uniform = auto_cast((void*)impl);

	auto next = impl;
	auto till = next + 256;
	while (next < till) {
		auto p = next;
		if (InstrTools::stepOverCheck(&next)) break;
		if (InstrTools::isEnd(p)) break;

		if (p[0] != 0x3B/* CMP */) continue;
		if (next - p != 6) continue;

		auto pCount = InstrTools::readRelOffset(p + 2, p + 6);
		auto instrCount = 0;
		auto next1 = next;
		cmbyte* pItems = nullptr;
		while (next1 < till) {
			p = next1;
			if (InstrTools::stepOverCheck(&next1)) break;
			if (++instrCount > 6) break;
			if (InstrTools::isEnd(p)) break;

			if (p[0] != 0x48/* MOV */) continue;
			if ((next1 - p) != 7) continue;
			pItems = InstrTools::readRelOffset(p + 3, p + 7);
			break;
		}
		if (!pItems) continue;

		gmlShaders.native.pCount = (PtrTo<int>)pCount;
		gmlShaders.native.pItems = (PtrTo<ArrayOf<PtrTo<YYNativeShader>>>)pItems;

		auto shaderCount = *pCount;
		gmlOriginal.native.count = shaderCount;
		gmlOriginal.native.items = *gmlShaders.native.pItems;

		auto newArr = malloc_arr<YYNativeShader*>((size_t)shaderCount);
		memcpy_arr(newArr, *gmlShaders.native.pItems, (size_t)shaderCount);
		*gmlShaders.native.pItems = newArr;

		trouble = false;
		return true;
	}
	return (int)shader_replace_unsafe_init_error::no_native_shader_array;

}

dllx double shader_replace_unsafe_init_5(cmbyte* q_shader_get_sampler_index) {
	f_shader_get_sampler_index = (GMLFunc*)csrGetCppAddr(q_shader_get_sampler_index, funcOffset);
	return true;
}

dllg void shader_replace_unsafe_cleanup() {
	*gmlShaders.wrappers.pItems = gmlOriginal.wrappers.items;
	*gmlShaders.wrappers.pCount = gmlOriginal.wrappers.count;
	*gmlShaders.native.pItems = gmlOriginal.native.items;
	*gmlShaders.native.pCount = gmlOriginal.native.count;
}