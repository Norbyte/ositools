#pragma once

#include <GameDefinitions/Base/Base.h>
#include <GameDefinitions/Enumerations.h>
#include <GameDefinitions/EntitySystem.h>

namespace dse
{
	namespace esv
	{
		struct HasRelationsObject
		{
			void* VMT;
			ComponentHandle Handle;
			FixedString Name;
			NetId NetID;
			Map<ComponentHandle, int> TemporaryRelations2;
			Map<ComponentHandle, int> TemporaryRelations;
		};


		struct AlignmentEntity : public HasRelationsObject
		{
			static constexpr auto ObjectTypeIndex = ObjectHandleType::Unknown;
		};


		struct Alignment : public HasRelationsObject
		{
			static constexpr auto ObjectTypeIndex = ObjectHandleType::Unknown;

			int MatrixIndex2;
			ObjectSet<AlignmentEntity*> Entities;
			int MatrixIndex;
		};


		struct AlignmentData
		{
			static constexpr auto ObjectTypeIndex = ObjectHandleType::Unknown;

			Alignment* ParentAlignment;
			bool HasOwnAlignment;
			Alignment* Alignment;
			ComponentHandle Handle;
			FixedString Name;
			NetId NetID;
			int MatrixIndex;
		};


		struct AlignmentContainer : public NetworkComponentFactory<AlignmentData>
		{
#if !defined(OSI_EOCAPP)
			struct SEntity
			{
				FixedString Alignment;
				FixedString Name;
			};

			struct SRelation
			{
				FixedString Source;
				FixedString Target;
				int Value;
			};
#endif

			void* VisitorVMT;
			int MatrixSize;
			BitMatrix* PermanentEnemyMatrix;
			BitMatrix* TemporaryEnemyMatrix;
			BitMatrix* AllyMatrix;
#if !defined(OSI_EOCAPP)
			Guid CurrentModuleUUID;
			RefMap<Guid, ObjectSet<FixedString>> Alignments;
			RefMap<Guid, ObjectSet<SEntity>> Entities;
			RefMap<Guid, ObjectSet<SRelation>> Relations;
			bool IsLoading;
#endif

			bool IsPermanentEnemy(ComponentHandle source, ComponentHandle target);
			bool IsTemporaryEnemy(ComponentHandle source, ComponentHandle target);
			bool IsAlly(ComponentHandle source, ComponentHandle target);

			void SetPermanentEnemy(ComponentHandle source, ComponentHandle target, bool enabled);
			void SetTemporaryEnemy(ComponentHandle source, ComponentHandle target, bool enabled);
			void SetAlly(ComponentHandle source, ComponentHandle target, bool enabled);

			AlignmentData* LuaGet(lua_State* L);
			ObjectSet<ComponentHandle> LuaGetAll();
		};

	}
}
