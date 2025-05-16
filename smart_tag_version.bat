@echo off
:: Get latest tag
for /f "tokens=* USEBACKQ" %%F in (`git describe --tags --abbrev=0`) do (set LAST_TAG=%%F)

:: Split version into parts
for /f "tokens=1,2,3 delims=." %%a in ("%LAST_TAG:~1%") do (
    set MAJOR=%%a
    set MINOR=%%b
    set /a PATCH=%%c + 1
)

set NEW_TAG=v%MAJOR%.%MINOR%.%PATCH%
echo Last tag: %LAST_TAG%
echo New tag: %NEW_TAG%

set /p MESSAGE=Enter commit message: 

git add .
git commit -m "%MESSAGE%"
git tag %NEW_TAG%
git push
git push origin %NEW_TAG%

echo.
echo ✅ Code pushed and version %NEW_TAG% tagged!
pause

set /p ISSUE=Enter issue number to close (leave empty to skip): 

if not "%ISSUE%"=="" (
    gh issue close %ISSUE% -c "✅ Fixed in %NEW_TAG%: %MESSAGE%"
    echo Issue #%ISSUE% closed.
) else (
    echo No issue number provided. Skipping issue close.
)
