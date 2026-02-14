@echo off
setlocal enabledelayedexpansion

REM Alle .miec Dateien kompilieren
for %%F in (*.miec) do (
    echo Kompiliere %%F ...
    .\MIECCompiler.exe -in "%%F" -out "%%~nF.iex"
)

REM Alle erzeugten .iex Dateien ausführen
for %%F in (*.iex) do (
    echo Fuehre %%F aus ...
    .\VMRiscV "%%F"
)

echo Fertig.