#include "OrientationMath.h"

/** TODO: see documentation in header file */
double computeAccPitch(double acc[3]) {
  double sign;
  sign = (acc[1] > 0) ? 1 : (acc[1] < 0) ? -1 : 1;

  double second =  sign * sqrt(sq(acc[0]) + sq(acc[1]));
  return -atan2(acc[2], second) * 180 / PI;

}

/** TODO: see documentation in header file */
double computeAccRoll(double acc[3]) {

  return -atan2(-acc[0], acc[1]) * 180/PI;

}

/** TODO: see documentation in header file */
double computeFlatlandRollGyr(double flatlandRollGyrPrev, double gyr[3], double deltaT) {

return flatlandRollGyrPrev + deltaT * gyr[2];

}

/** TODO: see documentation in header file */
double computeFlatlandRollAcc(double acc[3]) {

  return atan2(acc[0], acc[1]) * 180/PI;

}

/** TODO: see documentation in header file */
double computeFlatlandRollComp(double flatlandRollCompPrev, double gyr[3], double flatlandRollAcc, double deltaT, double alpha) {

  double prev_gyr = computeFlatlandRollGyr(flatlandRollCompPrev, gyr, deltaT);
  return alpha * prev_gyr + (1 - alpha) * flatlandRollAcc;

}


/** TODO: see documentation in header file */
void updateQuaternionGyr(Quaternion& q, double gyr[3], double deltaT) {
  // q is the previous quaternion estimate
  // update it to be the new quaternion estimate

  double magnitude = sqrt(sq(gyr[0]) + sq(gyr[1]) + sq(gyr[2]));

  if (magnitude <= 1e-8){
    return;
  }

  Quaternion q_delta = q.clone().setFromAngleAxis(deltaT * magnitude, gyr[0] / magnitude, gyr[1] / magnitude, gyr[2] / magnitude);
  q = Quaternion().multiply(q, q_delta).normalize();
}


/** TODO: see documentation in header file */
void updateQuaternionComp(Quaternion& q, double gyr[3], double acc[3], double deltaT, double alpha) {
  // q is the previous quaternion estimate
  // update it to be the new quaternion estimate
  Quaternion q_next = q.clone();

  // i and ii
  updateQuaternionGyr(q_next, gyr, deltaT);

  // iii
  Quaternion q_acc(0, acc[0], acc[1], acc[2]);

  // iv
  Quaternion q_inertial_frame = q_acc.rotate(q_next).normalize();

  // v
  double phi = acos(q_inertial_frame.q[2]) * 180 / PI;

  // vi
  double n_mag = sqrt(sq(q_inertial_frame.q[1] + sq(q_inertial_frame.q[3])));
  Quaternion tilt = Quaternion().setFromAngleAxis((1 - alpha) * phi, -q_inertial_frame.q[3] / n_mag, 0.0, q_inertial_frame.q[1] / n_mag).normalize();

  // vii
  q = q.multiply(tilt, q_next).normalize();

}
