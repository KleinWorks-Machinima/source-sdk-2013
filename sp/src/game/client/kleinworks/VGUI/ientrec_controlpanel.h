//===================| PROPERTY OF THE KLEINWORKS™ CORPORTATION®® |===================\\
//
// Purpose: 
// 
// Product Contributors: Barber
//
//===================| PROPERTY OF THE KLEINWORKS™ CORPORTATION®® |===================\\



#include <vgui/VGUI.h>

namespace vgui
{
	class Panel;
}



abstract_class IEntRecControlPanel
{
public:
	virtual void		Create(vgui::VPANEL parent) = 0;
	virtual void		Destroy(void) = 0;
	virtual void	    Update(const char* data, const char* metadata, int INPUTtickCount, int OUTPUTtickCount) = 0;

};

extern IEntRecControlPanel* entrec_controlpanel;
