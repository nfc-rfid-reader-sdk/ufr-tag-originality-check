# Digital Logic
# www.d-logic.net
#
# Helper Makefile
#
$(VERBOSE).SILENT:

PRJNAME = originality_check_demo

all : help

help info : 
	echo "----------------------------------------------"
	echo "Project output name: $(PRJNAME)"
	echo "----------------------------------------------"
	echo "For Windows - type:        'make win32'"
	echo "For Linux   - type:        'make lin32'"
	echo "For OSX     - type:        'make osx'"
	echo "----------------------------------------------"

lin32 :
	cd Release_Linux && gcc -m32 -o $(PRJNAME) ../src/*.c -I../lib/include -L../lib/linux/x86 -luFCoder-x86 -Wl,-R../lib/linux/x86
	echo "Making done, without errors."
	echo "To run the example - type:"
	echo "   'cd Release_Linux && ./$(PRJNAME)'"

win32 :
	cd Release_Win32 && gcc -o $(PRJNAME) ../src/*.c -I../lib/include -L../lib/windows/x86 -luFCoder-x86 -Wl,-Rlib/windows/x86 -Wl,--enable-stdcall-fixup
	echo "Making done, without errors."
	echo "To run the example - type:"
	echo "   'cd Release_Win32'"
	echo "   'run_me'"

osx :
	echo "----------------------------------------------"
	cd Release_OSX && gcc -o $(PRJNAME) ../src/*.c -I../lib/include -L../lib/macos/x86_64 -luFCoder-x86_64
	echo "Making done, without errors."
	echo "You must use library from the 'lib\macos\x86_64\'"
	echo ""
	echo "To run the example - type:"
	echo "   'cd Release_OSX && ./$(PRJNAME)'"
    