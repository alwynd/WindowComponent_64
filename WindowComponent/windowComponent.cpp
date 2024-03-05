#include "windowComponent_64.h"

///Constructor.
Component::Component()
{
	id = NULL;
	hwnd = NULL;

	className = NULL;
	text = NULL;

	style = WS_CHILD | WS_VISIBLE | WS_BORDER;
	ZeroMemory(&position, sizeof(RECT));
};

///Destructor.
Component::~Component()
{
};
