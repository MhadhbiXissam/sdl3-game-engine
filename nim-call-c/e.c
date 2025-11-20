#include <stdio.h>
#include <dlfcn.h>  // For Linux/macOS

int main() {
    // Load the shared library
    void *handle = dlopen("./libe.so", RTLD_LAZY);
    if (!handle) {
        fprintf(stderr, "Error loading library: %s\n", dlerror());
        return 1;
    }

    // Clear previous errors
    dlerror();

    // Load the NimMain symbol
    void (*NimMain)(void) = (void (*)(void)) dlsym(handle, "NimMain");
    char *error = dlerror();
    if (error != NULL) {
        fprintf(stderr, "Error finding symbol NimMain: %s\n", error);
        dlclose(handle);
        return 1;
    }

    // Call NimMain
    NimMain();
    // Load the symbol
    int (*add)(int, int) = (int (*)(int,int)) dlsym(handle, "add");

    if (error != NULL) {
        fprintf(stderr, "Error finding symbol: %s\n", error);
        dlclose(handle);
        return 1;
    }

    // Call the function
    int result = add(5, 7);
    printf("add(5,7) = %d\n", result);
    typedef void (*PrintNameFunc)(const char*);
    PrintNameFunc printName = (PrintNameFunc)dlsym(handle, "printNameFromJson");
    *error = dlerror();
    if (error != NULL) {
        fprintf(stderr, "Error finding symbol: %s\n", error);
        dlclose(handle);
        return 1;
    }

    // Call function
    char* jsonStr = "{\"name\":\"Issaù\",\"age\":30,\"languages\":[\"Nim\",\"Python\",\"C\"]}";
    printName(jsonStr);

    // Close the library
    dlclose(handle);

    return 0;
}
