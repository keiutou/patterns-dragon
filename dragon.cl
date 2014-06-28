#pragma OPENCL EXTENSION cl_khr_byte_addressable_store : enable

__kernel void dragon(__global unsigned int* out) {
  size_t tid = get_global_id(0);

  int n = tid * sizeof(unsigned int) * CHAR_BIT;

  size_t left = 0;
# pragma unroll
  for(size_t i = 0, end = sizeof(unsigned int) * CHAR_BIT; i != end; ++i) {
    ++n;
    left = left << 1;
    left |= ((((n & -n) << 1) & n) != 0) & 1;
  }
  out[tid] = left;
}
