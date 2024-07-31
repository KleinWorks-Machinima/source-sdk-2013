//===================| PROPERTY OF THE KLEINWORKS™ CORPORTATION®® |===================\\
//
// Purpose: 
// 
// Product Contributors: Barber
//
//===================| PROPERTY OF THE KLEINWORKS™ CORPORTATION®® |===================\\

#include "cbase.h"
#include "czmq_baseskeletal.h"






CzmqBaseSkeletal::CzmqBaseSkeletal(CBaseHandle hEntity)
{
	CBaseAnimating* pSkelEntity = gEntList.GetBaseEntity(hEntity)->GetBaseAnimating();


	mh_parent_entity = hEntity;
	m_ent_type  = int(ENTREC_TYPES::BASE_SKELETAL);

	CStudioHdr* pEntModel = pSkelEntity->GetModelPtr();

	// for some reason, the last bone shouldnt be accessable
	// (theres an assert that fails if you try to get it)
	m_numbones = pEntModel->numbones() - 1;

	for (int i = 0; i != m_numbones + 1; i++) {

		char*		boneName	  = new char[strlen(pEntModel->pBone(i)->pszName()) + 1];
		const char* constBoneName = new char[strlen(pEntModel->pBone(i)->pszName()) + 1];

		strcpy_s(boneName, strlen(pEntModel->pBone(i)->pszName()) + 1, pEntModel->pBone(i)->pszName());

		constBoneName = boneName;
		
		mch_bonenames_list.emplace_back(boneName);
	}
	

	// we have to do this to avoid taking a reference to GetDebugName() or GetModelName()

	int ent_name_len = strlen(pSkelEntity->GetDebugName());
	int ent_modelname_len = strlen(pSkelEntity->GetModelName().ToCStr());


	char* ent_name_proxystr = new char[ent_name_len + 1];
	char* ent_modelname_proxystr = new char[ent_modelname_len + 1];


	strcpy_s(ent_name_proxystr, ent_name_len + 1, pSkelEntity->GetDebugName());
	strcpy_s(ent_modelname_proxystr, ent_modelname_len + 1, pSkelEntity->GetModelName().ToCStr());


	m_ent_name  = ent_name_proxystr;
	m_ent_model = ent_modelname_proxystr;

	


	DevMsg(3, "KleinWorks: Entity of class %s initialized.\n", pSkelEntity->GetClassname());

	gEntList.AddListenerEntity(this);
}


CzmqBaseSkeletal::~CzmqBaseSkeletal()
{
	CzmqBaseEntity::~CzmqBaseEntity();
}





rapidjson::Value CzmqBaseSkeletal::GetEntityData(rapidjson::MemoryPoolAllocator<> &allocator)
{

	CBaseAnimating* pSkelEntity = gEntList.GetBaseEntity(mh_parent_entity)->GetBaseAnimating();

	rapidjson::Value entData_js = rapidjson::Value(rapidjson::kObjectType);

	entData_js.AddMember("ent_name", rapidjson::StringRef(m_ent_name), allocator);


	// get ang/pos for every bone

	CStudioHdr* pModelPtr = pSkelEntity->GetModelPtr();

	CBoneCache* pBoneCache = pSkelEntity->GetBoneCache();

	rapidjson::Value boneData_js = rapidjson::Value(rapidjson::kArrayType);

	for (int i = 0; i != m_numbones + 1; i++) {
		rapidjson::Value bone_js = rapidjson::Value(rapidjson::kObjectType);


		matrix3x4_t* boneToWorld = pBoneCache->GetCachedBone(i);

		matrix3x4_t worldToBone;
		matrix3x4_t local;

		Vector bonePosition;
		QAngle boneAngles;
		Quaternion boneQuatAngles;

		// converting worldspace to bonespace if this isn't the rootbone
		if (i == 0) {

			MatrixPosition(worldToBone, bonePosition);

			MatrixToAngles(worldToBone, boneAngles);

			AngleQuaternion(boneAngles, boneQuatAngles);
		}
		else {

			int iParent = pModelPtr->boneParent(i);

			MatrixInvert(boneToWorld[iParent], worldToBone);
			ConcatTransforms(worldToBone, boneToWorld[i], local);

			MatrixPosition(local, bonePosition);

			MatrixToAngles(local, boneAngles);

			AngleQuaternion(boneAngles, boneQuatAngles);
		}



		const char* bonePosStr     = static_cast<const char *>(CFmtStr("(%f, %f, %f)",     bonePosition.x, bonePosition.y, bonePosition.z));
		const char* boneQuatAngStr = static_cast<const char *>(CFmtStr("(%f, %f, %f, %f)", boneQuatAngles.w, boneQuatAngles.x, boneQuatAngles.y, boneQuatAngles.z));

		// have to do this little maneuver to avoid taking a reference of the strings

		char* bonePos_proxystr     = new char[strlen(bonePosStr)     + 1];
		char* boneQuatAng_proxystr = new char[strlen(boneQuatAngStr) + 1];


		strcpy_s(bonePos_proxystr,     strlen(bonePos_proxystr), bonePosStr);
		strcpy_s(boneQuatAng_proxystr, strlen(boneQuatAngStr),   boneQuatAngStr);

		bone_js.AddMember("POS", rapidjson::StringRef(bonePos_proxystr), allocator);
		bone_js.AddMember("ROT", rapidjson::StringRef(boneQuatAng_proxystr), allocator);

		boneData_js.PushBack(bone_js, allocator);
	}


	entData_js.AddMember("bonedata", boneData_js, allocator);


	return entData_js;
}





rapidjson::Value CzmqBaseSkeletal::GetEntityMetaData(rapidjson::MemoryPoolAllocator<> &allocator)
{
	rapidjson::Value entMetaData_js = rapidjson::Value(rapidjson::kObjectType);

	entMetaData_js.AddMember("ent_name", rapidjson::StringRef(m_ent_name), allocator);
	entMetaData_js.AddMember("ent_type", m_ent_type, allocator);
	entMetaData_js.AddMember("ent_modelpath", rapidjson::StringRef(m_ent_model), allocator);

	rapidjson::Value boneData_js = rapidjson::Value(rapidjson::kArrayType);

	for (int i = 0; i != m_numbones + 1; i++) {
		rapidjson::Value bone_js = rapidjson::Value(rapidjson::kObjectType);
		bone_js.AddMember("name", rapidjson::StringRef(mch_bonenames_list[i]), allocator);

		boneData_js.PushBack(bone_js, allocator);
	}

	entMetaData_js.AddMember("bonedata", boneData_js, allocator);

	return entMetaData_js;
}

