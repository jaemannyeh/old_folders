#include <stdio.h>

typedef struct {
    double q; //process noise covariance
    double r; //measurement noise covariance
    double x; //value
    double p; //estimation error covariance
    double k; //kalman gain
} kalman_state;

kalman_state kalman_init(double q, double r, double p, double intial_value) {
  kalman_state result;
  result.q = q;
  result.r = r;
  result.p = p;
  result.x = intial_value;
  return result;
}

void kalman_update(kalman_state* state, double measurement) {
  //time update //prediction
  //omit x = x
  state->p = state->p + state->q;

  //measurement update //correction
  state->k = state->p / (state->p + state->r);
  state->x = state->x + state->k * (measurement - state->x);
  state->p = (1 - state->k) * state->p;
  printf("q %f r %f m %f k %f x %f p %f",state->q,state->r,measurement,state->k,state->x,state->p);
  printf("\n");
}

int main() {
    kalman_state ks = kalman_init(0.1, 0.1, 1.1, 0.1);
    kalman_update(&ks,0.39);
    kalman_update(&ks,0.50);
    kalman_update(&ks,0.48);
    kalman_update(&ks,0.29);
    kalman_update(&ks,0.25);
    kalman_update(&ks,0.32);
    kalman_update(&ks,0.34);
    kalman_update(&ks,0.48);
    kalman_update(&ks,0.41);
    kalman_update(&ks,0.45);
    return 0;
}

// standard deviation
//    double q; //process noise covariance //constant
//    double r; //measurement noise covariance //constant
// k=0 means that I cannot believe the measurement.
// k=1 means that I totally believe the measurement.
//
// http://bilgin.esme.org/BitsAndBytes/KalmanFilterforDummies
// If you succeeded to fit your model into Kalman Filter,
// then the next step is to determine the necessary parameters and your initial values.
// The most remaining painful thing is to determine R and Q.
// R is rather simple to find out, because, in general, we're quite sure about the noise in the environment.
// But finding out Q is not so obvious. And at this stage, I can't give you a specific method
