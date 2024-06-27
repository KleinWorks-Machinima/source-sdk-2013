//===================| PROPERTY OF THE KLEINWORKS™ CORPORTATION®® |===================\\
//
// Purpose: 
// 
// Product Contributors: Barber
//
//===================| PROPERTY OF THE KLEINWORKS™ CORPORTATION®® |===================\\


#include "cbase.h"

#include "ientrec_controlpanel.h"
#include "../c_user_message_register.h"
#include <vgui/IVGui.h>
#include <vgui_controls/Frame.h>
#include <vgui_controls/Label.h>
#include <vgui_controls/RichText.h>
#include <vgui_controls/Button.h>
#include <string>

using namespace vgui;


class CEntRecControlPanel : public vgui::Frame
{
	DECLARE_CLASS_SIMPLE(CEntRecControlPanel, vgui::Frame);
public:
	CEntRecControlPanel(vgui::VPANEL parent);
	~CEntRecControlPanel(){ BaseClass::~Frame(); };


	/*======Member-Variables======*/
private:

	vgui::Button* m_pHideButton;

	vgui::Label*  m_pDataBufferLabel;
	vgui::Label*  m_pMetadataBufferLabel;

	vgui::Label*  m_pINPUTtickCountLabel;
	vgui::Label*  m_pOUTPUTtickCountLabel;

public:

	vgui::RichText*  m_pDataBuffer;
	vgui::RichText*  m_pMetadataBuffer;

	vgui::RichText*  m_pINPUTtickCount;
	vgui::RichText*  m_pOUTPUTtickCount;



	/*======Member-Functions======*/
protected:

	virtual void OnTick();

	virtual void OnCommand(const char* pcCommand);

};






CEntRecControlPanel::CEntRecControlPanel(vgui::VPANEL parent) : BaseClass(NULL, "EntRecControlPanel")
{
	SetParent(parent);

	SetKeyBoardInputEnabled(true);
	SetMouseInputEnabled(true);
	SetCloseButtonVisible(false);
	SetMinimizeButtonVisible(false);
	SetMaximizeButtonVisible(false);

	SetSizeable(false);
	SetMoveable(true);
	SetVisible(true);


	m_pHideButton = new vgui::Button(this, "HidePanel", "Close", this, "hide");
	
	m_pDataBufferLabel	    = new vgui::Label(this, "DataBufferLabel", "Active Data Buffer");
	m_pMetadataBufferLabel  = new vgui::Label(this, "MetadataBufferLabel", "Active Metadata Buffer");

	m_pINPUTtickCountLabel  = new vgui::Label(this, "INPUTtickCountLabel", "INPUT Tick Count");
	m_pOUTPUTtickCountLabel = new vgui::Label(this, "OUTPUTtickCountLabel", "OUTPUT Tick Count");

	m_pDataBuffer	   = new vgui::RichText(this, "DataBuffer");
	m_pMetadataBuffer  = new vgui::RichText(this, "MetadataBuffer");

	m_pINPUTtickCount  = new vgui::RichText(this, "INPUTtickCount");
	m_pOUTPUTtickCount = new vgui::RichText(this, "OUTPUTtickCount");

	SetScheme(vgui::scheme()->LoadSchemeFromFile("resource/SourceScheme.res", "SourceScheme"));

	LoadControlSettings("resource/UI/entrec_controlpanel.res");



	vgui::ivgui()->AddTickSignal(GetVPanel(), 100);


}





ConVar entrec_showcontrolpanel("entrec_showcontrolpanel", "0", FCVAR_CLIENTDLL, "Sets the state of EntRecControlPanel <state>");

CON_COMMAND(entrec_debug_controlpanel, "Toggles the EntRec control panel.")
{
	entrec_showcontrolpanel.SetValue(!entrec_showcontrolpanel.GetBool());
};




void CEntRecControlPanel::OnTick()
{
	BaseClass::OnTick();
	SetVisible(entrec_showcontrolpanel.GetBool());
}



void CEntRecControlPanel::OnCommand(const char* pcCommand)
{
	BaseClass::OnCommand(pcCommand);
	if (!Q_stricmp(pcCommand, "hide")) {
		entrec_showcontrolpanel.SetValue(false);
	}
}





class CEntRecControl : public IEntRecControlPanel
{
private:
	CEntRecControlPanel *m_pEntRecControlPanel;
public:
	CEntRecControl(void)
	{
		m_pEntRecControlPanel = NULL;
	}

	void Create(vgui::VPANEL parent)
	{
		m_pEntRecControlPanel = new CEntRecControlPanel(parent);
	}

	void Destroy(void)
	{
		if (m_pEntRecControlPanel)
		{
			m_pEntRecControlPanel->SetParent((vgui::Panel *)NULL);
			delete m_pEntRecControlPanel;
			m_pEntRecControlPanel = NULL;
		}
	}


	void Update(const char* data, const char* metadata, int INPUTtickCount, int OUTPUTtickCount/*, int recordRate, bool isSendingOutput, bool m_isReceivingInput*/)
	{
		if (m_pEntRecControlPanel == nullptr)
			return;
		
		
		m_pEntRecControlPanel->m_pDataBuffer->SetText(data);
		m_pEntRecControlPanel->m_pMetadataBuffer->SetText(metadata);


		

		m_pEntRecControlPanel->m_pINPUTtickCount->SetText(std::to_string(INPUTtickCount).c_str());
		m_pEntRecControlPanel->m_pOUTPUTtickCount->SetText(std::to_string(OUTPUTtickCount).c_str());

		
	}
};


static CEntRecControl g_EntRecControlPanel;
IEntRecControlPanel *entrec_controlpanel = (IEntRecControlPanel *)&g_EntRecControlPanel;









