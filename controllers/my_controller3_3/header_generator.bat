@echo off

rem このバッチファイルが格納されているフォルダ名を取得する方法

setlocal
set THIS_PATH=%~dp0
for %%1 in ("%THIS_PATH:~0,-1%") do set FOLDER_NAME=%%~nx1

rem フォルダ名取得の確認

echo.
echo Current directory is "%CD%"
echo Full path is "%~f0"
echo %%~dp0 is "%~dp0"
echo.
echo Folder name is "%FOLDER_NAME%"

rem Makefileの修正

echo space :=> Makefile
echo space +=>> Makefile
echo WEBOTS_HOME_PATH=$(subst $(space),\ ,$(strip $(subst \,/,$(WEBOTS_HOME))))>> Makefile
echo C_SOURCES = %FOLDER_NAME%.c tinkerbots_utility.c>> Makefile
echo include $(WEBOTS_HOME_PATH)/resources/Makefile.include>> Makefile

rem ヘッダファイル，ソースファイル，オブジェクトファイルのコピー
copy ..\..\libraries\tinkerbots_utility.h .\
copy ..\..\libraries\tinkerbots_utility.c .\
copy ..\..\libraries\tinkerbots_utility.d .\build\release
copy ..\..\libraries\tinkerbots_utility.o .\build\release




