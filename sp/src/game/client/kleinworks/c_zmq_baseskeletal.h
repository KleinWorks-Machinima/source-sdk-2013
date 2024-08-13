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

#include "bone_setup.h"

#include "c_zmq_baseentity.h"





class C_zmqBaseSkeletal : public C_zmqBaseEntity
{
public:
	C_zmqBaseSkeletal(CBaseHandle hEntity);
	~C_zmqBaseSkeletal();




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