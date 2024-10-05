//===================| PROPERTY OF THE KLEINWORKS™ CORPORTATION®® |===================\\
//
// Purpose: 
// 
// Product Contributors: Barber
//
//===================| PROPERTY OF THE KLEINWORKS™ CORPORTATION®® |===================\\

#pragma once

#include "czmq_baseentity_shared.h"

#ifndef CLIENT_DLL
class CRagdoll;
#endif




class CzmqBaseSkeletal : public CzmqBaseEntity
{
public:
	CzmqBaseSkeletal(CBaseHandle hEntity);
	~CzmqBaseSkeletal();




	/*======Member-Variables======*/
public:
	bool	   mb_is_npc;
	bool	   mb_is_ragdoll;

	CStudioHdr *mp_parent_model;


	CRagdoll   *mp_ragdoll;






	/*======Member-Functions======*/


	void			 OnParentRagdolled(CRagdoll &pParentRagdoll);


	bool			 IsValid() override;


	rapidjson::Value GetEntityData(rapidjson::MemoryPoolAllocator<> &allocator) override;
	rapidjson::Value GetEntityMetaData(rapidjson::MemoryPoolAllocator<> &allocator) override;

};