#include <iostream>
#include "memalloc.h"


using namespace std;

int main() {
   
    int *ptr1 = (int*) malloc1(sizeof(int) * 5);
    int *ptr2 = (int*) malloc1(sizeof(int) * 5);

    for (int i = 0; i < 5; i++) {
        ptr1[i] = i + 1;
        ptr2[i] = 2*i + 1;
    }

    for (int i = 0; i < 5; i++) {
        cout << ptr1[i] + ptr2[5 - i - 1] << endl;
    }

    // free(ptr1);
    // free(ptr2);

    global_free();

    return 0;
}