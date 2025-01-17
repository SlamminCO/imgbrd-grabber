:: Package everything into a "Grabber.zip" file at the root of the git repository

:: Copy all required files to the release directory
bash scripts/package.sh "release"
xcopy /I /E /Y /EXCLUDE:cpex.txt "src/sites" "release/sites/"
xcopy /I /E /K /H "src/dist/windows" "release"

:: Add Qt DLL and files
%Qt5_Dir%/bin/windeployqt --dir "release" "release/Grabber.exe"

:: Add OpenSSL and MySQL DLL
copy %OPENSSL_ROOT_DIR%/libcrypto-1_1*.dll "release"
copy %OPENSSL_ROOT_DIR%/libssl-1_1*.dll "release"
copy %MYSQL_DRIVER_DIR%/libmysql.dll "release"

:: Zip the whole directory
pushd release
    7z a -r "../Grabber.zip" .
popd

:: Cleanup
::rmdir /S /Q release
