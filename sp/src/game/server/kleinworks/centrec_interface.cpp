//===================| PROPERTY OF THE KLEINWORKS™ CORPORTATION®® |===================\\
//
// Purpose: "Provides some controls for CzmqManager in Hammer, and is used to update the EntRec
//	control panel VGUI."
//
// 
// Product Contributors: Barber
//
//===================| PROPERTY OF THE KLEINWORKS™ CORPORTATION®® |===================\\

#include "cbase.h"
#include "centrec_interface.h"







CEntRec_Interface::CEntRec_Interface()
{
	m_isRecording = false;

	SetTransmitState(FL_EDICT_ALWAYS);
	SetNextThink(gpGlobals->curtime);
}



LINK_ENTITY_TO_CLASS(entrec_interface, CEntRec_Interface);



//==========\\
//-DATADESC-\\
//==========\\

BEGIN_DATADESC(CEntRec_Interface)
DEFINE_INPUTFUNC(FIELD_VOID, "StartRecording", StartRecording),
DEFINE_INPUTFUNC(FIELD_VOID, "StopRecording", StopRecording),
DEFINE_INPUTFUNC(FIELD_EHANDLE, "AddEntToRecording", AddEntToRecording),
END_DATADESC();


//============\\
//-DATA-TABLE-\\
//============\\

IMPLEMENT_SERVERCLASS_ST(CEntRec_Interface, DT_EntRec_Interface)
SendPropBool(SENDINFO(m_isRecording)),
SendPropBool(SENDINFO(m_isReceivingInput)),
SendPropInt(SENDINFO(m_start_record_tick)),
SendPropInt(SENDINFO(m_end_record_tick)),
END_SEND_TABLE()


extern CzmqManager g_CzmqManager;


void CEntRec_Interface::StartRecording(inputdata_t &inputData)
{
	ConVar* kleinworksRecordCvar = cvar->FindVar("KW_entrec_record");
	if (kleinworksRecordCvar->GetBool() != true) {
		kleinworksRecordCvar->SetValue(true);

		UpdateNetworkedVars();
	}

}



void CEntRec_Interface::StopRecording(inputdata_t &inputData)
{
	ConVar* kleinworksRecordCvar = cvar->FindVar("KW_entrec_record");
	if (kleinworksRecordCvar->GetBool() != false) {
		kleinworksRecordCvar->SetValue(false);

		UpdateNetworkedVars();
	}

}




void CEntRec_Interface::AddEntToRecording(inputdata_t &inputData)
{
	inputData.value.Convert(FIELD_EHANDLE, this->GetBaseEntity(), inputData.pActivator, inputData.pCaller);

	DevMsg(3, "kleinworks_sv: Added entity with targetname %s to EntRec selection!\n", inputData.value.Entity().Get()->GetDebugName());

	g_CzmqManager.AddEntityToSelection(inputData.value.Entity());
}




void CEntRec_Interface::Think()
{
	BaseClass::Think();

	UpdateNetworkedVars();



	if (m_end_record_tick != 0 && gpGlobals->tickcount == m_end_record_tick) {
		Msg("kleinworks_sv_DEBUG: End record tick [%d].\n", gpGlobals->tickcount);

		g_CzmqManager.SetRecording(false);
	}
	

	//CzmqOnTick();


	SetNextThink(gpGlobals->curtime);
}




void CEntRec_Interface::CzmqOnTick()
{

	// if we are in the middle of stopping a recording, run anyway
	if (g_CzmqManager.m_zmq_comms.m_isDoneTransfering || g_CzmqManager.m_zmq_comms.m_peerIsDoneTransfering) {

		g_CzmqManager.OnTick();
		return;
	}

	if (engine->IsPaused()) 
		return;
	

	if (m_start_record_tick != 0 && gpGlobals->tickcount < m_start_record_tick)
		return;
	

	if (gpGlobals->tickcount == m_start_record_tick)
		Msg("kleinworks_sv_DEBUG: Start record tick [%d].\n", gpGlobals->tickcount);
	


	if (m_last_tick < gpGlobals->tickcount) {
		m_last_tick = gpGlobals->tickcount;
		g_CzmqManager.OnTick();
	}

}




void CEntRec_Interface::UpdateNetworkedVars()
{
	bool isRecordingCvar = cvar->FindVar("kw_entrec_message_record")->GetBool();

	if (isRecordingCvar != m_isRecording) {

		int tickDelayFactor = cvar->FindVar("kw_entrec_record_delay_factor")->GetInt();

		if (isRecordingCvar == true) {
			m_start_record_tick = gpGlobals->tickcount + tickDelayFactor;
			g_CzmqManager.m_start_record_tick = m_start_record_tick;

			Msg("kleinworks: m_start_record_tick = [%d].\n", m_start_record_tick);
		

			m_end_record_tick   = 0;
			g_CzmqManager.m_end_record_tick = m_end_record_tick;

			g_CzmqManager.SetRecording(isRecordingCvar);

		}
		else {
			m_end_record_tick   = gpGlobals->tickcount + tickDelayFactor;
			g_CzmqManager.m_end_record_tick = m_end_record_tick;

			Msg("kleinworks: m_end_record_tick = [%d].\n", m_end_record_tick);

			m_start_record_tick = 0;
			g_CzmqManager.m_start_record_tick = m_start_record_tick;
		}


		m_isRecording = isRecordingCvar;

		m_last_tick = gpGlobals->tickcount;

	}

}


