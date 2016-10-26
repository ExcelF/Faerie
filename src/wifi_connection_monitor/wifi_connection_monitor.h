#include <iostream>
#include <stdio.h>
#include <string>
#include <vector>
#include <cstring>
#include "../include/speak_lib.h"

using namespace std;

struct MacAddressEntry {
	int aMacAddress[6];
	int BeatsMissed;
	int BeatsSinceStart;
	
	bool HasBeat;
	
	MacAddressEntry()
	{
		Reset();
	}
	
	void Reset()
	{
		BeatsMissed = 0;
		BeatsSinceStart = 0;
		HasBeat = false;
	}
	//~MacAddressEntry()
	//{
	//	delete [] aMacAddress;
	//}
};

struct MacAddressWithMessage {
	int m_MacAddress[6];
	string m_Message;
	size_t m_MessageLength;
	
	MacAddressWithMessage()
	{
		memset(m_MacAddress, 0, 6*sizeof(int));
		m_MessageLength = 0;
	}
	
	MacAddressWithMessage(int* _macAddress, string _message)
	{
		for(unsigned int i=0; i<6; ++i)
			m_MacAddress[i] = _macAddress[i];
		
		m_Message = _message;
		m_MessageLength = _message.length();
	}
	
	bool operator==(MacAddressWithMessage _other)
	{
		bool isSame = true;
		for(unsigned int i=0; i<6; ++i)
		{
			if(m_MacAddress[i] != _other.m_MacAddress[i])
				isSame = false;
		}
		return isSame;
	}
	
	bool operator ==(MacAddressEntry _other)
	{
		bool isSame = true;
		for(unsigned int i=0; i<6; ++i)
		{
			//cout << i << " " << m_MacAddress[i] << " " << _other.aMacAddress[i] << "\n";
			if(m_MacAddress[i] != _other.aMacAddress[i])
				isSame = false;
		}
		return isSame;
	}
};

struct WifiConnectionMonitorEvent
{
	bool m_HasNewAddresses;
	MacAddressEntry* m_pNewAddress;
};

class WifiConnectionMonitor
{
public:
	typedef void (*WifiConnectionMonitorListener) (WifiConnectionMonitorEvent& rEvt);
	
private:
	
	static const int s_beatsTillDisconnect = 60 * 15; //3;
	static const int s_beatsTillConnect = 3;
	
	char m_consoleBuffer[512];
	FILE* m_fileHandle;
	bool m_isCommandExhausted;
	
	void UpdateNetworkMacAddresses();
	void UpdateMacAddressVector();

	int CloseArpScanFileHandle();
	bool OpenArpScanFileHandle();
	bool GoToNextLineFromHandle();
	MacAddressEntry* ExtractMacAddressFromCurrentLine();
	
	vector<WifiConnectionMonitorListener> m_listenerList;
	
	void CallListeners(WifiConnectionMonitorEvent& _rEvt);
public:
	
	WifiConnectionMonitor();

	vector<MacAddressEntry*> m_MacAddressVector;

	void Update();
	void AddListener(WifiConnectionMonitorListener _listener);
	void RemoveListener(WifiConnectionMonitorListener _listener);
	
	static void GetMacAddressAsIntArray(string _macAddress, int* _out);
};
