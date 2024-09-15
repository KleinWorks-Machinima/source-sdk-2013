//===================| PROPERTY OF THE KLEINWORKS™ CORPORTATION®® |===================\\
//
// Purpose: 
// 
// Product Contributors: Barber
//
//===================| PROPERTY OF THE KLEINWORKS™ CORPORTATION®® |===================\\

#include "cbase.h"
#include "czmq_baseskeletal_shared.h"














CzmqBaseSkeletal::CzmqBaseSkeletal(CBaseHandle hEntity)
{
#ifdef CLIENT_DLL
	CBaseAnimating* pSkelEntity = cl_entitylist->GetBaseEntityFromHandle(hEntity)->GetBaseAnimating();

	cl_entitylist->AddListenerEntity(this);
#else
	CBaseAnimating* pSkelEntity = gEntList.GetBaseEntity(hEntity)->GetBaseAnimating();

	gEntList.AddListenerEntity(this);
#endif // CLIENT_DLL

	// we have to do this to avoid taking a reference to GetDebugName() or GetModelName()

	const char* modelname = modelinfo->GetModelName(pSkelEntity->GetModel());

	int ent_name_len = strlen(pSkelEntity->GetDebugName());
	int ent_modelname_len = strlen(modelname);

	char* ent_name_proxystr = new char[ent_name_len + 1];
	char* ent_modelname_proxystr = new char[ent_modelname_len + 1];

	strcpy_s(ent_name_proxystr, ent_name_len + 1, pSkelEntity->GetDebugName());
	strcpy_s(ent_modelname_proxystr, ent_modelname_len + 1, modelname);

	CStudioHdr* pEntModel = pSkelEntity->GetModelPtr();

	m_ent_name		 = ent_name_proxystr;
	m_ent_model		 = ent_modelname_proxystr;
	mh_parent_entity = hEntity;
	m_ent_type		 = int(ENTREC_TYPES::BASE_SKELETAL);
	m_ent_id		 = hEntity.GetSerialNumber();


	// for some reason, the last bone shouldnt be accessable
	// (theres an assert that fails if you try to get it)
	m_ent_numbones = pEntModel->numbones() - 1;

	for (int i = 0; i != m_ent_numbones + 1; i++) {

		char*		boneName	  = new char[strlen(pEntModel->pBone(i)->pszName()) + 1];
		const char* constBoneName = new char[strlen(pEntModel->pBone(i)->pszName()) + 1];

		strcpy_s(boneName, strlen(pEntModel->pBone(i)->pszName()) + 1, pEntModel->pBone(i)->pszName());

		constBoneName = boneName;
		
		mch_bonenames_list.emplace_back(boneName);
	}

	


	DevMsg(3, "KleinWorks: Entity of class %s initialized.\n", pSkelEntity->GetClassname());

}


CzmqBaseSkeletal::~CzmqBaseSkeletal()
{
	CzmqBaseEntity::~CzmqBaseEntity();
}





rapidjson::Value CzmqBaseSkeletal::GetEntityData(rapidjson::MemoryPoolAllocator<> &allocator)
{

#ifdef CLIENT_DLL
	CBaseAnimating* pSkelEntity = cl_entitylist->GetBaseEntityFromHandle(mh_parent_entity)->GetBaseAnimating();
#else
	CBaseAnimating* pSkelEntity = gEntList.GetBaseEntity(mh_parent_entity)->GetBaseAnimating();
#endif // CLIENT_DLL

	CStudioHdr*     pModelPtr   = pSkelEntity->GetModelPtr();

	rapidjson::Value entData_js = rapidjson::Value(rapidjson::kObjectType);

	entData_js.AddMember("ent_id", m_ent_id, allocator);


	// get ang/pos for every bone

	rapidjson::Value bone_pos_js = rapidjson::Value(rapidjson::kObjectType);
	rapidjson::Value bone_rot_js = rapidjson::Value(rapidjson::kObjectType);


	for (int i = 0; i != m_ent_numbones; i++) {

		matrix3x4_t boneToWorld;

		mstudiobone_t* boneSelf = pModelPtr->pBone(i);

		pSkelEntity->GetBoneTransform(i, boneToWorld);


		Vector     bonePosition;
		Quaternion boneQuatAngles;

		MatrixAngles(boneToWorld, boneQuatAngles, bonePosition);


		rapidjson::Value bone_origin_js = rapidjson::Value(rapidjson::kObjectType);
		rapidjson::Value bone_quat_js	= rapidjson::Value(rapidjson::kObjectType);

		bone_origin_js.AddMember("x", bonePosition.x, allocator);
		bone_origin_js.AddMember("y", bonePosition.y, allocator);
		bone_origin_js.AddMember("z", bonePosition.z, allocator);

		bone_quat_js.AddMember("w", boneQuatAngles.w, allocator);
		bone_quat_js.AddMember("x", boneQuatAngles.x, allocator);
		bone_quat_js.AddMember("y", boneQuatAngles.y, allocator);
		bone_quat_js.AddMember("z", boneQuatAngles.z, allocator);

		char* boneName = new char[strlen(boneSelf->pszName()) + 1];
		strcpy_s(boneName, strlen(boneSelf->pszName()) + 1, boneSelf->pszName());

		bone_pos_js.AddMember(rapidjson::StringRef(boneName), bone_origin_js, allocator);
		bone_rot_js.AddMember(rapidjson::StringRef(boneName), bone_quat_js, allocator);

	}


	entData_js.AddMember("ent_pos", bone_pos_js, allocator);
	entData_js.AddMember("ent_rot", bone_rot_js, allocator);


	return entData_js;
}





rapidjson::Value CzmqBaseSkeletal::GetEntityMetaData(rapidjson::MemoryPoolAllocator<> &allocator)
{
	rapidjson::Value entMetaData_js = rapidjson::Value(rapidjson::kObjectType);



	entMetaData_js.AddMember("ent_id",		 m_ent_id, allocator);
	entMetaData_js.AddMember("ent_numbones", m_ent_numbones, allocator);
	entMetaData_js.AddMember("ent_type",	 m_ent_type, allocator);
	entMetaData_js.AddMember("ent_name",	 rapidjson::StringRef(m_ent_name), allocator);
	entMetaData_js.AddMember("ent_model",    rapidjson::StringRef(m_ent_model), allocator);

	rapidjson::Value boneData_js = rapidjson::Value(rapidjson::kArrayType);

	for (int i = 0; i != m_ent_numbones + 1; i++) {
		rapidjson::Value bone_js = rapidjson::Value(rapidjson::kObjectType);
		bone_js.AddMember("name", rapidjson::StringRef(mch_bonenames_list[i]), allocator);

		boneData_js.PushBack(bone_js, allocator);
	}

	entMetaData_js.AddMember("ent_bones", boneData_js, allocator);

	return entMetaData_js;
}
