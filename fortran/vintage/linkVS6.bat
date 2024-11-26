call "C:\Program Files (x86)\Microsoft Visual Studio 6\VC98\Bin\VCVARS32.BAT"

link Release\hw_live.obj C:\MSDEV\LIB\console.lib C:\MSDEV\LIB\libf.lib C:\MSDEV\LIB\portlib.lib simpledraw2D.lib /OUT:Release\test.exe
