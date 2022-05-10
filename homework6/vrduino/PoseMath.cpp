#include "PoseMath.h"
#include <math.h>

/**
 * TODO: see header file for documentation
 */
void convertTicksTo2DPositions(uint32_t clockTicks[8], double pos2D[8])
{
  //use variable CLOCKS_PER_SECOND defined in PoseMath.h
  //for number of clock ticks a second
  for (int i = 0; i < 8; i += 2) {
    double delta_h = (double)clockTicks[i] / (double)CLOCKS_PER_SECOND;
    double omega_h = -delta_h * 60.0 * 360.0 + 90.0;
    pos2D[i] = tan(2 * PI * omega_h / 360.0);

    double delta_v = (double)clockTicks[i + 1] / (double)CLOCKS_PER_SECOND;
    double omega_v = omega_v * 60.0 * 360.0 - 90.0;
    pos2D[i + 1] = tan(2 * PI * omega_v / 360.0);
  }

}

/**
 * TODO: see header file for documentation
 */
void formA(double pos2D[8], double posRef[8], double Aout[8][8]) {
  for (int i = 0; i < 8; i += 2) {

    // 1
    Aout[i][0] = posRef[i];
    Aout[i][1] = posRef[i + 1];
    Aout[i][2] = 1;
    Aout[i][3] = 0;
    Aout[i][4] = 0;
    Aout[i][5] = 0;
    Aout[i][6] = -posRef[i] * pos2D[i];
    Aout[i][7] = -posRef[i + 1] * pos2D[i];

    // 2
    Aout[i + 1][0] = 0;
    Aout[i + 1][1] = 0;
    Aout[i + 1][2] = 0;
    Aout[i + 1][3] = posRef[i];
    Aout[i + 1][4] = posRef[i + 1];
    Aout[i + 1][5] = 1;
    Aout[i + 1][6] = -posRef[1] * pos2D[i + 1];
    Aout[i + 1][7] = -posRef[i + 1] * pos2D[i + 1];

  }


}


/**
 * TODO: see header file for documentation
 */
bool solveForH(double A[8][8], double b[8], double hOut[8]) {
  //use Matrix Math library for matrix operations
  //example:
  int inverted;
  if ((inverted = Matrix.Invert((double*)A, 8)) == 0)
    return false;

  Matrix.Multiply((double*)A, b, 8, 8, 1, hOut);

  return true;

}

double distance(double x, double y, double z) {
  return sqrt(x * x + y * y + z * z);
}

double distance_4(double x, double y, double z, double w) {
  return sqrt(x * x + y * y + z * z + w * w);
}

double r_2_calc(double base, double r_const, double scale) {
  return base - r_const * scale;
}


/**
 * TODO: see header file for documentation
 */
void getRtFromH(double h[8], double ROut[3][3], double pos3DOut[3]) {
  double scale_factor = 2 / (distance(h[0], h[3], h[6]) + distance(h[1], h[4], h[7]));
  Matrix.Scale((double*)h, 8, 1, scale_factor);
  pos3DOut[0] = h[2];
  pos3DOut[1] = h[5];
  pos3DOut[2] = -scale_factor;

  // Comp R1 using estimations
  double r_x1_dist = distance(h[0], h[3], h[6]);
  ROut[0][0] = h[0] / r_x1_dist;
  ROut[1][0] = h[3] / r_x1_dist;
  ROut[2][0] = h[6] / r_x1_dist;

  // R2: Raw est., then normalizing
  double r_const = ROut[0][0] * h[1] + ROut[1][0] * h[4] + ROut[2][0] * h[7];
  double r12_est = r_2_calc(h[1], r_const, ROut[0][0]);
  double r22_est = r_2_calc(h[4], r_const, ROut[1][0]);
  double r32_est = r_2_calc(-h[7], r_const, ROut[2][0]);

  double norm_est_dist = distance(r12_est, r22_est, r32_est);
  ROut[0][1] = r12_est / norm_est_dist;
  ROut[1][1] = r22_est / norm_est_dist;
  ROut[2][1] = r32_est / norm_est_dist;

  // R3
  ROut[0][2] = ROut[1][0] * ROut[2][1] - ROut[2][0] * ROut[1][1];
  ROut[1][2] = ROut[2][0] * ROut[0][1] - ROut[0][0] * ROut[2][1];
  ROut[2][2] = ROut[0][0] * ROut[1][1] - ROut[1][0] * ROut[0][1];

}



/**
 * TODO: see header file for documentation
 */
Quaternion getQuaternionFromRotationMatrix(double R[3][3]) {
  double w_est = (sqrt(1 + R[0][0] + R[1][1] + R[2][2])) / 2;
  double x_est = (R[2][1] - R[1][2]) / (4 * w_est);
  double y_est = (R[0][2] - R[2][0]) / (4 * w_est);
  double z_est = (R[1][0] - R[0][1]) / (4 * w_est);
  Quaternion out = Quaternion(w_est, x_est, y_est, z_est);

  out.normalize();

  return out;

}
