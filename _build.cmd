:: Build file for scotools -- by swyter
:: Requires windres (from GCC's binutils) and TCC <bellard.org/tcc/>
@echo off && cls && title [swybuild] && goto :init

:build

  windres -v res/%1.rc res/%1.rc.o                                                 && echo.  ~Resources processed... && echo. 
  tcc -v %1.c scoutils/scoReader.c scoutils/scoWriter.c res/%1.rc.o -o _%1.exe     && echo.  ~Compilation done...    && echo. 
  goto :EOF
  
:init

  call :build scopng
  call :build pngsco
::call :build psdtopng

  echo Completed! Press any key to rebuild...
  pause > nul && echo. 
                 echo -------------  
                 echo.   
                 goto :init