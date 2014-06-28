all:
	g++ -Wfatal-errors -lOpenCL -I/opt/AMDAPP/SDK/include/SDKUtil -I/opt/AMDAPP/SDK/include/ dragon.cpp -o dragon
clean:
	rm -rf *o dragon
