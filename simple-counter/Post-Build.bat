@echo on

rem Output File Name
set "param1=%~1"

rem Output File Directory
set "param2=%~2"

rem Copy-To Directory
set "param3=%~3"

if not exist "%param2%" (
	echo file 1 not found "%param2%" Exiting...
	exit -1
)

if not exist "%param3%" (
	echo file 2 not found "%param3%" Exiting...
	exit -2
) 

set "file1=%param2%\%param1%.dll"
set "file2=%param2%\%param1%.pdb"

echo Copying %file1% to %param3%.
echo Copying %file2% to %param3%.

copy "%file1%" "%param3%"
copy "%file2%" "%param3%"
echo Copy complete!



PAUSE
exit 0


