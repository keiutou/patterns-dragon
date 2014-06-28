#include <utility>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>
#include <iterator>
#include <climits>
#include <CL/cl.hpp>

namespace {
using std::string;
using std::cerr;
using std::cout;
using std::endl;
using std::ifstream;
using std::ofstream;
using std::istreambuf_iterator;
using std::make_pair;
using std::vector;
using std::ios;
using std::flush;
using std::ostream;

unsigned int testInteger() {
  unsigned int test = 1;
  test = test << 1;
//  test |= ((((n & -n) << 1) & n) != 0) & 1;
  test |= 0;
  test = test << 1;
  test |= 1;
  test = test << 1;
  test |= 1;
  test = test << 1;
  test |= 0;
  test = test << 1;
  test |= 1;
  test = test << 1;
  test |= 1;
  test = test << 1;
  test |= 1;
  test = test << 1;
  test |= 0;
  test = test << 1;
  test |= 1;
  test = test << 1;
  test |= 1;
  test = test << 1;
  test |= 1;
  test = test << 1;
  test |= 1;
  test = test << 1;
  test |= 0;
  test = test << 1;
  test |= 1;
  test = test << 1;
  test |= 1;
  test = test << 1;
  test |= 1;
  test = test << 1;
  test |= 1;
  test = test << 1;
  test |= 1;
  test = test << 1;
  test |= 0;
  test = test << 1;
  test |= 1;
  test = test << 1;
  test |= 1;
  test = test << 1;
  test |= 1;
  test = test << 1;
  test |= 1;
  test = test << 1;
  test |= 1;
  test = test << 1;
  test |= 1;
  test = test << 1;
  test |= 0;
  test = test << 1;
  test |= 1;
  test = test << 1;
  test |= 1;
  test = test << 1;
  test |= 1;
  test = test << 1;
  test |= 1;
  test = test << 1;
  test |= 1;
  return test;
}

// starts with the high bit
void prettyPrintTurns(unsigned int val, ostream& stream) {
  for(unsigned int i = sizeof(unsigned int) * CHAR_BIT - 1, end = ~0; i != end; --i) {
    stream << ((val & (1 << i)) != 0 ? 'L' : 'R') << ' ';
  }
}
} // namespace

inline void checkErr(cl_int err, const char* name) {
  if(err == CL_SUCCESS)
    return;
  cerr << "ERROR: " << name << " (" << err << ")" << endl;
  exit(EXIT_FAILURE);
}

int main() {
  cl_int err;
  vector<cl::Platform> platformList;
  cl::Platform::get(&platformList);
  checkErr(platformList.size() != 0 ? CL_SUCCESS : -1, "cl::Platform::get");
  
  cerr << "Platform number is: " << platformList.size() << endl;
  
  string platformVendor;
  platformList[0].getInfo((cl_platform_info)CL_PLATFORM_VENDOR, &platformVendor);

  cerr << "Platform is by: " << platformVendor << "\n";
 
  cl_context_properties cprops[3] = {CL_CONTEXT_PLATFORM, (cl_context_properties)(platformList[0])(), 0};
  cl::Context context(CL_DEVICE_TYPE_GPU, cprops, NULL, NULL, &err);
  checkErr(err, "Context::Context()");


  const size_t length = 60000;
  unsigned int* outH = new unsigned int[length];
  const size_t outHLen = length * sizeof(unsigned int);

  cout << "debug outHLen: " << outHLen << endl;

  cl::Buffer outCL(context, CL_MEM_WRITE_ONLY|CL_MEM_USE_HOST_PTR, outHLen, outH, &err);
  checkErr(err, "Buffer::Buffer()");

  vector<cl::Device> devices = context.getInfo<CL_CONTEXT_DEVICES>();
  checkErr(devices.size() > 0 ? CL_SUCCESS : -1, "devices.size() > 0");

  const string kernelName = "dragon.cl";
  std::ifstream file(kernelName.c_str());
  checkErr(file.is_open() ? CL_SUCCESS : -1, kernelName.c_str());
  
  string programString(istreambuf_iterator<char>(file), (istreambuf_iterator<char>()));
  cl::Program::Sources source(1, make_pair(programString.c_str(), programString.length() + 1));
  cl::Program program(context, source);
  err = program.build(devices, "");
  checkErr(err, "Program::build()");

  cl::Kernel kernel(program, "dragon", &err);
  checkErr(err, "Kernel::Kernel()");
  err = kernel.setArg(0, outCL);
  checkErr(err, "Kernel::setArg()");

  
  cl::CommandQueue queue(context, devices[0], 0, &err);
  checkErr(err, "CommandQueue::CommandQueue()");

  cl::Event event;
  err = queue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(outHLen), cl::NDRange(1, 1), NULL, &event);
  checkErr(err, "CommandQueue::enqueueNDRangeKernel()");

  event.wait();
  err = queue.enqueueReadBuffer(outCL, CL_TRUE, 0, outHLen, outH);
  checkErr(err, "CommandQueue::enqueueReadBuffer()");

  cout << "outputting..." << flush;

  const bool binary = false;
  const string outputFileName = "output";

  if(binary) {
    ofstream file;
    file.open(outputFileName.c_str(), ios::binary);
    file.write((char*)outH, outHLen);
  } else {
    ofstream outputFile;
    outputFile.open(outputFileName.c_str());
    for(unsigned int i = 0, end = length; i != end; ++i) {
      prettyPrintTurns(outH[i], outputFile);
    }

  }
  cout << "output to " << outputFileName << endl;
  cout << endl;
  return EXIT_SUCCESS;
}
