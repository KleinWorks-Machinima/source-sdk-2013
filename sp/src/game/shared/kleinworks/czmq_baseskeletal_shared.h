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
	CzmqBaseSkeletal(CBaseHandle hEntity, bool isRagdoll = false);
	~CzmqBaseSkeletal();




	/*======Member-Variables======*/
public:
	bool	   mb_is_npc;
	bool	   mb_is_ragdoll;

	CStudioHdr *mp_parent_model;


	CRagdoll   *mp_ragdoll;






	/*======Member-Functions======*/


	void OnEntityDeleted(CBaseEntity *pEntity) override
	{
		if (mb_is_ragdoll)
			return;

#ifdef CLIENT_DLL
		if (pEntity->GetEntityIndex() != cl_entitylist->GetBaseEntityFromHandle(mh_parent_entity)->GetEntityIndex())
			return;

#else
		if (pEntity->edict() != gEntList.GetEdict(mh_parent_entity))
			return;

#endif // CLIENT_DLL

		OnParentEntityDestroyed(this);

		delete this;
	}


	void			 OnParentRagdolled(CRagdoll &pParentRagdoll);


	bool			 IsValid() override;


	rapidjson::Value GetEntityData(rapidjson::MemoryPoolAllocator<> &allocator) override;
	rapidjson::Value GetEntityMetaData(rapidjson::MemoryPoolAllocator<> &allocator) override;

};