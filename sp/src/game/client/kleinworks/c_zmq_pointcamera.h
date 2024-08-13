//===================| PROPERTY OF THE KLEINWORKS™ CORPORTATION®® |===================\\
//
// Purpose: 
// 
// Product Contributors: Barber
//
//===================| PROPERTY OF THE KLEINWORKS™ CORPORTATION®® |===================\\

#pragma once

#include "cbase.h"
#include "c_zmq_baseentity.h"
#include "c_point_camera.h"





class C_zmqPointCamera : public C_zmqBaseEntity
{
public:
	C_zmqPointCamera(CBaseHandle hEntity);
	~C_zmqPointCamera();




	/*======Member-Variables======*/

	bool	m_bIsPlayerCamera;




	/*======Member-Functions======*/


	rapidjson::Value   GetEntityData(rapidjson::MemoryPoolAllocator<> &allocator) override;

};