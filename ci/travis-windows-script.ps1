if ( $ENV:BUILD_DRIVER -like 'MSYS2*' ) {
$Env:MSYSTEM="MINGW64"
c:\tools\msys64\usr\bin\bash.exe -l -c "pwd; ci/travis-windows-script.sh || exit 1"
if ($LastExitCode -ne 0) {
    exit $LastExitCode 
}
}

if ( $ENV:BUILD_DRIVER -like 'MSVC*' ) {
cmake .
cmake --build .
}
