mkdir ..\bin\Win32Release\
mkdir ..\bin\Win32Debug\
mkdir ..\bin\Win64Release\
mkdir ..\bin\Win64Debug\

copy lib\Win32\pthreadVC2.dll ..\bin\Win32Release\pthreadVC2.dll /Y
copy lib\Win32\pthreadVC2.dll ..\bin\Win32Debug\pthreadVC2.dll /Y

copy lib\Win64\pthreadVC2.dll ..\bin\Win64Release\pthreadVC2.dll /Y
copy lib\Win64\pthreadVC2.dll ..\bin\Win64Debug\pthreadVC2.dll /Y