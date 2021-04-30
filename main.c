#include <assert.h>
#include <inttypes.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define chex 1023
#define expsz 11
#define mtsz 52
#define dsz 63
#define eps 0.000000000000001

void prmt(uint64_t a)
{
    for (int i = 0; i < dsz; ++i) {
        printf("%lu ", (a & ((uint64_t)1 << i)) >> i);
    }
    printf("\n");
}

struct dnom {
    int sign;
    int exp;
    uint64_t mt;
};

typedef struct dnom dnom;

void condo(double a, dnom *nom)
{
    uint64_t *p = (uint64_t *)&a;
    nom->sign = (*p) >> dsz;
    (*p) = (*p) & (((uint64_t)1 << dsz) - 1);
    nom->exp = (*p) >> mtsz;
    nom->mt = ((*p) & (((uint64_t)1 << mtsz) - 1)) + ((uint64_t)1 << mtsz);
}

double comp(dnom nom)
{
    double ans;
    uint64_t *p = (uint64_t *)&ans;
    *p = 0;
    *p |= (uint64_t)nom.sign << dsz;
    *p |= (uint64_t)nom.exp << mtsz;
    *p |= nom.mt & (((uint64_t)1 << mtsz) - 1);
    return ans;
}

/*
double add(double da, double db)
{
    uint64_t *pa = (uint64_t *)&da;
    uint64_t *pb = (uint64_t *)&db;
    // a>=b absol
    if (((*pa) & (((uint64_t)1 << dsz) - 1)) <
        ((*pb) & (((uint64_t)1 << dsz) - 1))) {
        double c = da;
        da = db;
        db = c;
    }

    if (*pa == 0) {
        return da;
    }

    dnom a;
    dnom b;
    condo(da, &a);
    condo(db, &b);

    if (a.mt == b.mt && a.exp == b.exp && a.sign != b.sign) {
        return 0;
    }

    dnom ans;
    ans.sign = a.sign;
    uint64_t mt = 0;
    int de = a.exp - b.exp;
    ans.exp = a.exp;
    if (de > dsz) {
        de = dsz;
    }

    if (a.sign == b.sign) {
        mt = a.mt + (b.mt >> de);
    } else {
        // ans.exp-=2;
        mt = (a.mt) - (b.mt >> (de));
        while (mt < (uint64_t)1 << (mtsz + 1)) {
            mt *= 2;
            ans.exp--;
        }
    }

    // assert(!(mt<(uint64_t)1<<54 || de<0));

    while (mt >= (uint64_t)1 << (mtsz + 1)) {
        ans.exp++;
        mt = mt / 2;
    }
    //    prmt(mt);

    ans.mt = mt;
    return comp(ans);
}

*/

double mul(double da, double db)
{
    if (da <= eps || db <= eps) {
        return 0;
    }
    dnom a;
    dnom b;
    condo(da, &a);
    condo(db, &b);

    dnom ans;
    ans.sign = (a.sign + b.sign) % 2;
    ans.mt = 0;
    ans.exp = a.exp + b.exp - chex;
    /*
     for(int i=0;i<(mtsz+1);++i){
         for(int j=0;j<(mtsz+1);++j){
             if(i+j>=52){
                 ans.mt+=((a.mt>>(i))*((uint64_t)1<<i)*(b.mt>>j)*((uint64_t)1<<j))<<(i+j-52);
             }

         }
     }
    // ans.exp-=52-43;
     */

    for (int i = 0; i < (mtsz + 1); ++i) {
        ans.mt += (b.mt >> (mtsz - i)) * ((((uint64_t)1 << i) & (a.mt)) >> i);
        if ((b.mt >> (mtsz - i)) && ((((uint64_t)1 << i) & (a.mt)) >> i)) {
            // printf("%d\n",i);
        }
    }
    /*
    prmt(ans.mt);
    prmt(a.mt);
    prmt(b.mt);
    */
    while (ans.mt >= ((uint64_t)1 << (mtsz + 1))) {
        ans.exp++;
        ans.mt /= 2;
    }
    // prmt(ans.mt);
    return comp(ans);
}

int main(int ncom, char *com[ncom])
{
    if (ncom != 3) {
        return 1;
    }

    char *end;
    double a = strtod(com[1], &end);
    if (com[1] == end) {
        return 1;
    }

    double b = strtod(com[2], &end);
    if (com[2] == end) {
        return 1;
    }

    printf("%.16e", mul(a, b));
}