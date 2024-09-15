//===================| PROPERTY OF THE KLEINWORKS™ CORPORTATION®® |===================\\
//
// Purpose: 
// 
// Product Contributors: Barber
//
//===================| PROPERTY OF THE KLEINWORKS™ CORPORTATION®® |===================\\

#pragma once

#include "cbase.h"

#pragma warning(push)
#pragma warning(disable: 4005)
#pragma warning(disable: 4003)

#include <rapidjson/document.h>
#include <rapidjson/reader.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

#pragma warning(pop)

#ifdef CLIENT_DLL

#include "../client/c_baseentity.h"
#include "../client/cliententitylist.h"

#define IEntityListener IClientEntityListener
#define CPointCamera    C_PointCamera

#else

#include "../server/baseentity.h"
#include "../server/entityapi.h"

#endif // CLIENT_DLL


#include "fmtstr.h"




const enum class ENTREC_TYPES : int
{
	BASE_ENTITY   = 0xF00,
	POINT_CAMERA  = 0xF01,
	BASE_SKELETAL = 0xF02
};




class CzmqBaseEntity : public IEntityListener
{
public:
	CzmqBaseEntity(CBaseHandle hEntity);
	CzmqBaseEntity();
	~CzmqBaseEntity();

	/*======Member-Variables======*/


	CBaseHandle	   mh_parent_entity;

	int			   m_ent_type;
	int			   m_ent_id;
	int			   m_ent_parent_id;
	int			   m_ent_numbones;
	const char*    m_ent_name;
	const char*    m_ent_model;


	/*==========Events============*/


	__event void OnParentEntityDestroyed(CzmqBaseEntity* pCaller);


	/*======Member-Functions======*/
public:

	virtual rapidjson::Value   GetEntityData(rapidjson::MemoryPoolAllocator<> &allocator);
	virtual rapidjson::Value   GetEntityMetaData(rapidjson::MemoryPoolAllocator<> &allocator);

	bool			IsValid();

	bool			operator ==(const CzmqBaseEntity&  other) const;
	bool			operator ==(const CzmqBaseEntity*  other) const;
	bool			operator ==(const CzmqBaseEntity   other) const;
	//bool operator ==(const std::unique_ptr<CzmqBaseEntity> other) const;
	bool			operator ==(const CBaseHandle      other) const;


private:

	void OnEntityDeleted( CBaseEntity *pEntity ) override
	{
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


};