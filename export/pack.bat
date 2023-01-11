@echo off

if not exist "shader_replace_unsafe-for-GMS1" mkdir "shader_replace_unsafe-for-GMS1"
cmd /C copyre ..\shader_replace_unsafe.gmx\extensions\shader_replace_unsafe.extension.gmx shader_replace_unsafe-for-GMS1\shader_replace_unsafe.extension.gmx
cmd /C copyre ..\shader_replace_unsafe.gmx\extensions\shader_replace_unsafe shader_replace_unsafe-for-GMS1\shader_replace_unsafe
cmd /C copyre ..\shader_replace_unsafe.gmx\datafiles\shader_replace_unsafe.html shader_replace_unsafe-for-GMS1\shader_replace_unsafe\Assets\datafiles\shader_replace_unsafe.html
cd shader_replace_unsafe-for-GMS1
cmd /C 7z a shader_replace_unsafe-for-GMS1.7z *
move /Y shader_replace_unsafe-for-GMS1.7z ../shader_replace_unsafe-for-GMS1.gmez
cd ..

if not exist "shader_replace_unsafe-for-GMS2\extensions" mkdir "shader_replace_unsafe-for-GMS2\extensions"
if not exist "shader_replace_unsafe-for-GMS2\datafiles" mkdir "shader_replace_unsafe-for-GMS2\datafiles"
if not exist "shader_replace_unsafe-for-GMS2\datafiles_yy" mkdir "shader_replace_unsafe-for-GMS2\datafiles_yy"
cmd /C copyre ..\shader_replace_unsafe_yy\extensions\shader_replace_unsafe shader_replace_unsafe-for-GMS2\extensions\shader_replace_unsafe
cmd /C copyre ..\shader_replace_unsafe_yy\datafiles\shader_replace_unsafe.html shader_replace_unsafe-for-GMS2\datafiles\shader_replace_unsafe.html
cmd /C copyre ..\shader_replace_unsafe_yy\datafiles_yy\shader_replace_unsafe.html.yy shader_replace_unsafe-for-GMS2\datafiles_yy\shader_replace_unsafe.html.yy
cd shader_replace_unsafe-for-GMS2
cmd /C 7z a shader_replace_unsafe-for-GMS2.zip *
move /Y shader_replace_unsafe-for-GMS2.zip ../shader_replace_unsafe-for-GMS2.yymp
cd ..

if not exist "shader_replace_unsafe-for-GMS2.3+\extensions" mkdir "shader_replace_unsafe-for-GMS2.3+\extensions"
if not exist "shader_replace_unsafe-for-GMS2.3+\datafiles" mkdir "shader_replace_unsafe-for-GMS2.3+\datafiles"
cmd /C copyre ..\shader_replace_unsafe_23\extensions\shader_replace_unsafe shader_replace_unsafe-for-GMS2.3+\extensions\shader_replace_unsafe
cmd /C copyre ..\shader_replace_unsafe_23\datafiles\shader_replace_unsafe.html shader_replace_unsafe-for-GMS2.3+\datafiles\shader_replace_unsafe.html
cd shader_replace_unsafe-for-GMS2.3+
cmd /C 7z a shader_replace_unsafe-for-GMS2.3+.zip *
move /Y shader_replace_unsafe-for-GMS2.3+.zip ../shader_replace_unsafe-for-GMS2.3+.yymps
cd ..

pause