#include "faerie.h"
#include "faerie_speech_event.h"
#include <functional> //hash

Faerie::Faerie()
	: m_IsListening(false)
{
	// ### INIT FAERIE EARS (SPEECH RECOGNITION)
	m_FaerieEars = FaerieEars();
	m_FaerieEars.Init();
	
	// ### INIT FAERIE VOICE (SPEECH SYNTHESIS)
	m_FaerieVoice = FaerieVoice();
	m_FaerieVoice.Init();
}

Faerie::~Faerie()
{
}

void Faerie::Update()
{
	if (m_FaerieEars.GetRecognizerQueueSize() > 0)
	{
		std::string message = m_FaerieEars.PopRecognizedString();
		m_FaerieEars.ClearRecognizerQueue();
		
		if (m_IsListening)
		{		
			FaerieSpeechEvent speechEvent;
			speechEvent.m_RecognizedString = message;
			speechEvent.m_IsHandled = false;
		}
		else
		{
			//m_FaerieVoice.Say();
			m_IsListening = true;
		}
	}

	for (int i = 0; i < m_Modules.size(); ++i)
	{
		m_Modules[i]->Update();
	}
}

void Faerie::Say(char* _pSentence, size_t _SentenceByteSize)
{
	m_FaerieVoice.Say(_pSentence, _SentenceByteSize);
}


void Faerie::Say(const char* _pSentence, size_t _SentenceByteSize)
{
	m_FaerieVoice.Say(_pSentence, _SentenceByteSize);
}

size_t Faerie::RegisterModule(std::string _ModuleKey, FaerieModule* _pModule)
{
	size_t moduleHash = hash<std::string> {}(_ModuleKey);
	
	if (m_ModuleMap.end() == m_ModuleMap.find(moduleHash))
	{
		m_Modules.push_back(_pModule);
		
		FaerieModuleInfo moduleInfo;
		moduleInfo.m_ModuleHash = moduleHash;
		moduleInfo.m_ModuleName = _ModuleKey;
		moduleInfo.m_ModulePointer = _pModule;
		m_ModuleMap[moduleHash] = moduleInfo;		
	}
}

FaerieModule* Faerie::GetModuleByName(std::string _ModuleName)
{
	size_t searchHash = hash<std::string> {}(_ModuleName);
	
	if (m_ModuleMap.end() != m_ModuleMap.find(searchHash))
		return m_ModuleMap[searchHash].m_ModulePointer;
	else
		return nullptr;	
}

FaerieModule* Faerie::GetModuleByHash(size_t _ModuleHash)
{
	if (m_ModuleMap.end() != m_ModuleMap.find(_ModuleHash))
		return m_ModuleMap[_ModuleHash].m_ModulePointer;
	else
		return nullptr;
}

FaerieModule* Faerie::GetModuleByIndex(size_t _ModuleIndex)
{
	if (m_Modules.size() < _ModuleIndex)
		return m_Modules[_ModuleIndex];
	else
		return nullptr;
}