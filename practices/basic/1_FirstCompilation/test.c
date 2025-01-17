int a() {
    int i = 0;
    /*
    int i  --->  alloca i32, align 4 (%1 in the IR)
    i = 0; --->  store i32 0, ptr %1, align 4
    */
    int a = 0;
    /*
    same logic for int a = 0;
    */

    // 
    for (i=0; i<30; i++) {
        a += 1;
    }
    return a;
}

int main() {
    a();
}
