//===================| PROPERTY OF THE KLEINWORKS™ CORPORTATION®® |===================\\
//
// Purpose: "Provides some controls for C_zmqManager, and is used to update the EntRec
//	control panel VGUI."
//
// 
// Product Contributors: Barber
//
//===================| PROPERTY OF THE KLEINWORKS™ CORPORTATION®® |===================\\

#include "cbase.h"
#include <networkvar.h>
#include "VGUI\ientrec_controlpanel.h"



class C_EntRec_Interface : public C_BaseEntity
{
public:
	DECLARE_CLASS(C_EntRec_Interface, C_BaseEntity);
	DECLARE_CLIENTCLASS();

	using C_BaseEntity::C_BaseEntity;

	/*======Member-Variables======*/
public:

	const char* m_CurrentDataBuffer;
	const char* m_CurrentMetaDataBuffer;

	int  m_OUTPUTtickCount;
	int  m_INPUTtickCount;
	int  m_RecordRate;


	bool m_isSendingOutput;
	bool m_isReceivingInput;

	/*======Member-Functions======*/

	void ClientThink();

	void OnDataChanged(DataUpdateType_t updateType);

	

};


LINK_ENTITY_TO_CLASS(entrec_interface, C_EntRec_Interface);



IMPLEMENT_CLIENTCLASS_DT(C_EntRec_Interface, DT_EntRec_Interface, CEntRec_Interface)

	RecvPropString(RECVINFO(m_CurrentDataBuffer)),
	RecvPropString(RECVINFO(m_CurrentMetaDataBuffer)),
	RecvPropInt(RECVINFO(m_OUTPUTtickCount)),
	RecvPropInt(RECVINFO(m_INPUTtickCount)),
	RecvPropInt(RECVINFO(m_RecordRate)),
	RecvPropBool(RECVINFO(m_isSendingOutput)),
	RecvPropBool(RECVINFO(m_isReceivingInput)),

END_RECV_TABLE()





extern IEntRecControlPanel* entrec_controlpanel;


void C_EntRec_Interface::ClientThink()
{
	BaseClass::ClientThink();
	entrec_controlpanel->Update(m_CurrentDataBuffer, m_CurrentMetaDataBuffer, m_INPUTtickCount, m_OUTPUTtickCount);

	SetNextThink(CLIENT_THINK_ALWAYS);
}



void C_EntRec_Interface::OnDataChanged(DataUpdateType_t updateType)
{
	BaseClass::OnDataChanged(updateType);

	if (updateType == DATA_UPDATE_CREATED)
	{
		// This is called when the entity is first created on the client
		SetNextClientThink(CLIENT_THINK_ALWAYS);
	}
}

