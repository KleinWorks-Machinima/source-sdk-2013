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
#include "fmtstr.h"

#include <algorithm>


// this is shared so thats why kleinworks/ is specified
#ifdef CLIENT_DLL
#include "c_baseplayer.h"

#include "kleinworks/c_zmq_baseentity.h"
#include "kleinworks/c_zmq_pointcamera.h"
#include "kleinworks/c_zmq_baseskeletal.h"
/*
typedef CzmqBaseEntity   (C_zmqBaseEntity);
typedef CzmqPointCamera  (C_zmqPointCamera);
typedef CzmqBaseSkeletal (C_zmqBaseSkeletal);
*/

#define CzmqBaseEntity   C_zmqBaseEntity
#define CzmqPointCamera  C_zmqPointCamera
#define CzmqBaseSkeletal C_zmqBaseSkeletal
#else
#include "player.h"

#include "kleinworks/czmq_baseentity.h"
#include "kleinworks/czmq_pointcamera.h"
#include "kleinworks/czmq_baseskeletal.h"

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


	rapidjson::Document m_entity_metadata_js;

	
	srcIPC::EntRec m_zmq_comms = srcIPC::EntRec(5555, 5556);


	
	/*======Member-Functions======*/
public:
	void    OnTick();

	void	UpdateSelectedEntities();

	void	RemoveEntityFromSelection(CzmqBaseEntity* pEntity);
	void	AddEntityToSelection(CBaseHandle hEntity);


	void	ClearEntitySelection();

	void    SetRecording(bool value);

	void	HandleSelectedEntityDestroyed(CzmqBaseEntity* pCaller);


};



