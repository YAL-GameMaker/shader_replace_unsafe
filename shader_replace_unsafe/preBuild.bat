@echo off
set dllPath=%~1
set solutionDir=%~2
set projectDir=%~3
set arch=%~4
set config=%~5

echo Running pre-build for %config%

where /q GmlCppExtFuncs
if %ERRORLEVEL% EQU 0 (
	echo Running GmlCppExtFuncs...
	GmlCppExtFuncs ^
	--prefix shader_replace_unsafe^
	--include "gml_internals.h"^
	--include "shared.h"^
	--cpp "%projectDir%autogen.cpp"^
	--gml "%solutionDir%shader_replace_unsafe_23/extensions/shader_replace_unsafe/autogen.gml"^
	%projectDir%init.cpp ^
	%projectDir%shared.cpp ^
	%projectDir%shader_replace_unsafe.cpp
)