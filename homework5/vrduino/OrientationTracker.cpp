#include "OrientationTracker.h"
#include <vector>

OrientationTracker::OrientationTracker(double imuFilterAlphaIn,  bool simulateImuIn) :

  imu(),
  gyr{0,0,0},
  acc{0,0,0},
  gyrBias{0,0,0},
  gyrVariance{0,0,0},
  accBias{0,0,0},
  accVariance{0,0,0},
  previousTimeImu(0),
  imuFilterAlpha(imuFilterAlphaIn),
  deltaT(0.0),
  simulateImu(simulateImuIn),
  simulateImuCounter(0),
  flatlandRollGyr(0),
  flatlandRollAcc(0),
  flatlandRollComp(0),
  quaternionGyr{1,0,0,0},
  eulerAcc{0,0,0},
  quaternionComp{1,0,0,0}

  {

}

void OrientationTracker::initImu() {
  imu.init();
}


/**
 * TODO: see documentation in header file
 */
void OrientationTracker::measureImuBiasVariance() {

  //check if imu.read() returns true
  //then read imu.gyrX, imu.accX, ...

  //compute bias, variance.
  //update:
  //gyrBias[0], gyrBias[1], gyrBias[2]
  //gyrVariance[0], gyrBias[1], gyrBias[2]
  //accBias[0], accBias[1], accBias[2]
  //accVariance[0], accBias[1], accBias[2]

  int count = 1000;

  float gyr_bias[3] = {0.0, 0.0, 0.0};
  float acc_bias[3] = {0.0, 0.0, 0.0};

  std::vector<float> gyr_variance_x;
  std::vector<float> gyr_variance_y;
  std::vector<float> gyr_variance_z;

  std::vector<float> acc_variance_x;
  std::vector<float> acc_variance_y;
  std::vector<float> acc_variance_z;

  while (count > 0){
    if(imu.read()){
      gyr_bias[0] += imu.gyrX;
      gyr_bias[1] += imu.gyrY;
      gyr_bias[2] += imu.gyrZ;

      acc_bias[0] += imu.accX;
      acc_bias[1] += imu.accY;
      acc_bias[2] += imu.accZ;

      gyr_variance_x.push_back(sq(imu.gyrX));
      gyr_variance_y.push_back(sq(imu.gyrY));
      gyr_variance_z.push_back(sq(imu.gyrZ));

      acc_variance_x.push_back(sq(imu.accX));
      acc_variance_y.push_back(sq(imu.accY));
      acc_variance_z.push_back(sq(imu.accZ));

      count--;
    }
  }

  gyrBias[0] = gyr_bias[0]/1000;
  gyrBias[1] = gyr_bias[1]/1000;
  gyrBias[2] = gyr_bias[2]/1000;

  accBias[0] = acc_bias[0]/1000;
  accBias[1] = acc_bias[1]/1000;
  accBias[2] = acc_bias[2]/1000;

  float total_gyr_var_x = 0.0;
  float total_gyr_var_y = 0.0;
  float total_gyr_var_z = 0.0;

  float total_acc_var_x = 0.0;
  float total_acc_var_y = 0.0;
  float total_acc_var_z = 0.0;

  for (int i = 0; i < 1000; i++) {
    total_gyr_var_x = sq(gyr_variance_x[i] - gyrBias[0]);
    total_gyr_var_y = sq(gyr_variance_y[i] - gyrBias[1]);
    total_gyr_var_z = sq(gyr_variance_z[i] - gyrBias[2]);

    total_acc_var_x = sq(acc_variance_x[i] - accBias[0]);
    total_acc_var_y = sq(acc_variance_y[i] - accBias[1]);
    total_acc_var_z = sq(acc_variance_z[i] - accBias[2]);
  }

  gyrVariance[0] = total_gyr_var_x / 1000;
  gyrVariance[1] = total_gyr_var_y / 1000;
  gyrVariance[2] = total_gyr_var_z / 1000;

  accVariance[0] = total_acc_var_x / 1000;
  accVariance[1] = total_acc_var_y / 1000;
  accVariance[2] = total_acc_var_z / 1000;
}

void OrientationTracker::setImuBias(double bias[3]) {

  for (int i = 0; i < 3; i++) {
    gyrBias[i] = bias[i];
  }

}

void OrientationTracker::resetOrientation() {

  flatlandRollGyr = 0;
  flatlandRollAcc = 0;
  flatlandRollComp = 0;
  quaternionGyr = Quaternion();
  eulerAcc[0] = 0;
  eulerAcc[1] = 0;
  eulerAcc[2] = 0;
  quaternionComp = Quaternion();

}

bool OrientationTracker::processImu() {

  if (simulateImu) {

    //get imu values from simulation
    updateImuVariablesFromSimulation();

  } else {

    //get imu values from actual sensor
    if (!updateImuVariables()) {

      //imu data not available
      return false;

    }

  }

  //run orientation tracking algorithms
  updateOrientation();

  return true;

}

void OrientationTracker::updateImuVariablesFromSimulation() {

    deltaT = 0.002;
    //get simulated imu values from external file
    for (int i = 0; i < 3; i++) {
      gyr[i] = imuData[simulateImuCounter + i];
    }
    simulateImuCounter += 3;
    for (int i = 0; i < 3; i++) {
      acc[i] = imuData[simulateImuCounter + i];
    }
    simulateImuCounter += 3;
    simulateImuCounter = simulateImuCounter % nImuSamples;

    //simulate delay
    delay(1);

}

/**
 * TODO: see documentation in header file
 */
bool OrientationTracker::updateImuVariables() {

  //sample imu values
  if (!imu.read()) {
  // return false if there's no data
    return false;
  }

  //call micros() to get current time in microseconds
  //update:
  //previousTimeImu (in seconds)
  //deltaT (in seconds)

  //read imu.gyrX, imu.accX ...
  //update:
  //gyr[0], ...
  //acc[0], ...

  float cur_time = micros()/1000000.0;

  deltaT = cur_time - previousTimeImu;
  previousTimeImu = cur_time;

  // You also need to appropriately modify the update of gyr as instructed in (2.1.3).
  gyr[0] = imu.gyrX - gyrBias[0];
  gyr[1] = imu.gyrY - gyrBias[1];
  gyr[2] = imu.gyrZ - gyrBias[2];

  acc[0] = imu.accX;
  acc[1] = imu.accY;
  acc[2] = imu.accZ;

  return true;

}


/**
 * TODO: see documentation in header file
 */
void OrientationTracker::updateOrientation() {

  //call functions in OrientationMath.cpp.
  //use only class variables as arguments to functions.

  //update:
  //flatlandRollGyr
  flatlandRollGyr = computeFlatlandRollGyr(flatlandRollGyr, gyr, deltaT);
  //flatlandRollAcc
  flatlandRollAcc = computeFlatlandRollAcc(acc);
  //flatlandRollComp
  flatlandRollComp = computeFlatlandRollComp(flatlandRollComp, gyr, flatlandRollAcc, deltaT, imuFilterAlpha);
  //quaternionGyr
  updateQuaternionGyr(quaternionGyr, gyr, deltaT);
  //eulerAcc
  eulerAcc[0] = computeAccPitch(acc);
  eulerAcc[1] = computeAccRoll(acc); 
  //quaternionComp
  quaternionComp = updateQuaternionComp(quaternionComp, gyr, acc, deltaT, imuFilterAlpha);


}
