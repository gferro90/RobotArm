/*
 * RobotArmGAM.cpp
 *
 *  Created on: 08/nov/2016
 *      Author: pc
 */

#include "RobotArmGAM.h"
#include "AdvancedErrorManagement.h"
#include "HighResolutionTimer.h"

/*---------------------------------------------------------------------------*/
/*                           Static definitions                              */
/*---------------------------------------------------------------------------*/
extern uint32 sentPacketNumber;

static float32 AbsVal(float32 u) {
    return (u >= 0) ? (u) : (-u);
}

static int8 SignVal(float32 u) {
    return (u >= 0) ? (1) : (-1);
}

extern int8 SM_changeState;
extern int8 SM_nextState;

/*---------------------------------------------------------------------------*/
/*                           Method definitions                              */
/*---------------------------------------------------------------------------*/

RobotArmGAM::RobotArmGAM() {
    timer = NULL;
    encoders = NULL;
    pwms = NULL;
    dirs = NULL;
    references = NULL;
    toUsb = NULL;
    fromUsb = NULL;
    switches = NULL;
    for (uint32 i = 0u; i < 3u; i++) {
        pid[i] = NULL;
    }
    maxControl = NULL;
    minControl = NULL;
    maxPwm = NULL;
    minPwm = NULL;
    dirIdx = NULL;
    switchIdx = NULL;
    dirPinMask = 0u;
    tic = 0u;

    int_e = NULL;
    endSwitch = NULL;
    endSwitchCounter = NULL;
    endSwitchBound = NULL;
    errorStore = NULL;
    encoderStore = NULL;
    currentState = 0;

}

RobotArmGAM::~RobotArmGAM() {
    if (encoders != NULL) {
        delete[] encoders;
    }
    for (uint32 i = 0u; i < 3u; i++) {
        if (pid[i] != NULL) {
            delete[] pid[i];
        }
    }
    if (pwms != NULL) {
        delete[] pwms;
    }
    if (references != NULL) {
        delete[] references;
    }
    if (maxControl != NULL) {
        delete[] maxControl;
    }
    if (minControl != NULL) {
        delete[] minControl;
    }
    if (maxPwm != NULL) {
        delete[] maxPwm;
    }
    if (minPwm != NULL) {
        delete[] minPwm;
    }
    if (dirIdx != NULL) {
        delete[] dirIdx;
    }
    if (switchIdx != NULL) {
        delete[] switchIdx;
    }

    if (int_e != NULL) {
        delete[] int_e;
    }
    if (endSwitch != NULL) {
        delete[] endSwitch;
    }
    if (endSwitchCounter != NULL) {
        delete[] endSwitchCounter;
    }

    if (endSwitchBound != NULL) {
        delete[] endSwitchBound;
    }
    if (errorStore != NULL) {
        delete[] errorStore;
    }

    if (encoderStore != NULL) {
        delete[] errorStore;
    }
}

bool RobotArmGAM::Initialise(StructuredDataI &data) {
    bool ret = GAM::Initialise(data);
    if (ret) {
        if (!data.Read("NumberOfMotors", numberOfMotors)) {
            REPORT_ERROR(ErrorManagement::Warning, "NumberOfMotors undefined: using 6 by default");
            numberOfMotors = 6;
        }

        references = new int32[numberOfMotors];
        int_e = new int32[numberOfMotors];
        endSwitch = new int32[numberOfMotors];
        endSwitchCounter = new uint32[numberOfMotors];
        errorStore = new int32[numberOfMotors];
        encoderStore = new uint32[numberOfMotors];

        //todo controller parameters
        pid[0] = new float32[numberOfMotors];
        Vector < float32 > kpVec(pid[0], numberOfMotors);
        if (!data.Read("Kp", kpVec)) {
            REPORT_ERROR(ErrorManagement::Warning, "Kp undefined: using 0 by default");
            for (uint32 k = 0u; k < numberOfMotors; k++) {
                (pid[0])[k] = 0u;
            }
        }

        pid[1] = new float32[numberOfMotors];
        Vector < float32 > kiVec(pid[1], numberOfMotors);
        if (!data.Read("Ki", kiVec)) {
            REPORT_ERROR(ErrorManagement::Warning, "Ki undefined: using 0 by default");
            for (uint32 k = 0u; k < numberOfMotors; k++) {
                (pid[1])[k] = 0u;
            }
        }

        pid[2] = new float32[numberOfMotors];
        Vector < float32 > kdVec(pid[2], numberOfMotors);
        if (!data.Read("Kd", kdVec)) {
            REPORT_ERROR(ErrorManagement::Warning, "Kd undefined: using 0 by default");
            for (uint32 k = 0u; k < numberOfMotors; k++) {
                (pid[2])[k] = 0u;
            }
        }
        endSwitchBound = new uint32[numberOfMotors];
        Vector < uint32 > endSwitchBoundVec(endSwitchBound, numberOfMotors);
        if (!data.Read("EndSwitchBound", endSwitchBoundVec)) {
            REPORT_ERROR(ErrorManagement::Warning, "EndSwitchBound undefined: using 10 by default");
            for (uint32 k = 0u; k < numberOfMotors; k++) {
                (endSwitchBound)[k] = 10u;
            }
        }

        ret = data.Read("DirPinMask", dirPinMask);
        if (!ret) {
            REPORT_ERROR(ErrorManagement::InitialisationError, "DirPinMask undefined");
        }
        else {
            dirIdx = new uint8[numberOfMotors];
            uint32 cnt = 0u;
            for (uint32 i = 0u; i < 16 && cnt < numberOfMotors; i++) {
                if ((dirPinMask & (1 << i)) != 0) {
                    dirIdx[cnt] = i;
                    cnt++;
                }
            }
        }

        uint16 switchPinMask;
        if (!data.Read("SwitchPinMask", switchPinMask)) {
            REPORT_ERROR(ErrorManagement::Warning, "SwitchPinMask undefined: Home will stops when motors reach their ends");
        }
        else {
            switchIdx = new uint8[numberOfMotors];
            uint32 cnt = 0u;
            for (uint32 i = 0u; i < 16 && cnt < numberOfMotors; i++) {
                if ((switchPinMask & (1 << i)) != 0) {
                    switchIdx[cnt] = i;
                    cnt++;
                }
            }
        }

        if (ret) {
            maxPwm = new uint32[numberOfMotors];
            Vector < uint32 > maxPwmVec(maxPwm, numberOfMotors);
            ret = data.Read("MaxPwm", maxPwmVec);
            if (!ret) {
                REPORT_ERROR(ErrorManagement::InitialisationError, "MaxPwm undefined");
            }
        }
        if (ret) {
            minPwm = new uint32[numberOfMotors];
            Vector < uint32 > minPwmVec(minPwm, numberOfMotors);
            ret = data.Read("MinPwm", minPwmVec);
            if (!ret) {
                REPORT_ERROR(ErrorManagement::InitialisationError, "MinPwm undefined");
            }
        }
        if (ret) {
            maxControl = new float32[numberOfMotors];
            Vector < float32 > maxControlVec(maxControl, numberOfMotors);
            ret = data.Read("MaxControl", maxControlVec);
            if (!ret) {
                REPORT_ERROR(ErrorManagement::InitialisationError, "MaxControl undefined");
            }
        }
        if (ret) {
            minControl = new float32[numberOfMotors];
            Vector < float32 > minControlVec(minControl, numberOfMotors);
            ret = data.Read("MinControl", minControlVec);
            if (!ret) {
                REPORT_ERROR(ErrorManagement::InitialisationError, "MinControl undefined");
            }
        }
    }

    return ret;
}

void RobotArmGAM::Setup() {
    //assign here the pointer to signals
    timer = (uint32*) GetInputSignalsMemory();
    fromUsb = (int32*) (timer + 1);
    encoders = (uint32*) (fromUsb + 2u);
    switches = encoders + numberOfMotors;
    dirs = (uint32*) GetOutputSignalsMemory();
    pwms = dirs + 1u;
    toUsb = (int32*) (pwms + numberOfMotors);
    for (uint32 i = 0u; i < numberOfMotors; i++) {
        references[i] = 0u;
        int_e[i] = 0u;
        endSwitch[i] = 0;
        endSwitchCounter[i] = 0u;
        errorStore[i] = 0;
        encoderStore[i] = 0;
    }
}

void RobotArmGAM::ConvertToPwm(float32 u,
                               uint32 i) {
    int32 step = encoders[i] - encoderStore[i];
    int32 error = (int32)(references[i] - encoders[i]);

    // giving voltage but the motor is hold
    if ((step == 0u) && (AbsVal(u) > minControl[i])) {
        endSwitchCounter[i]++;
        //blocked for an amount of cycles
        if (endSwitchCounter[i] > endSwitchBound[i]) {
            endSwitchCounter[i] = endSwitchBound[i];

            if ((u * endSwitch[i] >= 0) && (error == errorStore[i])) {
                endSwitch[i] = (u > 0) ? (1) : (-1);
                u = 0.;
            }
            // moving in the other direction... set as unblocked
            else {
                endSwitch[i] = 0;
                endSwitchCounter[i] = 0;
            }
        }
    }
    else {
        endSwitch[i] = 0;
        endSwitchCounter[i] = 0;
    }
    //map the control on pwm
    float32 uRange = maxControl[i]-minControl[i];
    uint32 pwmRange = maxPwm[i] - minPwm[i];
    int32 pwms_temp = (int32)(SignVal(u) * minPwm[i] + ((u-SignVal(u)*minControl[i]) / uRange) * pwmRange);

    // REPORT_ERROR_PARAMETERS(ErrorManagement::Warning, "Here 2 %d",pwms_temp);
    if (pwms_temp < 0) {
        pwms[i] = maxPwm[i] + pwms_temp; //invert the wave
        *dirs |= 1 << dirIdx[i]; //set negative direction
    }
    else {
        pwms[i] = pwms_temp;
    }
    //REPORT_ERROR_PARAMETERS(ErrorManagement::Warning, "Here 3 %d",pwms[i]);
    encoderStore[i] = encoders[i];
    errorStore[i] = error;

}

void RobotArmGAM::ExecuteHomeState(uint32 i) {
    //move until switch pin becomes high
    float32 u = 0.;
    //normally is equal to 1
    if ((((*switches) >> switchIdx[i]) & (0x1u))) {
        u = maxControl[i];
    }
    ConvertToPwm(u, i);
}

void RobotArmGAM::ExecuteBasicState(uint32 i) {
    float32 u = 0.;
    if (references[i] > 0) {
        u = maxControl[i];
    }
    else if (references[i] < 0) {
        u = -maxControl[i];
    }
    //REPORT_ERROR_PARAMETERS(ErrorManagement::Warning, "Here 1 %f", u);
    ConvertToPwm(u, i);

}

void RobotArmGAM::ExecuteControlState(uint32 i,
                                      float32 dt) {
    int32 error = (int32)(references[i] - encoders[i]);

    int_e[i] += error * dt;
    int32 d_e = (error - errorStore[i]);

    //proportional action
    float32 u = (pid[0])[i] * error;
    //integral action
    u += (pid[1])[i] * int_e[i];
    //derivative action
    u += (pid[2])[i] * (d_e / dt);
    //saturation
    if (AbsVal(u) > maxControl[i]) {
        u = SignVal(u) * maxControl[i];
    }
    //REPORT_ERROR_PARAMETERS(ErrorManagement::Warning, "u: %f, mc: %f", u, minControl[i]);
    /*if (AbsVal(u) < minControl[i]) {
        u = 0.;
    }*/

    ConvertToPwm(u, i);

}

bool RobotArmGAM::Execute() {
    *dirs &= ~((uint32) dirPinMask); //reset all pins
    *dirs |= ((uint32) dirPinMask) << 16; //reset all pins

    //REPORT_ERROR_PARAMETERS(ErrorManagement::Warning, "switch=%#0b", *switches);
    float32 dt = (float32) HighResolutionTimer::TicksToTime(HighResolutionTimer::Counter(), (uint64) tic);
    if (fromUsb[0] >= 0) {
        references[fromUsb[0]] = fromUsb[1];
        // read from ADC
        toUsb[0] = sentPacketNumber;
        sentPacketNumber++;
        toUsb[1] = *timer;
        for (uint32 i = 0u; i < numberOfMotors; i++) {
            if (tic > 0) {
                //REPORT_ERROR(ErrorManagement::Warning, "Here 0");
                if (currentState == 0u) {
                    ExecuteBasicState(i);
                }
                else if (currentState == 1u) {
                    ExecuteHomeState(i);
                }
                else if (currentState == 2u) {
                    ExecuteControlState(i, dt);
                }
            }
            toUsb[i + 2] = (uint32)(dt * 1e6);
            toUsb[i + 3] = references[i];
            toUsb[i + 4] = (int32) encoders[i];
            toUsb[i + 5] = (int32) pwms[i];
        }
    }
    //change state!
    else {
        currentState = fromUsb[1];
        if (currentState == -1) {
            SM_changeState = 1;
            SM_nextState = -1;
        }
        else {
            for (uint32 i = 0u; i < numberOfMotors; i++) {
                references[i] = 0u;
                int_e[i] = 0u;
                endSwitch[i] = 0;
                endSwitchCounter[i] = 0u;
                errorStore[i] = 0;
                encoderStore[i] = 0;
                pwms[i] = 0;
                toUsb[i + 2] = (uint32)(dt * 1e6);
                toUsb[i + 3] = references[i];
                toUsb[i + 4] = (int32) encoders[i];
                toUsb[i + 5] = (int32) pwms[i];
                encoders[i] = (uint32) - 1;

            }
        }

    }

    //initialise values
    tic = (uint32) HighResolutionTimer::Counter();
    //write on pwm (done by output broker)
    return true;
}

CLASS_REGISTER(RobotArmGAM, "1.0")
