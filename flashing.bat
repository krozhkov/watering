@echo off
set avrdudePath=%USERPROFILE%\.platformio\packages\tool-avrdude\avrdude
set avrdudeConf=%USERPROFILE%\.platformio\packages\tool-avrdude\avrdude.conf

for /R . %%x in (*.hex) do (
	set hexFile=%%x
 	goto flashing
)

:flashing
%avrdudePath% -p atmega328p -C %avrdudeConf% -c usbasp -Pusb -U flash:w:%hexFile%:i -q
