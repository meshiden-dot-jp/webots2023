@echo off

rem ���̃o�b�`�t�@�C�����i�[����Ă���t�H���_�����擾������@

setlocal
set THIS_PATH=%~dp0
for %%1 in ("%THIS_PATH:~0,-1%") do set FOLDER_NAME=%%~nx1

rem �t�H���_���擾�̊m�F

echo.
echo Current directory is "%CD%"
echo Full path is "%~f0"
echo %%~dp0 is "%~dp0"
echo.
echo Folder name is "%FOLDER_NAME%"

rem Makefile�̏C��

echo space :=> Makefile
echo space +=>> Makefile
echo WEBOTS_HOME_PATH=$(subst $(space),\ ,$(strip $(subst \,/,$(WEBOTS_HOME))))>> Makefile
echo C_SOURCES = %FOLDER_NAME%.c tinkerbots_utility.c>> Makefile
echo include $(WEBOTS_HOME_PATH)/resources/Makefile.include>> Makefile

rem �w�b�_�t�@�C���C�\�[�X�t�@�C���C�I�u�W�F�N�g�t�@�C���̃R�s�[
copy ..\..\libraries\tinkerbots_utility.h .\
copy ..\..\libraries\tinkerbots_utility.c .\
copy ..\..\libraries\tinkerbots_utility.d .\build\release
copy ..\..\libraries\tinkerbots_utility.o .\build\release




