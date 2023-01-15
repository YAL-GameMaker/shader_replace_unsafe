#include "stdafx.h"
#include "shared.h"
#include <vector>

// re-declared for gmlCppExtFuncs
typedef int shader_id;
/// @dllg:docName shader_id shader
typedef int shader_uniform_id;
/// @dllg:docName shader_uniform_id shader_uniform

YYOriginalShaders gmlOriginal{};
YYShaders gmlShaders{};
bool trouble = true;
YYRunnerInterface yyRunnerInterface{};
