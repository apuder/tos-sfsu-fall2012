// These 2 static functions declared but never defined, so commented out.
//static float calcSin(float degree);
//static float sqrt(float n);

// This static function defined but never used, so commented out (feel free to
// resurrect if needed)
//static long factorial(int v);

static float power(float x, int y);

#ifndef __TOS_MATH__
#define __TOS_MATH__

static float PI = 3.14159265f;

void init_math() {
    asm("finit");
}

static float sinf(float degree) {
    float res;
    asm("fsin" : "=t" (res) : "0" (degree));
    return res;
}

static float cosf(float degree) {
    float res;
    asm("fcos" : "=t" (res) : "0" (degree));
    return res;
}

/*static long factorial(int v){
        long result = 1;
        int i;
        for(i=2; i<=v; ++i){
                result*= i;
        }
	
        return result;
}*/

static float power(float x, int y) {
    float result = x;
    int i;
    for (i = 1; i < y; i++) {
        result *= x;
    }

    return result;
}

float s_func(float xn, float z) {
    return xn / 2.0f + z / (2.0f * xn);
}

static float sqrtf(float n) {
    float res;
    asm("fsqrt" : "=t" (res) : "0" (n));
    return res;
}

#endif
