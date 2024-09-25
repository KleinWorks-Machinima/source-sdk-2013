//===================| PROPERTY OF THE KLEINWORKS™ CORPORTATION®® |===================\\
//
// Purpose: 
// 
// Product Contributors: Barber
//
//===================| PROPERTY OF THE KLEINWORKS™ CORPORTATION®® |===================\\

#pragma once

#include "czmq_baseentity_shared.h"






class CzmqBaseSkeletal : public CzmqBaseEntity
{
public:
	CzmqBaseSkeletal(CBaseHandle hEntity);
	~CzmqBaseSkeletal();




	/*======Member-Variables======*/
public:
	bool	  mb_is_npc;
	bool	  mb_is_ragdoll;





	/*======Member-Functions======*/

	void			 OnParentRagdolled(CBaseHandle pParentRagdoll);


	rapidjson::Value GetEntityData(rapidjson::MemoryPoolAllocator<> &allocator) override;
	rapidjson::Value GetEntityMetaData(rapidjson::MemoryPoolAllocator<> &allocator) override;

};