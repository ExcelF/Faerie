#include <iostream>
#include <stdio.h>
#include <vector>
#include <string>
#include <thread>
#include <chrono>
#include "wifi_connection_monitor.h"

using namespace std;

int locGetNumberFromMacAddress(string str, size_t startPos, size_t endPos)
{
	// substr uses starting pos and length as parameters
	string currentHex = str.substr(startPos, endPos-startPos);
	//std::cout << currentHex << ", " << stoi(currentHex, nullptr, 16) << "\n";
	return stoi(currentHex, nullptr, 16);
}

void WifiConnectionMonitor::GetMacAddressAsIntArray(string _macAddress, int* _out)
{
	size_t oldColonPos = 0;
	size_t colonPos = _macAddress.find(":", 0);
	// remember the part of the mac address we're at
	unsigned int currentHexIdx = 0;
	while(colonPos < _macAddress.length()-1)
	{			
		// extract a part of the mac address and put into current array
		_out[currentHexIdx] = locGetNumberFromMacAddress(_macAddress, oldColonPos, colonPos);
		// update colon positions
		oldColonPos = colonPos + 1;
		colonPos = _macAddress.find(":", oldColonPos);
		++currentHexIdx;
	}
	// do the final strech
	_out[currentHexIdx] = locGetNumberFromMacAddress(_macAddress, oldColonPos, _macAddress.length());
}

WifiConnectionMonitor::WifiConnectionMonitor()
{
	
}

bool WifiConnectionMonitor::OpenArpScanFileHandle()
{
	if(!(m_fileHandle = popen("sudo arp-scan -l -q -g", "r")))
		return false;
	
	return true;
}

int WifiConnectionMonitor::CloseArpScanFileHandle()
{
	return pclose(m_fileHandle);
}

bool WifiConnectionMonitor::GoToNextLineFromHandle()
{
	m_isCommandExhausted = fgets(m_consoleBuffer, sizeof(m_consoleBuffer), m_fileHandle) 
							&& !(m_consoleBuffer[0] == '\n');
	return m_isCommandExhausted;
}

MacAddressEntry* WifiConnectionMonitor::ExtractMacAddressFromCurrentLine()
{
	string buffAsString(m_consoleBuffer);
	// go to the start of the mac address
	size_t macAddressStart = buffAsString.find(":")-2;
	buffAsString = buffAsString.substr(macAddressStart, buffAsString.length() - macAddressStart);
	// start getting all the parts of the address
	size_t oldColonPos = 0;
	size_t colonPos = buffAsString.find(":", 0);
	// create a new array to hold the data and add to our vector
	MacAddressEntry* curMacAddress = new MacAddressEntry; //myVec[myVec.size()-1];
	// remember the part of the mac address we're at
	unsigned int currentHexIdx = 0;
	while(colonPos < buffAsString.length()-1)
	{			
		// extract a part of the mac address and put into current array
		curMacAddress->aMacAddress[currentHexIdx] = locGetNumberFromMacAddress(buffAsString, oldColonPos, colonPos);
		// update colon positions
		oldColonPos = colonPos + 1;
		colonPos = buffAsString.find(":", oldColonPos);
		++currentHexIdx;
	}
	// do the final strech
	curMacAddress->aMacAddress[currentHexIdx] = locGetNumberFromMacAddress(buffAsString, oldColonPos, buffAsString.length());
	
	return curMacAddress;
}

void WifiConnectionMonitor::UpdateNetworkMacAddresses()
{
	if(!OpenArpScanFileHandle())
		return;		
	
	// get rid of the first two lines of the output
	for(int i = 0; i < 2; ++i)
		GoToNextLineFromHandle();
	// get the mac addresses 	
	while(GoToNextLineFromHandle())
	{
		MacAddressEntry* curMacAddress;
		curMacAddress = ExtractMacAddressFromCurrentLine();
		
		// check whether this address is already in the vector
		bool newAddress = true;
		for(unsigned int i = 0; i < m_MacAddressVector.size(); ++i)
		{
			MacAddressEntry* candidateAddress = m_MacAddressVector[i];
			bool isSame = true;
			for(unsigned int k = 0; k < 6; ++k)
			{
				if(candidateAddress->aMacAddress[k] != curMacAddress->aMacAddress[k])
				{
					isSame = false;
					break;
				}
			}
			if(isSame)
			{
				newAddress = false;
				candidateAddress->HasBeat = true;
				break;
			}
		}
		
		// handle new addresses
		if(newAddress)
		{
			curMacAddress->HasBeat = true;
			m_MacAddressVector.push_back(curMacAddress);
		}
		else // or cleanup
		{
			delete curMacAddress;
		}		
					
	}
	CloseArpScanFileHandle();
}

void WifiConnectionMonitor::UpdateMacAddressVector()
{
	for(int i = m_MacAddressVector.size()-1; i >= 0; --i)
	{		
		MacAddressEntry* current = m_MacAddressVector[i];
		
		for(unsigned int k = 0; k < 6; ++k)
		{
			cout << current->aMacAddress[k] << " ";
		}
		cout << ": ";
		
		if(current->HasBeat)
		{
			++current->BeatsSinceStart;
			cout << "Adding a beat: " << current->BeatsSinceStart << "\n";
			if(current->BeatsSinceStart == WifiConnectionMonitor::s_beatsTillConnect)
			{
				cout << "New Address!" << "\n";
				// Create new connection event
				WifiConnectionMonitorEvent evt = WifiConnectionMonitorEvent();
				evt.m_pNewAddress = current;
				CallListeners(evt);
			}
			if(current->BeatsMissed > 0)
				current->BeatsMissed = 0;
		}
		else
		{
			++current->BeatsMissed;
			cout << "Beat missed: " << current->BeatsMissed << "\n";
			if(current->BeatsMissed >= WifiConnectionMonitor::s_beatsTillDisconnect)
			{
				m_MacAddressVector[i] = m_MacAddressVector[m_MacAddressVector.size()-1];
				m_MacAddressVector.pop_back();
				cout << "Address timed out! \n";
				delete current;
				current = nullptr;
			}
		}
		
		if(current)
			current->HasBeat = false;
	}
}

void WifiConnectionMonitor::Update()
{
	UpdateNetworkMacAddresses();
	UpdateMacAddressVector();
}

void WifiConnectionMonitor::CallListeners(WifiConnectionMonitorEvent& _rEvt)
{
	for(unsigned int i = 0; i < m_listenerList.size(); ++i)
	{
		WifiConnectionMonitor::WifiConnectionMonitorListener currentListener = m_listenerList[i];
		currentListener(_rEvt);
	}
}

void WifiConnectionMonitor::AddListener(WifiConnectionMonitor::WifiConnectionMonitorListener listener)
{
	m_listenerList.push_back(listener);
}

void WifiConnectionMonitor::RemoveListener(WifiConnectionMonitor::WifiConnectionMonitorListener listener)
{
	for(int i = m_listenerList.size()-1; i >= 0; --i)
	{
		if(m_listenerList[i] == listener)
		{
			m_listenerList[i] = m_listenerList[m_listenerList.size()-1];
			m_listenerList.pop_back();
		}
	}
}
