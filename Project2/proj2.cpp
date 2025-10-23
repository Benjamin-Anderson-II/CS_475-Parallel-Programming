#include <cmath>
#include <cstdlib>
#define _USE_MATH_DEFINES

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <omp.h>


int     NowYear;    // 2024-2029
int     NowMonth;   // 0-11

float   NowPrecip;  // inches of rain per month
float   NowTemp;    // temperature this month
float   NowHeight;  // grain height in inches
int     NowNumDeer; // number of deer in the current population

int     NowNumBears; // number of bears in current population

const float GRAIN_GROWS_PER_MONTH   = 12.0;
const float ONE_DEER_EATS_PER_MONTH = 1.0;

const float AVG_PRECIP_PER_MONTH    = 7.0;  // Average
const float AMP_PRECIP_PER_MONTH    = 6.0;  // Plus or Minus
const float RANDOM_PRECIP           = 2.0;  // plus or minus noise

const float AVG_TEMP                = 60.0; // Average
const float AMP_TEMP                = 20.0; // Plus or Minus
const float RANDOM_TEMP             = 10.0; // plus or minus noise

const float MIDTEMP                 = 40.0;
const float MIDPRECIP               = 10.0;

unsigned int seed = 0;

#define SQR(x) x*x

void initBarrier(int numThreads);
void Deer();
void Grain();
void Watcher();
void Bears();

int
main (int argc, char *argv[]) {
    NowMonth = 0;
    NowYear  = 2024;
    NowNumDeer = 5;
    NowNumBears = 5;
    NowHeight  = 50.;
    printf("Month, Temp (C), Precip (in), Height (in), Deer, Bears\n");

    srand(seed);
    omp_set_num_threads(4);
    initBarrier(4);

    #pragma omp parallel sections
    {
        #pragma omp section
        {
            Deer();
        }

        #pragma omp section
        {
            Grain();
        }

        #pragma omp section
        {
            Watcher(); 
        }
        #pragma omp section 
        {
            Bears();
        }
    }

    return 0;
}

omp_lock_t      Lock;
volatile int    NumInThreadTeam;
volatile int    NumAtBarrier;
volatile int    NumGone;

void
initBarrier(int n){
    NumInThreadTeam = n;
    NumAtBarrier = 0;
    omp_init_lock(&Lock);
}

float
ranf(float low, float high) {
    float r = (float) rand();               // 0 - RAND_MAX
    float t = r  /  (float) RAND_MAX;       // 0. - 1.

    return   low  +  t * (high - low);
}

void
WaitBarrier() {
    omp_set_lock(&Lock);
    {
        NumAtBarrier++;
        if(NumAtBarrier == NumInThreadTeam){
            NumGone = 0;
            NumAtBarrier = 0;
            // let all other threads return before this one unlocks
            while(NumGone != NumInThreadTeam - 1);
            omp_unset_lock(&Lock);
            return;
        }
    }
    omp_unset_lock(&Lock);

    while(NumAtBarrier != 0);   // All threads wait here until the last one arrives

    #pragma omp atomic          // ... and sets NumAtBarrier to 0
        NumGone++;
}

void
Watcher() {
    while(NowYear < 2030){

        // Do nothing
        WaitBarrier(); // Done Computing

        // Do nothing
        WaitBarrier(); // Done Assigning
        
        // Print Current Data
        printf("%d, %f, %f, %f, %d, %d\n", (NowYear-2024) * (12) + NowMonth,
                                     (5. / 9.) * (NowTemp - 32),
                                     (NowPrecip),
                                     (NowHeight),
                                     (NowNumDeer),
                                     (NowNumBears)
              );

        // Calculate next cycle params
        float ang = (30. * (float)NowMonth + 15.) * (M_PI / 180.);
        
        float temp = AVG_TEMP - AMP_TEMP * cos(ang);
        NowTemp = temp + ranf(-RANDOM_TEMP, RANDOM_TEMP);
        
        float precip = AVG_PRECIP_PER_MONTH + AMP_PRECIP_PER_MONTH * sin(ang);
        NowPrecip = precip + ranf(-RANDOM_PRECIP, RANDOM_PRECIP);
        if(NowPrecip < 0) NowPrecip = 0;

        NowMonth = (NowMonth + 1) % 12;
        NowYear += (NowMonth == 0); // if not, evals to 0; adds nothing
        WaitBarrier(); // Done Printing
    }
}

void
Deer(){
    while(NowYear < 2030){

        // calculate next cycle data (if-less)
        int NextNumDeer = (NowNumDeer-1) * (NowNumDeer > NowHeight) + // if greater, minus 1
                          (NowNumDeer+1) * (NowNumDeer < NowHeight) + // if less, plus 1
                          (NowNumDeer)   * (NowNumDeer == NowHeight); // if same, nothing
        if(NowNumBears > NowNumDeer / 2 && (NowMonth > 3 && NowMonth < 10))
            NextNumDeer--;

        if(NextNumDeer < 0) NextNumDeer = 0;
        WaitBarrier(); // Done Computing

        // replace current cycle data
        NowNumDeer = NextNumDeer;
        WaitBarrier(); // Done Assigning

        // Do nothing
        WaitBarrier(); // Done Printing
    }
}

void
Grain() {
    while(NowYear < 2030){
        
        // calculate next cycle data
        float tempFactor = exp( -1 * SQR((NowTemp - MIDTEMP) / 10.));
        float precipFactor = exp( -1 * SQR((NowPrecip - MIDPRECIP) / 10.));

        float NextHeight = NowHeight;
        NextHeight += tempFactor * precipFactor * GRAIN_GROWS_PER_MONTH;
        NextHeight -= (float)NowNumDeer * ONE_DEER_EATS_PER_MONTH;

        if(NextHeight < 0.) NextHeight = 0.;
        WaitBarrier(); // Done Computing

        // replace current cycle data
        NowHeight = NextHeight;
        WaitBarrier(); // Done Assigning

        // Do nothing
        WaitBarrier(); // Done Printing
    }
}

void
Bears(){
    while(NowYear < 2030){

        // calculate next cycle data (if-less)
        int NextNumBears = NowNumBears;
        if(NowMonth < 4 || NowMonth > 9)
            goto hibernating;
        if(NowNumBears > NowNumDeer / 2)
            NextNumBears = NowNumBears - 1;
        else if(NowNumBears < NowNumDeer / 2)
            NextNumBears = NowNumBears + 1;

        if(NextNumBears < 0) NextNumBears = 0;
hibernating:;
        WaitBarrier(); // Done Computing

        // replace current cycle data
        NowNumBears = NextNumBears;
        WaitBarrier(); // Done Assigning

        // Do nothing
        WaitBarrier(); // Done Printing
    }
}
