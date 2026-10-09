#include <iostream>
#include "memalloc.h"


using namespace std;

int main() {
    int *ptr = (int*) malloc(sizeof(int));
    if (ptr == NULL)
    {
        printf("Memory allocation failed\n");
        return 1;
    }

    for (int i = 0; i < 5; i++) {
        ptr[i] = i + 1;
    }

    for (int i = 0; i < 5; i++) {
        cout << ptr[i] << endl;
    }

    free(ptr);

    return 0;
}