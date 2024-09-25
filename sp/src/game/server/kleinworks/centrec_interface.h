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
#include "kleinworks/czmq_manager_shared.h"
#include <networkvar.h>



class CEntRec_Interface : public CPointEntity
{
public:
	DECLARE_CLASS(CEntRec_Interface, CPointEntity);
	DECLARE_SERVERCLASS();


	DECLARE_DATADESC();

	CEntRec_Interface();

	/*======Member-Variables======*/

	//CNetworkVar(int, m_RecordRate);
	CNetworkVar(bool, m_isRecording);
	CNetworkVar(bool, m_isReceivingInput);


	CNetworkVar(int, m_start_record_tick);
	CNetworkVar(int, m_end_record_tick);

	int			m_last_tick;


	/*======Member-Functions======*/
public:

	int UpdateTransmitState()
	{
		return SetTransmitState(FL_EDICT_ALWAYS);
	}


	void StartRecording(inputdata_t &inputData);
	void StopRecording(inputdata_t &inputData);

	void AddEntToRecording(inputdata_t &inputData);

	void Think() override;

	void UpdateNetworkedVars();

	void CzmqOnTick();

};