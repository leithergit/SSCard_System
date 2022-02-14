@echo off
cd ..
@echo update SDK to %~dp0%1

:: copy /y ..\sdk.git\Printer\KT_Printer\%1\*.*				%~dp0\%1
:: copy /y ..\sdk.git\Reader\KT_Reader\%1\*.*					%~dp0\%1
:: copy /y ..\sdk.git\sd_sscardinfo\SD_SSCardInfo\%1\*.* 		%~dp0\%1
:: copy /y ..\sdk.git\sscarddriver\SSCardDriver\%1\*.*  		%~dp0\%1
:: copy /y ..\sdk.git\sscardhsm\SSCardHSM\%1\*.* 				%~dp0\%1
:: copy /y ..\sdk.git\SSCardInfo\SSCardInfo\%1\*.* 			%~dp0\%1

@echo update Printer Driver...
copy /y ..\SDK.git\Printer\KT_Printer\KT_Printer\KT_Printer.h .\SDK\KT_Printer
copy /y ..\sdk.git\Printer\KT_Printer\%1\*.*	.\SDK\KT_Printer

@echo update Reader Driver...
copy /y ..\sdk.git\Reader\KT_Reader\KT_Reader\KT_Reader.h	.\SDK\KT_Reader
copy /y ..\sdk.git\Reader\KT_Reader\%1\*.*	.\SDK\KT_Reader

@echo update SSCardDriver...
copy /y ..\sdk.git\sscarddriver\SSCardDriver\SSCardDriver\SSCardDriver.h .\SDK\SSCardDriver
copy /y ..\sdk.git\sscarddriver\SSCardDriver\%1\*.*  .\SDK\SSCardDriver

@echo update SSCardHSM...
copy /y ..\sdk.git\sscardhsm\SSCardHSM\SSCardHSM\KT_SSCardHSM.h .\SDK\SSCardHSM
copy /y ..\sdk.git\sscardhsm\SSCardHSM\%1\*.* .\SDK\SSCardHSM

@echo update SSCardInfo Driver...
copy /y ..\sdk.git\SSCardInfo\SSCardInfo\SSCardInfo\KT_SSCardInfo.h .\SDK\SSCardInfo_Henan
copy /y ..\sdk.git\SSCardInfo\SSCardInfo\%1\*.* .\SDK\SSCardInfo_Henan

@echo update SD_SSCardInfo...
copy /y ..\sdk.git\sd_sscardinfo\SD_SSCardInfo\SD_SSCardInfo\SD_SSCardInfo.h .\SDK\SSCardinfo_Sandong
copy /y ..\sdk.git\sd_sscardinfo\SD_SSCardInfo\%1\*.* .\SDK\SSCardinfo_Sandong



