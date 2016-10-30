#pragma once

#include <string>

struct FaerieSpeechEvent
{
	std::string m_RecognizedString;
	bool m_IsHandled;
};