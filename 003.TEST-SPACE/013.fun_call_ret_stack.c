__attribute__((noinline)) 
int square(int n) { return n * n; }

__attribute__((noinline)) 
int cube(int n) { return n * square(n); }

/**
 * arm:
 *    arm-gnu-toolchain-14.3.rel1-darwin-arm64-aarch64-none-elf/bin/aarch64-none-elf-gcc
 * -S -O0 013.fun_call_ret_stack.c -o 013.fun_call_ret_stack.s
 */