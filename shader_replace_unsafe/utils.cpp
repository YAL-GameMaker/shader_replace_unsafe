#include "stdafx.h"

class StringConv {
public:
    char* cbuf = NULL;
    size_t cbuf_size = 0;
    WCHAR* wbuf = NULL;
    size_t wbuf_size = 0;
    StringConv() {

    }
    void init() {
        cbuf = nullptr;
        cbuf_size = 0;
        wbuf = nullptr;
        wbuf_size = 0;
    }
    LPCWSTR wget(size_t size) {
        if (wbuf_size < size) {
            if (wbuf != nullptr) {
                wbuf = realloc_arr(wbuf, size);
            } else wbuf = malloc_arr<wchar_t>(size);
            wbuf_size = size;
        }
        return wbuf;
    }
    LPCWSTR proc(const char* src, int cp = CP_UTF8) {
        size_t size = MultiByteToWideChar(cp, 0, src, -1, NULL, 0);
        LPCWSTR buf = wget(size);
        MultiByteToWideChar(cp, 0, src, -1, wbuf, size);
        return wbuf;
    }
    char* get(size_t size) {
        if (cbuf_size < size) {
            if (cbuf != nullptr) {
                cbuf = realloc_arr(cbuf, size);
            } else cbuf = malloc_arr<char>(size);
            cbuf_size = size;
        }
        return cbuf;
    }
    char* proc(LPCWSTR src, int cp = CP_UTF8) {
        size_t size = WideCharToMultiByte(cp, 0, src, -1, NULL, 0, NULL, NULL);
        char* buf = get(size);
        WideCharToMultiByte(cp, 0, src, -1, buf, size, NULL, NULL);
        return buf;
    }
} c1;

double FileTimeToGMTime(FILETIME _t) {
	int64_t t = _t.dwLowDateTime | ((int64_t)_t.dwHighDateTime << 32);
	t -= (10000i64/* 100ns/ms */ * 1000 * 60 * 60 * 24) * 109205i64/* days between FILETIME/1601 and Delphi/1900 */;
	return (double)t / (10000. * 1000 * 60 * 60 * 24);
}
///
dllx double shader_replace_unsafe_get_mtime(const char* path) {
	HANDLE h = CreateFile(c1.proc(path), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (h == INVALID_HANDLE_VALUE) return 0;
	FILETIME t;
	if (!GetFileTime(h, NULL, NULL, &t)) { CloseHandle(h); return 0; }
	auto result = FileTimeToGMTime(t);
	CloseHandle(h);
	return result;
}