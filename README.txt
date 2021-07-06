This Virtual Cam3 is a DXGI directshow screen capture filter (only Windows 8+)
and uses portions of code of screen-capture-recorder and SCFH DSF filters.
-Zero config, fixed frame rate of 30 fps (is intended for movie captures), 32 bit color, 
capture at monitor resolution (no resize for improved speed), DXGI acceleration.

-tested with ffmpeg, minidlna-for-windows, oshare mod version with desktop streaming and virtualdub2 
(minidlna requires alternative name).
 
-Install:

To install  VCAM3 filter (64 bit Windows):

-copy 32 bit vcam3.dll (release folder) to Windows\Syswow64 folder.
-open a command prompt with administrator privileges, go to Windows\Syswow64 and register the dll:

regsvr32 vcam3.dll

-copy 64bit (x64/release folder) vcam3.dll to Windows\system32 folder
-open a command prompt with administrator privileges, go to Windows\System32 and register the dll:

regsvr32 vcam3.dll

To uninstall: go to folders above and unregister dlls with: 

regsvr32 -u vcam3.dll
and delete vcam3.dll.,

Filter not have configuration or registry settings.
---------------
Capture with minidlna-for-windows (cygwin mod version https://github.com/xpto999us/Minidlna-For-Windows-with-Transcoding):

Check Virtual Cam3 alternative name (minidlna.conf command line do not accept white spaces): 

ffmpeg -list_devices true -f dshow -i dummy > output.txt 2>&1

Check output.txt for alternative name of Virtual Cam3 and copy to minidlna.conf

-see new minidlna.conf (#DESKTOP STREAMING) for details and command line.
=============================================================================



