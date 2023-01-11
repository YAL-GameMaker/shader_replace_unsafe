#include "stdafx.h"
#include "instr_tools.h"
#include <DbgEng.h>

IDebugClient* debugClient = nullptr;
IDebugControl* debugControl = nullptr;
IDebugSymbols* debugSymbols = nullptr;

namespace InstrTools {
	bool init() {
		if (debugControl) return true;
		HRESULT status;

		if ((status = DebugCreate(__uuidof(IDebugClient), (void**)&debugClient)) != S_OK) {
			trace("IDebugClient DebugCreate failed: 0x%X\n", status);
			return false;
		}

		debugClient->AttachProcess(NULL, GetProcessId(GetCurrentProcess()), DEBUG_ATTACH_NONINVASIVE | DEBUG_ATTACH_NONINVASIVE_NO_SUSPEND);

		if ((status = debugClient->QueryInterface(__uuidof(IDebugControl), (void**)&debugControl)) != S_OK) {
			trace("IDebugControl QueryInterface failed: 0x%X\n", status);
			return false;
		}

		if ((status = debugClient->QueryInterface(__uuidof(IDebugSymbols), (void**)&debugSymbols)) != S_OK) {
			trace("IDebugSymbols QueryInterface failed: 0x%X\n", status);
			return false;
		}
		debugControl->WaitForEvent(DEBUG_WAIT_DEFAULT, 100);
		return true;
	}
	cmbyte* readRelOffset(cmbyte* at, cmbyte* referencePoint) {
		int64_t offset = (int64_t)at[0];
		offset |= ((int64_t)at[1]) << 8;
		offset |= ((int64_t)at[2]) << 16;
		offset |= (int64_t)(*(int8_t*)(at + 3)) << 24;
		return referencePoint + offset;
	}

	cmbyte* stepOver(cmbyte* at) {
		if (!debugControl) return nullptr;
		uintptr_t next = 0;
		auto hr = debugControl->Disassemble((uintptr_t)at, 0, nullptr, 0, nullptr, &next);
		return (uint8_t*)next;
	}
	cmbyte* findFuncWithCallCount(cmbyte* start, size_t max, int count) {
		auto p = start;
		auto till = p + max;
		while (p < till) {
			if (isEnd(p)) break;
			if (isRelCallOrJump(p)) {
				auto fn = readRelCallTarget(p);
				auto p1 = p;
				auto found = 0;
				while (p1 < till) {
					if (isEnd(p1)) break;
					if (isRelCallOrJump(p1)) {
						if (readRelCallTarget(p1) == fn) found += 1;
					}
					if (stepOverCheck(&p1)) break;
				}
				if (found == count) return fn;
			}
			if (stepOverCheck(&p)) break;
		}
		return nullptr;
	}
	cmbyte* findFuncCallAfter(cmbyte* start, size_t max, cmbyte* refCall) {
		auto p = start;
		auto till = p + max;
		auto next = false;
		while (p < till) {
			if (isEnd(p)) break;
			if (isRelCallOrJump(p)) {
				auto fn = readRelCallTarget(p);
				if (next) {
					if (fn != refCall) return fn;
				} else {
					if (fn == refCall) next = true;
				}
			}
			if (stepOverCheck(&p)) break;
		}
		return nullptr;
	}
}
