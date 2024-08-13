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

#include "../c_baseentity.h"
#include "../cliententitylist.h"
//#include "../entityapi.h"

#include "fmtstr.h"




const enum class ENTREC_TYPES : int
{
	BASE_ENTITY   = 0xF00,
	POINT_CAMERA  = 0xF01,
	BASE_SKELETAL = 0xF02
};




class C_zmqBaseEntity : public IClientEntityListener
{
public:
	C_zmqBaseEntity(CBaseHandle hEntity);
	C_zmqBaseEntity();
	~C_zmqBaseEntity();

	/*======Member-Variables======*/


	CBaseHandle	   mh_parent_entity;

	int			   m_ent_type;
	const char*    m_ent_name;
	const char*    m_ent_model;


	/*==========Events============*/


	__event void OnParentEntityDestroyed(C_zmqBaseEntity* pCaller);


	/*======Member-Functions======*/
public:

	virtual rapidjson::Value   GetEntityData(rapidjson::MemoryPoolAllocator<> &allocator);
	virtual rapidjson::Value   GetEntityMetaData(rapidjson::MemoryPoolAllocator<> &allocator);

	bool			operator ==(const C_zmqBaseEntity&  other) const;
	bool			operator ==(const C_zmqBaseEntity*  other) const;
	bool			operator ==(const C_zmqBaseEntity   other) const;
	//bool operator ==(const std::unique_ptr<C_zmqBaseEntity> other) const;
	bool			operator ==(const CBaseHandle      other) const;


private:

	void OnEntityDeleted( C_BaseEntity *pEntity ) override
	{

		if (pEntity->GetEntityIndex() != cl_entitylist->GetBaseEntityFromHandle(mh_parent_entity)->GetEntityIndex())
			return;

		OnParentEntityDestroyed(this);

		delete this;
	}


};