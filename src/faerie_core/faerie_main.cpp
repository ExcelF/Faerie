#include <iostream>
#include <fstream>
#include <stdio.h>
#include <thread>
#include <chrono>
#include <unistd.h>
#include "../faerie_core/faerie_ears.h"
#include "../include/speak_lib.h"
#include "../wifi_connection_monitor/wifi_connection_monitor.h"


// stub
int synthCallback(short *wav, int numsamples, espeak_EVENT *events)
{
	return 0;
}

static bool playWelcomeMessage = false;
// remember last mac address to connect; TODO: maybe make a nice queue for this
static MacAddressEntry* lastConnection = nullptr;

void locMainListener(WifiConnectionMonitorEvent& rEvt)
{
	if (rEvt.m_pNewAddress)
	{
		playWelcomeMessage = true;
		lastConnection = rEvt.m_pNewAddress;
	}
}

vector<MacAddressWithMessage*> locReadKnownMacAddressFile()
{	
	// Open filehandle to message config file
	ifstream knownMacAddressesFile;
	knownMacAddressesFile.open("knownMacAddresses.txt");
	string line;
	// return value
	vector<MacAddressWithMessage*> addressVector;
	
	if (knownMacAddressesFile.is_open())
	{
		// read lines
		while (getline(knownMacAddressesFile, line))
		{
			size_t equalsLoc = line.find("=");
			string macAddress = line.substr(0, equalsLoc);
			string textMessage = line.substr(equalsLoc + 1, line.length() - 1);
			
			cout << macAddress << "-" << textMessage << "\n";
			// Create new Message Entry
			MacAddressWithMessage* pMacAddressWithMessage = new MacAddressWithMessage;
			WifiConnectionMonitor::GetMacAddressAsIntArray(macAddress, &(pMacAddressWithMessage->m_MacAddress[0]));
			pMacAddressWithMessage->m_Message = textMessage.c_str();
			pMacAddressWithMessage->m_MessageLength = textMessage.length();
			
			// add to local address vector; maybe move functionality into a class?
			addressVector.push_back(pMacAddressWithMessage);
		}
	}
	knownMacAddressesFile.close();
	return addressVector;
}

int main()
{
	// Read known macAddresses
	vector<MacAddressWithMessage*> macAddressesWithMessage;
	macAddressesWithMessage = locReadKnownMacAddressFile();
	
	// Add listener function to new wifi connections
	WifiConnectionMonitor wifiMonitor = WifiConnectionMonitor();
	wifiMonitor.AddListener((WifiConnectionMonitor::WifiConnectionMonitorListener) &locMainListener);
	
	// Speech synth init
	espeak_Initialize(AUDIO_OUTPUT_PLAYBACK, 200, 0L, 0);
	espeak_SetSynthCallback(synthCallback);
	espeak_SetParameter(espeakRATE, 175, 0);
	// Setup voice (female)
	espeak_VOICE myVoice = espeak_VOICE();
	//memset(&myVoice, 0, sizeof(espeak_VOICE));
	myVoice.languages = "en-us";
	myVoice.gender = 2; // 0=none, 1=male, 2=female
	myVoice.age = 17;
	espeak_SetVoiceByProperties(&myVoice);
	const char myString [] = "Detected a new guest in our network. Hi!";
	const char yesString [] = "Yes?";
	
	
	// ### INIT FAERIE EARS (SPEECH RECOGNITION)
	FaerieEars myEars = FaerieEars();
	myEars.Init();
	
	bool quit = false;
	bool isListening = false;
	
	while (!quit)
	{
		//std::this_thread::sleep_for(std::chrono::milliseconds(2000));
		//usleep(200);
		if (myEars.GetRecognizerQueueSize() > 0)
		{
			std::string words = myEars.PopRecognizedString();
			
			if (!isListening)
			{
				if (std::string::npos != words.rfind("fairy"))
				{
					espeak_Synth(yesString,
						sizeof(yesString),
						0,
						espeak_POSITION_TYPE::POS_CHARACTER, 
						0, 
						espeakCHARS_8BIT | espeakENDPAUSE, 
						nullptr, 
						nullptr);
					espeak_Synchronize(); // wait till message is spoken
					isListening = true;
				}
			}
			else
			{
				if (std::string::npos != words.rfind("quit"))
				{
					quit = true;
				}
				if (std::string::npos != words.rfind("nothing"))
				{
					isListening = false;
					
				}
			}
			std::cout << "You said: " << words << std::endl;
		}
		
		if (isListening)
		{
			continue;
		}
		
		cout << "****\n";	
		wifiMonitor.Update();
		
		if (playWelcomeMessage)
		{
			// Check if mac address is registered with a message
			for (unsigned int i = 0; i < macAddressesWithMessage.size(); ++i)
			{
				if (*macAddressesWithMessage[i] == *lastConnection)
				{
					espeak_Synth(macAddressesWithMessage[i]->m_Message.c_str(),
						macAddressesWithMessage[i]->m_MessageLength * sizeof(char),
						0,
						espeak_POSITION_TYPE::POS_CHARACTER, 
						0, 
						espeakCHARS_8BIT | espeakENDPAUSE, 
						nullptr, 
						nullptr);
					playWelcomeMessage = false; // switch off default message
				}
			}
		}
		// Default message
		if (playWelcomeMessage)
		{
			espeak_Synth(myString, 
				sizeof(myString), 
				0, 
				espeak_POSITION_TYPE::POS_CHARACTER, 
				0, 
				espeakCHARS_8BIT | espeakENDPAUSE, 
				nullptr, 
				nullptr);
		}
		playWelcomeMessage = false;
		espeak_Synchronize(); // wait till message is spoken
	}
	return 0;
}
