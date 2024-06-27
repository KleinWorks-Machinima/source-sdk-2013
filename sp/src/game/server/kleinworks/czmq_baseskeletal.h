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





class CzmqBaseSkeletal : public CzmqBaseEntity
{
public:
	CzmqBaseSkeletal(CBaseHandle hEntity);
	~CzmqBaseSkeletal();




	/*======Member-Variables======*/






	/*======Member-Functions======*/


	virtual rapidjson::Value GetEntityData(rapidjson::MemoryPoolAllocator<> &allocator) override;

};