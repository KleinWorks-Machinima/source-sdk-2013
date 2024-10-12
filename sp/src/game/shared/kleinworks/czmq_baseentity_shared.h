//===================| PROPERTY OF THE KLEINWORKS™ CORPORTATION®® |===================\\
//
// Purpose: 
// 
// Product Contributors: Barber
//
//===================| PROPERTY OF THE KLEINWORKS™ CORPORTATION®® |===================\\

#pragma once

#include "cbase.h"

#ifdef CLIENT_DLL

#define IEntityListener IClientEntityListener
#define CPointCamera    C_PointCamera

#endif // CLIENT_DLL


#include "fmtstr.h"

#pragma warning(push)
#pragma warning(disable: 4348)

namespace rapidjson {
	class CrtAllocator;
	template<typename BaseAllocator = typename CrtAllocator>
	class MemoryPoolAllocator;

	template <typename Encoding, typename Allocator = MemoryPoolAllocator<> >
	class GenericValue;

	template<typename CharType = char>
	struct UTF8;

	typedef GenericValue<UTF8<> > Value;
}

#pragma warning(pop)




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
	bool		   mb_is_gib;



	/*==========Events============*/


	__event void OnParentEntityDestroyed(CzmqBaseEntity* pCaller);


	/*======Member-Functions======*/
public:

	virtual rapidjson::Value	GetEntityData(rapidjson::MemoryPoolAllocator<> &allocator);
	virtual rapidjson::Value	GetEntityMetaData(rapidjson::MemoryPoolAllocator<> &allocator);

	virtual bool				IsValid();

	bool						operator ==(const CzmqBaseEntity&  other) const;
	bool						operator ==(const CzmqBaseEntity*  other) const;
	bool						operator ==(const CzmqBaseEntity   other) const;
	//bool						operator ==(const std::unique_ptr<CzmqBaseEntity> other) const;
	bool						operator ==(const CBaseHandle      other) const;


private:

	virtual void OnEntityDeleted( CBaseEntity *pEntity ) override
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