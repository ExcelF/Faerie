#ifndef FAERIE_EARS_H
#define FAERIE_EARS_H
#include <iostream>

class FaerieEars
{
public:
	FaerieEars();
	~FaerieEars();
	
	void Init();
	void Shutdown();
	void SetGrammar(std::string _pathToGrammar, std::string _grammarName);
	int GetRecognizerQueueSize();
	void ClearRecognizerQueue();
	
	std::string PopRecognizedString();
	
	
};
#endif
