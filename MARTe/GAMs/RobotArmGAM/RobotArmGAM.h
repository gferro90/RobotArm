/*
 * RobotArmGAM.h
 *
 *  Created on: 08/nov/2016
 *      Author: pc
 */

#ifndef ROBOTARMGAM_H_
#define ROBOTARMGAM_H_

#include "GAM.h"
#include "ConfigurationDatabase.h"
#include "stm32f4xx_hal.h"

/*---------------------------------------------------------------------------*/
/*                           Class declaration                               */
/*---------------------------------------------------------------------------*/

using namespace MARTe;

class RobotArmGAM: public GAM {
public:
    CLASS_REGISTER_DECLARATION()

    RobotArmGAM();
    virtual ~RobotArmGAM();

    virtual bool Initialise(StructuredDataI &data);

    virtual bool Setup();

    virtual bool Execute();


private:

    void ExecuteControlState(uint32 i, float32 dt);
    void ExecuteBasicState(uint32 i);
    void ExecuteHomeState(uint32 i);
    void ConvertToPwm(float32 u, uint32 i);

    uint32 *timer;
    uint32 *encoders;
    int8 *motorDirection;
    int8 *counterDirection;
    int32 *pwms;
    int32 *references;
    uint32 numberOfMotors;
    float32 *pid[3];
    float32 *maxControl;
    float32 *minControl;
    uint32 *maxPwm;
    uint32 *minPwm;
    uint8 *switchIdx;
    int32 *toUsb;
    int32 *fromUsb;
    int32 *int_e;
    int32 *endSwitch;
    uint32 *switches;
    uint32 *endSwitchCounter;
    uint32 *endSwitchBound;
    int32 *errorStore;
    uint32 *encoderStore;
    uint32 tic;
    int8 currentState;

};
#endif /* ROBOTARMGAM_H_ */
