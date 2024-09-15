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

#include "kleinworks/czmq_manager_shared.h"

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

	bool	m_isRecording;
	bool	m_isReceivingInput;

	int		m_start_record_tick;
	int		m_end_record_tick;
	int		m_last_tick;

	/*======Member-Functions======*/

	void ClientThink();

	void OnDataChanged(DataUpdateType_t updateType);

	void CzmqOnTick();

	

};


extern CzmqManager g_C_zmqManager;



LINK_ENTITY_TO_CLASS(entrec_interface, C_EntRec_Interface);



IMPLEMENT_CLIENTCLASS_DT(C_EntRec_Interface, DT_EntRec_Interface, CEntRec_Interface)
	RecvPropBool(RECVINFO(m_isRecording)),
	RecvPropBool(RECVINFO(m_isReceivingInput)),
	RecvPropInt(RECVINFO(m_start_record_tick)),
	RecvPropInt(RECVINFO(m_end_record_tick)),
END_RECV_TABLE()





extern IEntRecControlPanel* entrec_controlpanel;

void C_EntRec_Interface::ClientThink()
{
	BaseClass::ClientThink();


	if (m_end_record_tick != 0 && gpGlobals->tickcount == m_end_record_tick) {
		Msg("kleinworks_cl_DEBUG: End record tick [%d].\n", gpGlobals->tickcount);

		g_C_zmqManager.SetRecording(m_isRecording);
	}
		


	//CzmqOnTick();

	SetNextThink(gpGlobals->curtime);

}



void C_EntRec_Interface::CzmqOnTick()
{
	// if we are in the middle of stopping a recording, run anyway
	if (g_C_zmqManager.m_zmq_comms.m_isDoneTransfering || g_C_zmqManager.m_zmq_comms.m_peerIsDoneTransfering) {

		g_C_zmqManager.OnTick();
		return;
	}

	if (engine->IsPaused())
		return;


	if (m_start_record_tick != 0 && gpGlobals->tickcount < m_start_record_tick) 
		return;

	if (gpGlobals->tickcount == m_start_record_tick)
		Msg("kleinworks_cl_DEBUG: Start record tick [%d].\n", gpGlobals->tickcount);
	


	if (m_last_tick < gpGlobals->tickcount) {
		m_last_tick = gpGlobals->tickcount;
		g_C_zmqManager.OnTick();
	}
}



void C_EntRec_Interface::OnDataChanged(DataUpdateType_t updateType)
{
	BaseClass::OnDataChanged(updateType);

	g_C_zmqManager.m_start_record_tick = m_start_record_tick;
	g_C_zmqManager.m_end_record_tick   = m_end_record_tick;

	if (updateType == DATA_UPDATE_CREATED)
	{
		// This is called when the entity is first created on the client
		SetNextClientThink(CLIENT_THINK_ALWAYS);
	}

	if (m_isRecording != g_C_zmqManager.m_record_toggle && m_isRecording == true) 
		g_C_zmqManager.SetRecording(m_isRecording);
	

}

