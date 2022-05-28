-- Backwards compatibility with old JSON APIs
Ext.JsonStringify = Ext.Json.Stringify
Ext.JsonParse = Ext.Json.Parse

-- Backwards compatibility with old debug APIs
Ext.DebugBreak = Ext.Debug.DebugBreak
Ext.IsDeveloperMode = Ext.Debug.IsDeveloperMode

-- Backwards compatibility with old utility APIs
Ext.Print = Ext.Utils.Print
Ext.PrintWarning = Ext.Utils.PrintWarning
Ext.PrintError = Ext.Utils.PrintError
Ext.GetGameMode = Ext.Utils.GetGameMode
Ext.GetDifficulty = Ext.Utils.GetDifficulty
Ext.Random = Ext.Utils.Random
Ext.Round = Ext.Utils.Round

Ext.MonotonicTime = Ext.Utils.MonotonicTime
Ext.Version = Ext.Utils.Version
Ext.GameVersion = Ext.Utils.GameVersion
Ext.GetHandleType = Ext.Utils.GetHandleType
Ext.ShowErrorAndExitGame = Ext.Utils.ShowErrorAndExitGame

-- Backwards compatibility with old IO APIs
Ext.LoadFile = Ext.IO.LoadFile
Ext.SaveFile = Ext.IO.SaveFile
Ext.AddPathOverride = Ext.IO.AddPathOverride
Ext.GetPathOverride = Ext.IO.GetPathOverride

-- Backwards compatibility with old Mod APIs
Ext.IsModLoaded = Ext.Mod.IsModLoaded
Ext.GetModLoadOrder = Ext.Mod.GetLoadOrder
Ext.GetModInfo = Ext.Mod.GetModInfo

-- Backwards compatibility with old localization APIs
Ext.GetTranslatedString = Ext.L10N.GetTranslatedString
Ext.GetTranslatedStringFromKey = Ext.L10N.GetTranslatedStringFromKey
Ext.CreateTranslatedString = Ext.L10N.CreateTranslatedString
Ext.CreateTranslatedStringKey = Ext.L10N.CreateTranslatedStringKey
Ext.CreateTranslatedStringHandle = Ext.L10N.CreateTranslatedStringHandle

-- Backwards compatibility with old stat APIs
Ext.GetStatEntries = Ext.Stats.GetStats
Ext.GetStatEntriesLoadedBefore = Ext.Stats.GetStatsLoadedBefore
Ext.GetStat = Ext.Stats.Get
Ext.CreateStat = Ext.Stats.Create
Ext.SyncStat = Ext.Stats.Sync
Ext.StatSetPersistence = Ext.Stats.SetPersistence
Ext.StatGetAttribute = Ext.Stats.GetAttribute
Ext.StatSetAttribute = Ext.Stats.SetAttribute
Ext.StatAddCustomDescription = Ext.Stats.AddCustomDescription
Ext.StatSetLevelScaling = Ext.Stats.SetLevelScaling
Ext.EnumIndexToLabel = Ext.Stats.EnumIndexToLabel
Ext.EnumLabelToIndex = Ext.Stats.EnumLabelToIndex

Ext.GetDeltaMod = Ext.Stats.DeltaMod.GetLegacy
Ext.UpdateDeltaMod = Ext.Stats.DeltaMod.Update
Ext.GetSkillSet = Ext.Stats.SkillSet.GetLegacy
Ext.UpdateSkillSet = Ext.Stats.SkillSet.Update
Ext.GetEquipmentSet = Ext.Stats.EquipmentSet.GetLegacy
Ext.UpdateEquipmentSet = Ext.Stats.EquipmentSet.Update
Ext.GetTreasureTable = Ext.Stats.TreasureTable.GetLegacy
Ext.UpdateTreasureTable = Ext.Stats.TreasureTable.Update
Ext.GetTreasureCategory = Ext.Stats.TreasureCategory.GetLegacy
Ext.UpdateTreasureCategory = Ext.Stats.TreasureCategory.Update
Ext.GetItemCombo = Ext.Stats.ItemCombo.GetLegacy
Ext.UpdateItemCombo = Ext.Stats.ItemCombo.Update
Ext.GetItemComboPreviewData = Ext.Stats.ItemComboPreview.GetLegacy
Ext.UpdateItemComboPreviewData = Ext.Stats.ItemComboPreview.Update
Ext.GetItemComboProperty = Ext.Stats.ItemComboPreview.GetLegacy
Ext.UpdateItemComboProperty = Ext.Stats.ItemComboPreview.Update
Ext.GetItemGroup = Ext.Stats.ItemGroup.GetLegacy
Ext.GetNameGroup = Ext.Stats.NameGroup.Update

-- Backwards compatibility with old surface APIs
Ext.GetSurfaceTransformRules = Ext.Surface.GetTransformRules
Ext.UpdateSurfaceTransformRules = Ext.Surface.UpdateTransformRules
Ext.GetSurfaceTemplate = Ext.Surface.GetTemplate

-- Backwards compatibility with old UI APIs
Ext.CreateUI = Ext.UI.Create
Ext.GetUI = Ext.UI.GetByName
Ext.GetUIByType = Ext.UI.GetByType
Ext.GetBuiltinUI = Ext.UI.GetByPath
Ext.DestroyUI = Ext.UI.Destroy
Ext.UISetDirty = Ext.UI.SetDirty
Ext.UIEnableCustomDrawCallDebugging = Ext.UI.EnableCustomDrawCallDebugging
Ext.HandleToDouble = Ext.UI.HandleToDouble
Ext.DoubleToHandle = Ext.UI.DoubleToHandle

-- Backwards compatibility with old Net APIs
Ext.PostMessageToServer = Ext.Net.PostMessageToServer

-- Backwards compatibility with old Osiris APIs
Ext.RegisterOsirisListener = Ext.Osiris.RegisterListener
Ext.OsirisIsCallable = Ext.Osiris.IsCallable
Ext.NewCall = Ext.Osiris.NewCall
Ext.NewQuery = Ext.Osiris.NewQuery
Ext.NewEvent = Ext.Osiris.NewEvent

-- Backwards compatibility with old Net APIs
Ext.BroadcastMessage = Ext.Net.BroadcastMessage
Ext.PostMessageToClient = Ext.Net.PostMessageToClient
Ext.PostMessageToUser = Ext.Net.PostMessageToUser
Ext.PlayerHasExtender = Ext.Net.PlayerHasExtender

-- Backwards compatibility with old custom stat APIs
Ext.CreateCustomStat = Ext.CustomStat.Create
Ext.GetCustomStatById = Ext.CustomStat.GetById
Ext.GetCustomStatByName = Ext.CustomStat.GetByName
Ext.GetAllCustomStats = Ext.CustomStat.GetAll

-- Backwards compatibility with old surface APIs
Ext.CreateSurfaceAction = Ext.Surface.Action.Create
Ext.ExecuteSurfaceAction = Ext.Surface.Action.Execute
Ext.CancelSurfaceAction = Ext.Surface.Action.Cancel
