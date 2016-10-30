#pragma once

#include "faerie_ears.h"
#include "faerie_voice.h"
#include "faerie_module.h"
#include <vector>
#include <map>

using namespace std;

// Forward declarations
class FaerieSpeechEvent; 

struct FaerieModuleInfo
{
	std::string		m_ModuleName;
	size_t			m_ModuleHash;
	FaerieModule*	m_ModulePointer;
};


class Faerie
{
public:
	Faerie();
	~Faerie();
	
	FaerieEars& GetEars() { return m_FaerieEars; };
	FaerieVoice& GetVoice() { return m_FaerieVoice; };
	
	/// Update core functionality and modules
	void Update(); // maybe pass delta time?
	
	void Say(char* _pSentence, size_t _SentenceByteSize);
	void Say(const char* _pSentence, size_t _SentenceByteSize);
	
	/// Register new module under _ModuleKey.
	/// Returns moduleHash or -1 if there is already a module registered under _ModuleKey;
	/// RegisterModule does not actually stop you from registering the same module(pointer) twice.
	size_t RegisterModule(std::string _ModuleKey, FaerieModule* _pModule);
	
	FaerieModule* GetModuleByName(std::string _ModuleName);
	FaerieModule* GetModuleByHash(size_t _ModuleHash);
	FaerieModule* GetModuleByIndex(size_t _ModuleIndex);
private:
	FaerieEars m_FaerieEars;
	FaerieVoice m_FaerieVoice;
	
	bool m_IsListening;
	
	std::string* m_pCurrentMessageReceived;
	
	vector<FaerieModule*> m_Modules;
	map<size_t, FaerieModuleInfo> m_ModuleMap;
};
