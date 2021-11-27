BEGIN_CLS(eoc::ItemDefinition)
P(RootTemplate)
P(OriginalRootTemplate)
P(Slot)
P(Amount)
P(GoldValueOverwrite)
P(WeightValueOverwrite)
P(DamageTypeOverwrite)
P(ItemType)
P(CustomDisplayName)
P(CustomDescription)
P(CustomBookContent)
P(GenerationStatsId)
P(GenerationItemType)
P(GenerationRandom)
P(GenerationLevel)
P(StatsLevel)
P(Key)
P(LockLevel)
P(StatsEntryName)
P(EquipmentStatsType)
P(HasModifiedSkills)
P(Skills)
P(HasGeneratedStats)
P(IsIdentified)
P(GMFolding)
P(CanUseRemotely)
END_CLS()


BEGIN_CLS(esv::Item)
INHERIT(IEoCServerObject)
P_RO(WorldPos)
P(Flags);
P(Flags2);
/*
// Make dangerous flags read-only
propertyMap.Flags[GFS.strActivated].Flags &= ~kPropWrite;
propertyMap.Flags[GFS.strOffStage].Flags &= ~kPropWrite;
propertyMap.Flags[GFS.strDestroyed].Flags &= ~kPropWrite;
propertyMap.Flags[GFS.strGlobal].Flags &= ~kPropWrite;

EnumInfo<esv::ItemFlags>::Values.Iterate([&propertyMap](auto const& name, auto const& id) {
	auto& flag = propertyMap.Flags[name];
	if (flag.Flags & kPropWrite) {
		flag.Set = [id](void* obj, bool value) -> bool {
			auto ch = reinterpret_cast<esv::Item*>(obj);
			if (value) {
				ch->SetFlags((uint64_t)id);
			}
			else {
				ch->ClearFlags((uint64_t)id);
			}
			return true;
		};
	}
	});
	*/
P_RO(CurrentLevel)
P_RO(Scale)
P(CustomDisplayName)
P(CustomDescription)
P(CustomBookContent)
P(StatsId)
P_RO(InventoryHandle)
P_RO(ParentInventoryHandle)
P_RO(Slot)
P(Amount)
P(Vitality)
P(Armor)
P_RO(InUseByCharacterHandle)
P(Key)
P(LockLevel)
P_RO(OwnerHandle)
P(ComputedVitality)
P(ItemType)
P(GoldValueOverwrite)
P(WeightValueOverwrite)
P(TreasureLevel)
P(LevelOverride)
P_RO(ForceSynch)
END_CLS()


BEGIN_CLS(ecl::Item)
// EoCClientObject
P_RO(NetID)
P_RO(MyGuid)
// Item
P_RO(WorldPos)
P_RO(CurrentLevel)
P_RO(Scale)
P_RO(StatsId)
P_RO(Weight)
P_RO(KeyName)
P_RO(Level)
P_RO(ItemType)
P_RO(GoldValueOverride)
P_RO(BaseWeightOverwrite)
P_RO(ItemColorOverride)
END_CLS()
