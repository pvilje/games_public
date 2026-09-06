@echo off
REM Compile resource script to embed icon (assets/logo.ico)
rc.exe /fo icon.res icon.rc 2>nul
if %errorlevel% neq 0 (
	echo rc.exe not found or resource compilation failed. Building without icon.
	cl minesweeper.c "lib/raylibdll.lib" "winmm.lib" /link /SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup
) else (
	echo Resource compiled to icon.res. Building with icon.
	cl minesweeper.c "lib/raylibdll.lib" "winmm.lib" icon.res /link /SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup
)