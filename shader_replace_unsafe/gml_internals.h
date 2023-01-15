#pragma once
#include "stdafx.h"
#include <d3d11.h>

const int VALUE_REAL = 0;		// Real value
const int VALUE_STRING = 1;		// String value
const int VALUE_ARRAY = 2;		// Array value
const int VALUE_OBJECT = 6;		// YYObjectBase* value 
const int VALUE_INT32 = 7;		// Int32 value
const int VALUE_UNDEFINED = 5;	// Undefined value
const int VALUE_PTR = 3;		// Ptr value
const int VALUE_VEC3 = 4;		// Deprecated : unused : Vec3 (x,y,z) value (within the RValue)
const int VALUE_VEC4 = 8;		// Deprecated : unused :Vec4 (x,y,z,w) value (allocated from pool)
const int VALUE_VEC44 = 9;		// Deprecated : unused :Vec44 (matrix) value (allocated from pool)
const int VALUE_INT64 = 10;		// Int64 value
const int VALUE_ACCESSOR = 11;	// Actually an accessor
const int VALUE_NULL = 12;		// JS Null
const int VALUE_BOOL = 13;		// Bool value
const int VALUE_ITERATOR = 14;	// JS For-in Iterator
const int VALUE_REF = 15;		// Reference value (uses the ptr to point at a RefBase structure)
#define MASK_KIND_RVALUE	0x0ffffff
const int VALUE_UNSET = MASK_KIND_RVALUE;

struct RefString {
	const char* text;
	int refCount;
	int size;
};
struct RValue {
	union {
		int v32;
		int64_t v64;
		double real;
		RefString* str;
		void* ptr;
	};
	uint32_t flags;
	uint32_t kind;

	int getInt32(int defValue) {
		switch (kind & MASK_KIND_RVALUE) {
			case VALUE_REAL: case VALUE_BOOL: return (int)real;
			case VALUE_INT32: return v32;
			case VALUE_INT64: return (int)v64;
			default: return defValue;
		}
	}
};
class CInstanceBase {
public:
	void* varArray;
	virtual ~CInstanceBase() {};
	virtual RValue& getYyVarRef(int index) = 0;
	virtual RValue& getYyVarRefL(int index) = 0;
};
class YYObjectBase : public CInstanceBase {
public:
	YYObjectBase* next;
	YYObjectBase* prev;
	YYObjectBase* prototype;
	const char* gmlClass;
	void* getOwnProperty;
	void* deleteProperty;
	void* defineOwnProperty;
	void* varMap;
	void* weakRefs;
	uint32_t numWeakRefs;
	uint32_t numVars;
	uint32_t flags;
	uint32_t capacity;
	uint32_t visited;
	uint32_t visitedGC;
	int32_t GCgeneration;
	int32_t GCcreationFrame;
	int32_t slot;
	int32_t kind;
	int32_t rvalueInitType;
	int32_t currentSlot;

	virtual ~YYObjectBase() {};
	virtual bool markForGC(uint32_t*, int) = 0;
	virtual bool markOnlyThisForGC(uint32_t*, int) = 0;
	virtual bool markOnlyChildrenForGC(uint32_t*, int) = 0;

	virtual void free(bool) = 0;
	virtual void threadFree(bool, void* = 0) = 0;
	virtual void preFree() = 0;
	virtual RValue* getDispose() = 0;
};
class CInstance : public YYObjectBase {
public:
	// todo
};
class CScriptRef : public YYObjectBase {
public:
	void* callVM;
	void* callCpp;
	void* callYYC;
};

typedef void(GMLFunc)(RValue& result, CInstance* self, CInstance* other, int argc, RValue* argv);

struct YYShaderDataHeader {
	int version;
	int constantBufferCount;
	int constantVariableCount;
	int samplerCount;
	int textureCount;
	int inputCount;
	int shaderSize;
	void* mysteries[5];
	void* shaderData;
};
enum class YYShaderKind : int {
	Vertex = 0,
	Pixel,
};
// The following will look bad because `register` is a keyword in C++
struct YYShaderConstBuf {
	int reg;
	int size; // rounded up?
	YYShaderKind type = YYShaderKind::Vertex;
	uint8_t* data;
	ID3D11Buffer* buffer;
	bool dirty;
};
struct YYShaderSampler {
	const char* name;
	int reg;
	YYShaderKind type = YYShaderKind::Pixel;
};

enum class YYShaderVarType : int {
	Void = 0,
	Bool,
	Int,
	Uint,
	Byte,
	Float,
	Double,
};
struct YYShaderConstBufVar {
	const char* name;
	uint8_t* data; // = buffer->data + this->offset
	int bufferIndex;
	int offset;
	int size;
	YYShaderVarType type;
	int cols;
	int rows;
	int elementCount;
};

enum class YYShaderInputSemantic : int {
	None = 0,
	Position,
	Color,
	Normal,
	TexCoord,
	BlendWeight,
	BlendIndex,
	PSize,
	Tangent,
	Binormal,
	Unknown1,
	Unknown2,
	Fog,
	Depth,
	Sample,
};
struct YYShaderInputParam {
	YYShaderInputSemantic semantic;
	int semanticIndex;
};
struct YYShaderInputLayout {
	int format;
	ID3D11InputLayout* layout;
};

struct YYNativeShader {
	ID3D11VertexShader* vertexShader = nullptr;
	ID3D11PixelShader* pixelShader = nullptr;

	YYShaderDataHeader* vertexHeader = nullptr;
	YYShaderDataHeader* pixelHeader = nullptr;

	uint8_t mysteryZero = 0;

	int constBufferCount = 0;
	YYShaderConstBuf* constBuffers = nullptr;

	int samplerCount = 0;
	YYShaderSampler* samplers = nullptr;

	int constBufVarCount = 0;
	YYShaderConstBufVar* constBufVars = nullptr;

	int inputCount = 0;
	YYShaderInputParam* inputs = nullptr;

	int inputLayoutCount = 0;
	YYShaderInputLayout* inputLayouts = nullptr;
	int lastUsedInputLayout = -1;
	YYNativeShader() {}
	~YYNativeShader() {
		// oh no
	}
};
///
enum class shader_kind { // snake_case because it gets exported to GML
	none = 0,
	glsl_es,
	glsl,
	hlsl_9,
	hlsl_11,
};
struct YYShaderPair {
	void* vertexShader = nullptr;
	void* fragmentShader = nullptr;
};
struct YYShader {
	int id;
	const char* name;
	shader_kind kind;
	YYShaderPair GLSLES, GLSL, HLSL9, HLSL11;

	YYShaderPair mysteryPairs[3];
	int mysteryPadding[4];

	const char* errorText;
	bool isFragError;

	int attributeCount;
	ArrayOf<const char*> attributeNames;

	union {
		int shaderHandle;
		void* shaderPointer;
	};

	// the following are IDs of default uniforms/etc.:
	int gm_BaseTexture;
	int gm_Matrices;
	int gm_Lights_Direction;
	int gm_Lights_PosRange;
	int gm_Lights_Colour;
	int gm_AmbientColour;
	int gm_LightingEnabled;
};