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
#include "czmq_manager.h"
#include <networkvar.h>



class CEntRec_Interface : public CBaseEntity
{
public:
	DECLARE_CLASS(CEntRec_Interface, CLogicalEntity);
	DECLARE_SERVERCLASS();
	DECLARE_DATADESC();

	CEntRec_Interface();
	
	/*======Member-Variables======*/
	
	CNetworkVar(const char*, m_CurrentDataBuffer);
	CNetworkVar(const char*, m_CurrentMetaDataBuffer);
	CNetworkVar(int, m_OUTPUTtickCount);
	CNetworkVar(int, m_INPUTtickCount);
	//CNetworkVar(int, m_RecordRate);
	CNetworkVar(bool, m_isSendingOutput);
	CNetworkVar(bool, m_isReceivingInput);


	/*======Member-Functions======*/
public:

	int UpdateTransmitState()
	{
		return SetTransmitState(FL_EDICT_ALWAYS);
	}


	void StartRecording(inputdata_t &inputData);

	void StopRecording(inputdata_t &inputData);


	void AddEntToRecording(inputdata_t &inputData);


	void UpdateNetworkedVars();

};

extern CzmqManager g_CzmqManager;



LINK_ENTITY_TO_CLASS(entrec_interface, CEntRec_Interface);



//==========\\
//-DATADESC-\\
//==========\\

BEGIN_DATADESC(CEntRec_Interface)

	DEFINE_INPUTFUNC(FIELD_VOID, "StartRecording", StartRecording),
	DEFINE_INPUTFUNC(FIELD_VOID, "StopRecording", StopRecording),
	DEFINE_INPUTFUNC(FIELD_EHANDLE, "AddEntToRecording", AddEntToRecording),
	DEFINE_THINKFUNC(UpdateNetworkedVars),

END_DATADESC();


//============\\
//-DATA-TABLE-\\
//============\\

IMPLEMENT_SERVERCLASS_ST(CEntRec_Interface, DT_EntRec_Interface)

	SendPropString(SENDINFO(m_CurrentDataBuffer)),
	SendPropString(SENDINFO(m_CurrentMetaDataBuffer)),
	SendPropInt(SENDINFO(m_OUTPUTtickCount)),
	SendPropInt(SENDINFO(m_INPUTtickCount)),
	//SendPropInt(SENDINFO(m_RecordRate)),
	SendPropBool(SENDINFO(m_isSendingOutput)),
	SendPropBool(SENDINFO(m_isReceivingInput)),

END_SEND_TABLE()





CEntRec_Interface::CEntRec_Interface()
{
	SetTransmitState(FL_EDICT_ALWAYS);
	SetThink (&CEntRec_Interface::UpdateNetworkedVars );
	SetNextThink(gpGlobals->curtime);
}




void CEntRec_Interface::StartRecording(inputdata_t &inputData)
{
	ConVar* kleinworksRecordCvar = cvar->FindVar("KW_entrec_record");
	if (kleinworksRecordCvar->GetBool() != true)
		kleinworksRecordCvar->SetValue(true);

}



void CEntRec_Interface::StopRecording(inputdata_t &inputData)
{
	ConVar* kleinworksRecordCvar = cvar->FindVar("KW_entrec_record");
	if (kleinworksRecordCvar->GetBool() != false)
		kleinworksRecordCvar->SetValue(false);
}




void CEntRec_Interface::AddEntToRecording(inputdata_t &inputData)
{
	inputData.value.Convert(FIELD_EHANDLE, this->GetBaseEntity(), inputData.pActivator, inputData.pCaller);

	DevMsg(3, "KleinWorks: Added entity with targetname %s to EntRec selection!\n", inputData.value.Entity().Get()->GetDebugName());

	g_CzmqManager.AddEntityToSelection(inputData.value.Entity());
}




void CEntRec_Interface::UpdateNetworkedVars()
{
	//  if the entrec control panel isn't visible, theres no reason to update it
	if (cvar->FindVar("entrec_showcontrolpanel")->GetBool() != true) {
		SetNextThink(gpGlobals->curtime);
		return;
	}
	


	m_OUTPUTtickCount  = g_CzmqManager.m_zmq_comms.m_INPUT_tick_count;
	m_INPUTtickCount   = g_CzmqManager.m_zmq_comms.m_OUTPUT_tick_count;

	//m_RecordRate	   = g_CzmqManager.record_rate;
	m_isSendingOutput  = g_CzmqManager.m_zmq_comms.m_isSendingOutput;
	m_isReceivingInput = g_CzmqManager.m_zmq_comms.m_isReceivingInput;


	
	/*
	if (g_CzmqManager.m_zmq_comms.m_sending_data_buffer == nullptr)
		m_CurrentDataBuffer = "NO_DATA";
	else
		if (zframe_is(g_CzmqManager.m_zmq_comms.m_sending_data_buffer))
			m_CurrentDataBuffer = zframe_strdup(g_CzmqManager.m_zmq_comms.m_sending_data_buffer);
	/*

	if (g_CzmqManager.m_zmq_comms.m_sending_metadata != NULL) {


		m_CurrentMetaDataBuffer = g_CzmqManager.m_zmq_comms.m_sending_metadata;
	}
	else
		m_CurrentMetaDataBuffer = "NO_METADATA";
	*/
	SetNextThink(gpGlobals->curtime);
}


