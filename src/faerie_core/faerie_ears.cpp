#include "faerie_ears.h"

#include "../include/voce.h"

#ifdef WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

// Currently using Voce. Which is a static interface to Java. Thus this could currently be
// static aswell but, we'll keep it as a object class for now. Maybe will use Julius in the future.

FaerieEars::FaerieEars()
{
	
}

FaerieEars::~FaerieEars()
{
	Shutdown();
}

void FaerieEars::Init()
{
	voce::init("../../voce-0.9.1/lib", false, true, "./grammar", "faerie_main");	// TODO: currently this path etc. is hardcoded; can we load it from a config?
																				// Also: This is only able to load a single grammar, how to handle that
}

void FaerieEars::Shutdown()
{
	voce::destroy();
}

int FaerieEars::GetRecognizerQueueSize()
{
	int i = voce::getRecognizerQueueSize();
	return i;
}

std::string FaerieEars::PopRecognizedString()
{
	return voce::popRecognizedString();
}