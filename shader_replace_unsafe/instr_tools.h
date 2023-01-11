#pragma once
typedef const uint8_t cmbyte;
namespace InstrTools {
	inline bool isEnd(cmbyte* at) {
		return *at == 0xCC;
	}
	inline bool isRelCallOrJump(cmbyte* at) {
		auto first = *at;
		return first == 0xE8 /* relative call*/ || first == 0xE9 /* relative jump */;
	}

	bool init();

	cmbyte* stepOver(cmbyte* at);
	inline bool stepOverCheck(cmbyte** at) {
		auto next = stepOver(*at);
		*at = next;
		return next == nullptr;
	}

	cmbyte* readRelOffset(cmbyte* at, cmbyte* referencePoint);
	inline cmbyte* readRelCallTarget(cmbyte* at) {
		return readRelOffset(at + 1, at + 5);
	}

	cmbyte* findFuncWithCallCount(cmbyte* start, size_t max, int count);
	cmbyte* findFuncCallAfter(cmbyte* start, size_t max, cmbyte* refCall);
}