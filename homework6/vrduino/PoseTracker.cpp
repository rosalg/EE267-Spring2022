#include "PoseTracker.h"
#include <Wire.h>

PoseTracker::PoseTracker(double alphaImuFilterIn, int baseStationModeIn, bool simulateLighthouseIn) :

  OrientationTracker(alphaImuFilterIn, false),
  lighthouse(),
  simulateLighthouse(simulateLighthouseIn),
  simulateLighthouseCounter(0),
  position{0,0,-500},
  baseStationPitch(0),
  baseStationRoll(0),
  baseStationMode(baseStationModeIn),
  position2D{0,0,0,0,0,0,0,0},
  clockTicks{0,0,0,0,0,0,0,0},
  numPulseDetections{0,0,0,0,0,0,0,0},
  pulseWidth{0,0,0,0,0,0,0,0}

  {

}

int PoseTracker::processLighthouse() {

  if (simulateLighthouse) {
  //if in simulation mode, get data from external file
    for (int i = 0; i < 8; i++) {
      clockTicks[i] = clockTicksData[(simulateLighthouseCounter*8 + i) % nLighthouseSamples];
      numPulseDetections[i] = 0;
    }

    //base station pitch/roll values remain the same throughout the simulation
    if (simulateLighthouseCounter == 0) {
      baseStationPitch = baseStationPitchSim;
      baseStationRoll = baseStationRollSim;
    }

    //data wraps around after end of array is reached
    simulateLighthouseCounter = (simulateLighthouseCounter + 1) % nLighthouseSamples;

    //slight delay to simulate delay between sensor readings (not exactly 120 Hz)
    delay(1);

  } else {
    //check data is available
    if (!lighthouse.readTimings(baseStationMode, clockTicks, numPulseDetections, pulseWidth,
      baseStationPitch, baseStationRoll)) {
      return -2;
    }

    //check that all diodes have only one detection
    //the number of dectections could be more than one due to reflections.
    for (int i = 0; i < 8; i++) {
      if (numPulseDetections[i] != 1) {
        return -1;
      }
    }
  }

  return updatePose();

}

/**
 * TODO: see header file for documentation
 */
int PoseTracker::updatePose() {

  // call functions in PoseMath.cpp to get a new position
  // and orientation estimate.
  //
  // you will need to use the following class variables:
  // - clockTicks
  // - position2D
  // - positionRef
  // - position
  // - quaternionHm
  //
  // - position and quaternionHm should hold the your position
  // and orientation estimates at the end of this function
  //
  // return 0 if errors occur, return 1 if successful
  convertTicksTo2DPositions(clockTicks, position2D);

  double Aout[8][8];
  formA(position2D, positionRef, Aout);

  double hOut[8][8];
  bool inverted = solveForH(Aout, position2D, hOut);

  double r[3][3];
  getRtFromH(hOut, r, position);

  quaternionHm = getQuaternionFromRotationMatrix(r);

  return inverted ? 1 : 0;

}
