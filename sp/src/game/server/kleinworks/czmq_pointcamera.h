//===================| PROPERTY OF THE KLEINWORKS™ CORPORTATION®® |===================\\
//
// Purpose: 
// 
// Product Contributors: Barber
//
//===================| PROPERTY OF THE KLEINWORKS™ CORPORTATION®® |===================\\

#pragma once

#include "cbase.h"
#include "czmq_baseentity.h"
#include "point_camera.h"





class CzmqPointCamera : public CzmqBaseEntity
{
public:
	CzmqPointCamera(CBaseHandle hEntity);
	~CzmqPointCamera();




	/*======Member-Variables======*/

	bool	m_bIsPlayerCamera;




	/*======Member-Functions======*/


	rapidjson::Value   GetEntityData(rapidjson::MemoryPoolAllocator<> &allocator) override;

};