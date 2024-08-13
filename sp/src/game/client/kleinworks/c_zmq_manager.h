//===================| PROPERTY OF THE KLEINWORKS™ CORPORTATION®® |===================\\
//
// Purpose: 
// 
// Product Contributors: Barber
//
//===================| PROPERTY OF THE KLEINWORKS™ CORPORTATION®® |===================\\

#pragma once

#include "cbase.h"

#include "ZMQ\srcIPC_EntRec.h"
#include "player.h"
#include "fmtstr.h"

#include <algorithm>

#include "czmq_baseentity.h"
#include "czmq_pointcamera.h"
#include "czmq_baseskeletal.h"






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


	rapidjson::Document m_entity_metadata_js;

	
	srcIPC::EntRec m_zmq_comms = srcIPC::EntRec(5555, 5556);


	
	/*======Member-Functions======*/
public:
	void    OnTick();

	void	UpdateSelectedEntities();

	void	RemoveEntityFromSelection(CzmqBaseEntity* pEntity);
	void	AddEntityToSelection(CBaseHandle hEntity);


	void	ClearEntitySelection();


	void	HandleSelectedEntityDestroyed(CzmqBaseEntity* pCaller);


};



