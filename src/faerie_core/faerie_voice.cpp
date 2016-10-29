#include "faerie_voice.h"

const char* FaerieVoice::s_DefaultLanguage = "en-us";
int FaerieVoice::s_ObjectsAlive = 0;
FaerieVoice* FaerieVoice::s_LastUser = nullptr;

// TODO: Make a system that with an actual callback that lets you register listeners
int locNullCallback(short* _pWav, int _NumberOfSamples, espeak_EVENT* _pEvt)
{
	return 0;
}

FaerieVoice::FaerieVoice()
{
	if (FaerieVoice::s_ObjectsAlive <= 0)
	{
		espeak_Initialize(AUDIO_OUTPUT_PLAYBACK, 200, 0L, 0);
		espeak_SetSynthCallback(locNullCallback);
	}
	++FaerieVoice::s_ObjectsAlive;
}

FaerieVoice::~FaerieVoice()
{
	--FaerieVoice::s_ObjectsAlive;
	if (m_pVoice)
		delete m_pVoice;
	if (FaerieVoice::s_ObjectsAlive <= 0)
		espeak_Terminate();
}

void FaerieVoice::Init(int _speakRate, int _pitch, int _pitchVariance)
{
	m_SpeakRate = _speakRate;
	m_Pitch = _pitch;
	m_PitchVariance = _pitchVariance;
	// Set Default Voice
	InitDefaultVoice();
}

void FaerieVoice::SetVoice(espeak_VOICE* _pVoice)
{
	if (m_pVoice)
		delete m_pVoice; // hopefully nobody else uses that voice; maybe don't do that
	m_pVoice = _pVoice;
}

void FaerieVoice::Say(char* _pSentence, size_t _SentenceByteSize)
{
	InitInternal();
	
	espeak_Synth(_pSentence,
		_SentenceByteSize,
		0,
		espeak_POSITION_TYPE::POS_CHARACTER, 
		0, 
		espeakCHARS_8BIT | espeakENDPAUSE | espeakPHONEMES | espeakSSML, 
		nullptr, 
		nullptr);
	espeak_Synchronize(); // wait till message is spoken
}

void FaerieVoice::Say(const char* _pSentence, size_t _SentenceByteSize)
{
	InitInternal();
	
	espeak_Synth(_pSentence,
		_SentenceByteSize,
		0,
		espeak_POSITION_TYPE::POS_CHARACTER, 
		0, 
		espeakCHARS_8BIT | espeakENDPAUSE | espeakPHONEMES | espeakSSML, 
		nullptr, 
		nullptr);
	espeak_Synchronize(); // wait till message is spoken
}

void FaerieVoice::SayNoPhonemes(char* _pSentence, size_t _SentenceByteSize)
{
	InitInternal();
	
	espeak_Synth(_pSentence,
		_SentenceByteSize,
		0,
		espeak_POSITION_TYPE::POS_CHARACTER, 
		0, 
		espeakCHARS_8BIT | espeakENDPAUSE, 
		nullptr, 
		nullptr);
	espeak_Synchronize(); // wait till message is spoken
}

void FaerieVoice::InitDefaultVoice()
{
	m_pVoice = new espeak_VOICE();
	m_pVoice->languages = FaerieVoice::s_DefaultLanguage;
	m_pVoice->gender = FaerieVoice::s_DefaultGender;
	m_pVoice->age = FaerieVoice::s_DefaultAge;
}

void FaerieVoice::InitInternal()
{
	if (FaerieVoice::s_LastUser != this)
	{
		espeak_SetParameter(espeakRATE, m_SpeakRate, 0);
		espeak_SetParameter(espeakPITCH, m_Pitch, 0);
		espeak_SetVoiceByProperties(m_pVoice);
		FaerieVoice::s_LastUser = this;
	}
	// TODO: Randomize Pitch by _pitchVariance
}

