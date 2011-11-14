libpath = /LIBPATH:"C:\Program Files (x86)\Microsoft SDKs\Windows\v7.0A\Lib" /LIBPATH:"C:\Program Files (x86)\Microsoft Visual Studio 10.0\VC\lib"
libpath64 = /LIBPATH:"C:\Program Files (x86)\Microsoft SDKs\Windows\v7.0A\Lib\x64" /LIBPATH:"C:\Program Files (x86)\Microsoft Visual Studio 10.0\VC\lib\amd64"
cl = "C:\Program Files (x86)\Microsoft Visual Studio 10.0\VC\bin\cl.exe"
cl64 = "C:\Program Files (x86)\Microsoft Visual Studio 10.0\VC\bin\amd64\cl.exe"
link = "C:\Program Files (x86)\Microsoft Visual Studio 10.0\VC\bin\link.exe"
link64 = "C:\Program Files (x86)\Microsoft Visual Studio 10.0\VC\bin\amd64\link.exe"
clparam = /nologo /EHsc /c /D "_CONSOLE" /D "WIN32" /D "NDEBUG" /D "_UNICODE" /D "UNICODE" /Zc:wchar_t /Zc:forScope /Gd /GS /Gy /GL /W3 /O2 /Oi
linkparam = /LTCG /RELEASE /SUBSYSTEM:CONSOLE /NOLOGO /MACHINE:X86 /DYNAMICBASE /NXCOMPAT /OPT:REF /OPT:ICF /ALLOWISOLATION /INCREMENTAL:NO $(libpath)
linkparam64 = /LTCG /RELEASE /SUBSYSTEM:CONSOLE /NOLOGO /MACHINE:X64 /DYNAMICBASE /NXCOMPAT /OPT:REF /OPT:ICF /ALLOWISOLATION /INCREMENTAL:NO $(libpath64)
csharp = csc.exe
csparam = /nologo /target:exe
objs = evttocsv.obj
objsx = evtxtocsv.obj
objs64 = evttocsv64.obj
objs64x = evtxtocsv64.obj
libs = kernel32.lib Wevtapi.lib
target = evttocsv.exe
targetx = evtxtocsv.exe
target64 = evttocsv64.exe
target64x = evtxtocsv64.exe
cstarget = evt2txt.exe

all: $(target) $(target64) $(targetx) $(target64x) $(cstarget)

x64: $(target64) $(target64x) $(cstarget)

clean:
	del $(objs) $(objs64) $(objsx) $(objs64x) $(target) $(target64) $(targetx) $(target64x) $(cstarget)

evttocsv.obj: evttocsv.cpp
	$(cl) $(clparam) /Fo"$@" $**

evttocsv64.obj: evttocsv.cpp
	$(cl64) $(clparam) /Fo"$@" $**

evtxtocsv.obj: evtxtocsv.cpp
	$(cl) $(clparam) /Fo"$@" $**

evtxtocsv64.obj: evtxtocsv.cpp
	$(cl64) $(clparam) /Fo"$@" $**

$(target): $(objs)
	$(link) $(linkparam) /OUT:$(target) $(libs) $**

$(target64): $(objs64)
	$(link64) $(linkparam64) /OUT:$(target64) $(libs) $**

$(targetx): $(objsx)
	$(link) $(linkparam) /OUT:$(targetx) $(libs) $**

$(target64x): $(objs64x)
	$(link64) $(linkparam64) /OUT:$(target64x) $(libs) $**

$(cstarget): evt2txt.cs
	$(csharp) $(csparam) /platform:anycpu /OUT:$@ $**
