#include "../include/speak_lib.h"


// TODO: Add something like a library of strings that can be registered and called easily; but maybe thats also better to have at faerie_core
class FaerieVoice
{
public:
	FaerieVoice();
	~FaerieVoice();	
	
	void Init(int _speakRate = 165, int _pitch = 60, int _pitchVariance = 5);
	
	void SetVoice(espeak_VOICE* _pVoice);
	
	void Say(char* _pSentence, size_t _SentenceByteSize);
	void Say(const char* _pSentence, size_t _SentenceByteSize);
	void SayNoPhonemes(char* _pSentence, size_t _SentenceByteSize);
		
	static const int s_DefaultGender = 2;
	static const int s_DefaultAge = 17;
	static const char* s_DefaultLanguage;
	
	int m_SpeakRate;
	int m_Pitch;
	int m_PitchVariance;
	
	espeak_VOICE* m_pVoice;
	
	static int s_ObjectsAlive; // since espeak is a static library we keep track whether another FaerieVoice object has initialized it
	static FaerieVoice* s_LastUser;
	
private:
	
	void InitDefaultVoice();
	
	void InitInternal();
};