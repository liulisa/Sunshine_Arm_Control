// Copyright (C) 2013-2014 Thalmic Labs Inc.
// Distributed under the Myo SDK license agreement. See LICENSE.txt for details.

#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>
#include <iomanip>
#include <stdexcept>
#include <string>
#include <algorithm>
#include <array>
#include <sstream>
#include <Windows.h>
#include <stdafx.h>

#include <myo/myo.hpp>
#include <Serial.h>
//#include <MyoController.h

using namespace std;

#using <System.dll>
using namespace System;
using namespace System::IO::Ports;
#define RX_BUFFSIZE 20

void printUsage(_TCHAR progName[]);

// Classes that inherit from myo::DeviceListener can be used to receive events from Myo devices. DeviceListener
// provides several virtual functions for handling different kinds of events. If you do not override an event, the
// default behavior is to do nothing.
class DataCollector : public myo::DeviceListener {
public:
    DataCollector()
    : onArm(false), isUnlocked(true), currentPose()
    {
    }

    // onUnpair() is called whenever the Myo is disconnected from Myo Connect by the user.
    void onUnpair(myo::Myo* myo, uint64_t timestamp)
    {
        // We've lost a Myo.
        // Let's clean up some leftover state.
        onArm = false;
        isUnlocked = false;
    }
	
	// onPose() is called whenever the Myo detects that the person wearing it has changed their pose, for example,
    // making a fist, or not making a fist anymore.
    void onPose(myo::Myo* myo, uint64_t timestamp, myo::Pose pose)
    {
        currentPose = pose;

        if (pose != myo::Pose::unknown && pose != myo::Pose::rest) {
            // Tell the Myo to stay unlocked until told otherwise. We do that here so you can hold the poses without the
            // Myo becoming locked.
            myo->unlock(myo::Myo::unlockHold);

            // Notify the Myo that the pose has resulted in an action, in this case changing
            // the text on the screen. The Myo will vibrate.
            myo->notifyUserAction();
        } else {
            // Tell the Myo to stay unlocked only for a short period. This allows the Myo to stay unlocked while poses
            // are being performed, but lock after inactivity.
            myo->unlock(myo::Myo::unlockTimed);
        }
    }

	// onArmUnsync() is called whenever Myo has detected that it was moved from a stable position on a person's arm after
    // it recognized the arm. Typically this happens when someone takes Myo off of their arm, but it can also happen
    // when Myo is moved around on the arm.
    void onArmUnsync(myo::Myo* myo, uint64_t timestamp)
    {
        onArm = false;
    }

	// onLock() is called whenever Myo has become locked. No pose events will be sent until the Myo is unlocked again.
    void onLock(myo::Myo* myo, uint64_t timestamp)
    {
        isUnlocked = false;
    }

	// onUnlock() is called whenever Myo has become unlocked, and will start delivering pose events.
    void onUnlock(myo::Myo* myo, uint64_t timestamp)
    {
        isUnlocked = true;
    }

    // There are other virtual functions in DeviceListener that we could override here, like onAccelerometerData().
    // For this example, the functions overridden above are sufficient.

    // We define this function to print the current values that were updated by the on...() functions above.
    
    // This is set by onUnlocked() and onLocked() above.
    bool isUnlocked;

	// This is set by onArmUnsync() above.
	bool onArm;
    // This is set onPose() above.
    myo::Pose currentPose;
};

class StateMachine {
////////////////////////////////
//Experimental code: running through the state machine route
public:
	StateMachine()
	: state(1), pose("")
	{
	}

	int state;
	std::string pose;

	void update(DataCollector* collector)
	{
		pose = collector->currentPose.toString();
		if (state == 1){
				if (pose == "fist"){
					state = 2;
				}
				else if (pose == "fingersSpread"){
					state = 3;
				}
				else if (pose == "waveIn" || pose == "waveOut"){
					state = 4;
				}
		}
		else if (state == 2){
			if (pose == "fist"){
				state = 5;
			}
			else if (pose == "fingersSpread"){
				state = 6;
			}
			else if (pose == "waveOut"){
				state = 7;
			}
			else if (pose == "waveIn"){
				state = 8;
			}
		}
		else if (state == 3){
			if (pose == "fist"){
				state = 9;
			}
			else if (pose == "fingersSpread"){
				state = 10;
			}
			else if (pose == "waveIn"){
				state = 11;
			}
			else if (pose == "waveOut"){
				state = 12;
			}
		}
		else if (state == 4){
			if (pose == "fingersSpread"){
				state = 7;
			}
			else if (pose == "fist"){
				state = 8;
			}
			else if (pose == "waveIn"){
				state = 13;
				}
			else if (pose == "waveOut"){
				state = 14;
			}
		}
		else if (state == 5 || state == 6 || state == 7 || state == 8 || state == 9 || state == 10 || state == 11 || state == 12 || state == 13 || state == 14){
			if (pose == "rest"){
				state = 1;
			}
		}
	moveRobot();
	//std::cout<<state<<'\n'<<pose<<'\n';
	}

	void moveRobot(){
		if (state == 1){std::cout<<"Waiting for commands\n";}
		else if (state == 2 || state == 3 || state == 4){std::cout<<"Preparing for movement...\n";}
		else if (state == 5){std::cout<<"Gripper closing\n";}
		else if (state == 6){std::cout<<"Releasing gripper\n";}
		else if (state == 7){std::cout<<"Wrist up\n";}
		else if (state == 8){std::cout<<"Wrist down\n";}
		else if (state == 9){std::cout<<"Elbow up\n";}
		else if (state == 10){std::cout<<"Elbow down\n";}
		else if (state == 11){std::cout<<"Shoulder down\n";}
		else if (state == 12){std::cout<<"Shoulder up\n";}
		else if (state == 13){std::cout<<"Base left\n";}
		else if (state == 14){std::cout<<"Base right\n";}
	}
};

int main(int argc, char** argv)
{
    // We catch any exceptions that might occur below -- see the catch statement for more details.
    try {
		// First, we create a Hub with our application identifier. Be sure not to use the com.example namespace when
		// publishing your application. The Hub provides access to one or more Myos.
		myo::Hub hub("com.CSNE.SunshineArmy");

		std::cout << "Attempting to find a Myo...\n";

		// Next, we attempt to find a Myo to use. If a Myo is already paired in Myo Connect, this will return that Myo
		// immediately.
		// waitForMyo() takes a timeout value in milliseconds. In this case we will try to find a Myo for 10 seconds, and
		// if that fails, the function will return a null pointer.
		myo::Myo* myo = hub.waitForMyo(10000);

		// If waitForMyo() returned a null pointer, we failed to find a Myo, so exit with an error message.
		if (!myo) {
			throw std::runtime_error("Unable to find a Myo!");
		}

		// We've found a Myo.
		std::cout << "Connected to a Myo armband!\n";

		// Next we enable EMG streaming on the found Myo.
		myo->setStreamEmg(myo::Myo::streamEmgEnabled);

		// Next we construct an instance of our DeviceListener, so that we can register it with the Hub.
		DataCollector collector;

		// Hub::addListener() takes the address of any object whose class inherits from DeviceListener, and will cause
		// Hub::run() to send events to all registered device listeners.
		hub.addListener(&collector);
	
		// Then we construct a sate machine to listen to the Myo and make decisions based on what it is doing
		String^ portname = "COM5";
		int BAUD_RATE = 115200; //typically 9600 or 115200
		SerialPort port(portname, BAUD_RATE);
		port.Open();
		int timeSinceUpdate = 0;
		StateMachine machine;

		// Finally we enter our main loop.
		// In each iteration of our main loop, we run the Myo event loop for a set number of milliseconds.
		// In this case, we wish to update our display 50 times a second, so we run for 1000/20 milliseconds.
		int runtime = 1000/20;
		bool hasRest = false;
		while (1) {
			hub.run(runtime);
			timeSinceUpdate += runtime;
			// After processing events, we call the update() member function we defined above to determine
			// where the state machine should move and print the state and gesture to console.
			if (machine.state == 2 || machine.state == 3 || machine.state == 4){
				if (collector.currentPose.toString() == "rest"){
					hasRest = true;
				}
				if (hasRest){
					machine.update(&collector);
					hasRest = (collector.currentPose.toString() == "rest");
				}
			}
			else {
				machine.update(&collector);
			}
			// cout<<machine.state<<"\n";
			if (timeSinceUpdate >= 1000){ //only send once/second max
				port.Write(System::Convert::ToString(machine.state));
				timeSinceUpdate -= 1000;
			}
			//Console::Write(port.ReadLine());
		}
		port.Close();
    // If a standard exception occurred, we print out its message and exit.
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        std::cerr << "Press enter to continue.";
        std::cin.ignore();
        return 1;
    }
};