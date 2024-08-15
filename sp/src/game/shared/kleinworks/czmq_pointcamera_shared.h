//===================| PROPERTY OF THE KLEINWORKS™ CORPORTATION®® |===================\\
//
// Purpose: 
// 
// Product Contributors: Barber
//
//===================| PROPERTY OF THE KLEINWORKS™ CORPORTATION®® |===================\\

#pragma once

#include "cbase.h"
#include "czmq_baseentity_shared.h"


#ifdef CLIENT_DLL

#include "c_point_camera.h"

#else

#include "../server/point_camera.h"

#endif // CLIENT_DLL





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