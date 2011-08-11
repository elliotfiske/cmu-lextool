
#include <stdio.h>

extern "C" {

__global__ void pokus() {
//    fprintf(stderr, "ahoj........................\n");
}

void runKernel() {
        pokus<<<10,10>>>();
}

}

