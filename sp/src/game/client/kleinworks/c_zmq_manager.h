//===================| PROPERTY OF THE KLEINWORKS™ CORPORTATION®® |===================\\
//
// Purpose: 
// 
// Product Contributors: Barber
//
//===================| PROPERTY OF THE KLEINWORKS™ CORPORTATION®® |===================\\

#pragma once

#include "cbase.h"

// ZMQ is shared which is why kleinworks/ is specified
#include "kleinworks/ZMQ/srcIPC_EntRec.h"
#include "c_baseplayer.h"
#include "fmtstr.h"

#include <algorithm>

#include "c_zmq_baseentity.h"
#include "c_zmq_pointcamera.h"
#include "c_zmq_baseskeletal.h"






class C_zmqManager
{
public:
	C_zmqManager();
	~C_zmqManager();

	/*======Member-Variables======*/
public:
	std::vector<std::unique_ptr<C_zmqBaseEntity>> m_pSelected_EntitiesList;
	
	
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

	void	RemoveEntityFromSelection(C_zmqBaseEntity* pEntity);
	void	AddEntityToSelection(CBaseHandle hEntity);


	void	ClearEntitySelection();


	void	HandleSelectedEntityDestroyed(C_zmqBaseEntity* pCaller);


};



