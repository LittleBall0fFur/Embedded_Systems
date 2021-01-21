
#ifndef MOUSEMOVEMENT_CLASS
#define MOUSEMOVEMENT_CLASS

#include <Mouse.h>

#define INTERRUPT_PIN 0  // use pin 2 on Arduino Uno & most boards
volatile bool mpuInterrupt = false;     // indicates whether MPU interrupt pin has gone high
void dmpDataReady() {
    mpuInterrupt = true;
};

#include "I2Cdev.h"
#include "MPU6050_6Axis_MotionApps20.h"

#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
    #include "Wire.h"
#endif

MPU6050 mpu;

class MouseMovement{
  public:
    MouseMovement(){
      Mouse.begin();
      
      // join I2C bus (I2Cdev library doesn't do this automatically)
      #if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
          Wire.begin();
          Wire.setClock(400000); // 400kHz I2C clock. Comment this line if having compilation difficulties
      #elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
          Fastwire::setup(400, true);
      #endif

      // init last euler array
      for(int i = 0; i < 3; i++){
        last_euler[i] = 0;
      }
      
      // initialize device
      mpu.initialize();
      int devStatus = mpu.dmpInitialize();
  
      // supply your own gyro offsets here, scaled for min sensitivity
      mpu.setXGyroOffset(220);
      mpu.setYGyroOffset(76);
      mpu.setZGyroOffset(-85);
      mpu.setZAccelOffset(1788); // 1688 factory default for my test chip

      // make sure it worked (returns 0 if so)
      if (devStatus == 0) {
          // Calibration Time: generate offsets and calibrate our MPU6050
          mpu.CalibrateAccel(6);
          mpu.CalibrateGyro(6);
          // turn on the DMP, now that it's ready
          mpu.setDMPEnabled(true);
  
          // enable Arduino interrupt detection
          attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN), dmpDataReady, RISING);
          mpuIntStatus = mpu.getIntStatus();
  
          // set our DMP Ready flag so the main loop() function knows it's okay to use it
          dmpReady = true;
  
          // get expected DMP packet size for later comparison
          packetSize = mpu.dmpGetFIFOPacketSize();
      } else {
          // ERROR!
          // 1 = initial memory load failed
          // 2 = DMP configuration updates failed
          // (if it's going to break, usually the code will be 1)
      }
    };
    ~MouseMovement(){
      
    };

    void update(){
      
    // if programming failed, don't try to do anything
    if (!dmpReady) 
      return;

    // wait for MPU interrupt or extra packet(s) available
    if (mpuInterrupt && fifoCount < packetSize) {
      // try to get out of the infinite loop 
      fifoCount = mpu.getFIFOCount();
    }else if (!mpuInterrupt && fifoCount < packetSize){
      return;  
    }

    // reset interrupt flag and get INT_STATUS byte
    mpuInterrupt = false;
    mpuIntStatus = mpu.getIntStatus();

    // get current FIFO count
    fifoCount = mpu.getFIFOCount();
    if(fifoCount < packetSize){
          //Lets go back and wait for another interrupt. We shouldn't be here, we got an interrupt from another event
      // This is blocking so don't do it   while (fifoCount < packetSize) fifoCount = mpu.getFIFOCount();
    }
    // check for overflow (this should never happen unless our code is too inefficient)
    else if ((mpuIntStatus & (0x01 << MPU6050_INTERRUPT_FIFO_OFLOW_BIT)) || fifoCount >= 1024) {
      // reset so we can continue cleanly
      mpu.resetFIFO();
    } 
    else if (mpuIntStatus & (0x01 << MPU6050_INTERRUPT_DMP_INT_BIT)) {
      while(fifoCount >= packetSize){ // Lets catch up to NOW, someone is using the dreaded delay()!
        mpu.getFIFOBytes(fifoBuffer, packetSize);
        // track FIFO count here in case there is > 1 packet available
        // (this lets us immediately read more without waiting for an interrupt)
        fifoCount -= packetSize;
      }
        // display Euler angles in degrees
        mpu.dmpGetQuaternion(&q, fifoBuffer);
        mpu.dmpGetEuler(euler, &q);
        Serial.print("{");
        Serial.print("  \"x\" : ");
        Serial.print(euler[0] * 180/M_PI);
        Serial.print(", \"y\" : ");
        Serial.print(euler[1] * 180/M_PI);
        Serial.print(", \"z\" : ");
        Serial.print(euler[2] * 180/M_PI);

        float valX = (last_euler[0] - euler[0]) * 180/M_PI;
        float valZ = (last_euler[2] - euler[2]) * 180/M_PI;
        //Serial.print("{");
        Serial.print(" \"mx\" : ");
        Serial.print(valX);
        Serial.print(", \"my\" : ");
        Serial.print(valZ);
        Serial.println(" }");
        
        Mouse.move(valX * -20, valZ * -15, 0);
        for(int i = 0; i < 3; i++){
          last_euler[i] = euler[i];
        }
      }

    };

  private:
    Quaternion q;           // [w, x, y, z]         quaternion container
    float euler[3];         // [psi, theta, phi]    Euler angle container
    float last_euler[3];           // [yaw, pitch, roll]   yaw/pitch/roll containe
    
    // MPU control/status vars
    bool dmpReady = false;  // set true if DMP init was successful
    uint8_t mpuIntStatus;   // holds actual interrupt status byte from MPU
    uint8_t devStatus;      // return status after each device operation (0 = success, !0 = error)
    uint16_t packetSize;    // expected DMP packet size (default is 42 bytes)
    uint16_t fifoCount;     // count of all bytes currently in FIFO
    uint8_t fifoBuffer[64]; // FIFO storage buffer
};

#endif
