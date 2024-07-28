//===================| PROPERTY OF THE KLEINWORKS™ CORPORTATION®® |===================\\
//
// Purpose: 
// 
// Product Contributors: Barber
//
//===================| PROPERTY OF THE KLEINWORKS™ CORPORTATION®® |===================\\

#pragma once

#include "cbase.h"

#include <vector>

#include "czmq_baseentity.h"





class CzmqBaseSkeletal : public CzmqBaseEntity
{
public:
	CzmqBaseSkeletal(CBaseHandle hEntity);
	~CzmqBaseSkeletal();




	/*======Member-Variables======*/
public:
	bool	  mb_is_npc;
	bool	  mb_is_ragdoll;

	int		  m_numbones;

	std::vector<const char*> mch_bonenames_list;





	/*======Member-Functions======*/


	rapidjson::Value GetEntityData(rapidjson::MemoryPoolAllocator<> &allocator) override;
	rapidjson::Value GetEntityMetaData(rapidjson::MemoryPoolAllocator<> &allocator) override;

};