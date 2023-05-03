int a[32]; // the global array

unsigned long crash_array(int n) {
    int i;
    unsigned long sum = 0;

    for (i = 0; i < sizeof(a)/sizeof(int); i++) {
        sum = sum + a[i];
    }

    return sum;
}

int main(void) {
    unsigned long s;

    s = crash_array(100000);
    printf("Hello world, the sum:%ld\n", s);
    return 0; 
}
