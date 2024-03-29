BEGIN_CLS(ModuleInfo)
P_RO(ModuleUUID)
P(Name)
P(StartLevel)
P(MenuLevel)
P(LobbyLevel)
P(CharacterCreationLevel)
P(PhotoBoothLevel)
P_RO(ModVersion)
P_RO(PublishVersion)
P(Hash)
P_RO(Directory)
P(Author)
P(Description)
P_REF(Tags)
P(NumPlayers)
P(GMTemplate)
P_REF(TargetModes)
P_RO(ModuleType)
P_REF(DisplayName)
P_REF(DisplayDescription)
END_CLS()

BEGIN_CLS(Module)
P_REF(Info)
P_REF(LoadOrderedModules)
P_REF(ContainedModules)
P_REF(DependentModules)
P_REF(AddonModules)
P_RO(HasValidHash)
P_RO(UsesLsfFormat)
P_RO(FinishedLoading)
P_RO(BFSReset)
END_CLS()

BEGIN_CLS(ModuleShortDesc)
P_RO(ModuleUUID)
P_RO(Name)
P_RO(Version)
P_RO(MD5)
P_RO(Folder)
END_CLS()

BEGIN_CLS(ModuleSettings)
P_REF(Mods)
P_REF(ModOrder)
END_CLS()

BEGIN_CLS(ModManager)
P_REF(BaseModule)
P_RO(Flag)
P_REF(AvailableMods)
P_REF(Settings)
END_CLS()
