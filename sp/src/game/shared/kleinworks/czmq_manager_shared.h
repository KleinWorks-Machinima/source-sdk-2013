//===================| PROPERTY OF THE KLEINWORKS™ CORPORTATION®® |===================\\
//
// Purpose: 
// 
// Product Contributors: Barber
//
//===================| PROPERTY OF THE KLEINWORKS™ CORPORTATION®® |===================\\

#pragma once

#include "cbase.h"

#include "ZMQ/srcIPC_EntRec.h"
#include "czmq_baseentity_shared.h"
#include "czmq_pointcamera_shared.h"
#include "czmq_baseskeletal_shared.h"

#include "fmtstr.h"

#include <algorithm>


// this is shared so thats why kleinworks/ is specified
#ifdef CLIENT_DLL

#define KW_OUTPUT_PORTNUM 5533

#include "c_baseplayer.h"

#else

#define KW_OUTPUT_PORTNUM 5577

#include "player.h"

#endif // CLIENT_DLL






class CzmqManager
{
public:
	CzmqManager();
	~CzmqManager();

	/*======Member-Variables======*/
public:
	std::vector<std::unique_ptr<CzmqBaseEntity>> m_pSelected_EntitiesList;
	
	
	int	 m_RecordUntil;
	bool record_toggle;
	int	 record_frame_start;
	int	 record_frame_end;

	
	srcIPC::EntRec m_zmq_comms = srcIPC::EntRec(5555, KW_OUTPUT_PORTNUM);


	
	/*======Member-Functions======*/
public:
	void    OnTick();

	void	UpdateSelectedEntities();

	void	RemoveEntityFromSelection(CzmqBaseEntity* pEntity);
	void	AddEntityToSelection(CBaseHandle hEntity);


	void	ClearEntitySelection();

	void    SetRecording(bool value);

	void	HandleSelectedEntityDestroyed(CzmqBaseEntity* pCaller);

private:

	rapidjson::Document		GetEntityMetadata();

	rapidjson::Document		GetEntityMetadata(CzmqBaseEntity* pEntity);

};



