#include "winhook.h"

void GGInput::setCallBack(function<void> callback)
{
	m_callback = callback;
}
