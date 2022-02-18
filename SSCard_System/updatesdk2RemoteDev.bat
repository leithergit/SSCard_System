@echo off
cd %2
cd ..
@echo update Printer Driver to remote device ...
copy /y ..\sdk.git\Printer\KT_Printer\%1\*.dll			Z:\

@echo update Reader Driver to remote device ...

copy /y ..\sdk.git\Reader\KT_Reader\%1\*.dll			Z:\

@echo update SSCardDriver to remote device ...
copy /y ..\sdk.git\sscarddriver\SSCardDriver\%1\*.dll  Z:\

@echo update SSCardHSM to remote device ...
copy /y ..\sdk.git\sscardhsm\SSCardHSM\%1\*.dll 		Z:\

@echo update SSCardInfo Driver to remote device ...
copy /y ..\sdk.git\SSCardInfo\SSCardInfo\%1\*.dll 		Z:\

@echo update SD_SSCardInfo to remote device ...
copy /y ..\sdk.git\sd_sscardinfo\SD_SSCardInfo\%1\*.dll Z:\



