mkdir ..\bin\Win32Release\
mkdir ..\bin\Win32Debug\
mkdir ..\bin\Win64Release\
mkdir ..\bin\Win64Debug\

copy lib\x86\pthreadVC2.lib ..\bin\Win32Release\pthread.lib /Y
copy lib\x86\pthreadVC2.lib ..\bin\Win32Debug\pthread.lib /Y
copy lib\x86\pthreadVC2.dll ..\bin\Win32Release\pthreadVC2.dll /Y
copy lib\x86\pthreadVC2.dll ..\bin\Win32Debug\pthreadVC2.dll /Y

copy lib\x64\pthreadVC2.lib ..\bin\Win64Release\pthread.lib /Y
copy lib\x64\pthreadVC2.lib ..\bin\Win64Debug\pthread.lib /Y
copy lib\x64\pthreadVC2.dll ..\bin\Win64Release\pthreadVC2.dll /Y
copy lib\x64\pthreadVC2.dll ..\bin\Win64Debug\pthreadVC2.dll /Y