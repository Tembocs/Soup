@echo off
if not defined CONFIG (
    set CONFIG=Debug
)
SET ScriptsDir=%~dp0
dotnet %ScriptsDir%..\bin\Soup.Client\%CONFIG%\netcoreapp2.1\Soup.dll %*