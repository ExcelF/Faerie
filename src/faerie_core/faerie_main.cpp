#include <iostream>
#include <fstream>
#include <stdio.h>
#include <thread>
#include <chrono>
#include <unistd.h>
#include "../faerie_core/faerie_ears.h"
#include "../faerie_core/faerie_voice.h"
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
	// ### INIT FAERIE EARS (SPEECH RECOGNITION)
	FaerieEars myEars = FaerieEars();
	myEars.Init();
	
	// ## INIT FAERIE VOICE (SPEECH SYNTHESIS)
	FaerieVoice myVoice = FaerieVoice();
	myVoice.Init();
	
	// Read known macAddresses
	vector<MacAddressWithMessage*> macAddressesWithMessage;
	macAddressesWithMessage = locReadKnownMacAddressFile();
	
	// Add listener function to new wifi connections
	WifiConnectionMonitor wifiMonitor = WifiConnectionMonitor();
	wifiMonitor.AddListener((WifiConnectionMonitor::WifiConnectionMonitorListener) &locMainListener);
	
	const char myString [] = "Detected a new guest in our network. Hai!";
	const char yesString [] = "Yes?";
	const char faerieOnlineString [] = "Fairy, On-line";
		
	bool quit = false;
	bool isListening = false;
	
	while (!quit)
	{

		if (myEars.GetRecognizerQueueSize() > 0)
		{
			std::string words = myEars.PopRecognizedString();
			
			if (!isListening)
			{
				if (std::string::npos != words.rfind("fairy"))
				{
					myVoice.Say(yesString, sizeof(yesString));
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
					myVoice.Say(macAddressesWithMessage[i]->m_Message.c_str(),
						macAddressesWithMessage[i]->m_MessageLength * sizeof(char));
					playWelcomeMessage = false; // switch off default message
				}
			}
		}
		// Default message
		if (playWelcomeMessage)
		{
			myVoice.Say(myString, sizeof(myString));
		}
		playWelcomeMessage = false;
	}
	return 0;
}
