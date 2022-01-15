BEGIN_CLS(BaseComponent)
// FIXME - entity handles not supported yet!
// P_RO(Handle)
END_CLS()


BEGIN_CLS(IGameObject)
P_REF(Base)
P_RO(MyGuid)
P_RO(NetID)

P_GETTER(Handle, LuaGetHandle)
// FIXME - re-add after entity proxies are done
// P_GETTER(EntityHandle, LuaGetEntityHandle)
P_GETTER_SETTER(Translate, LuaGetTranslate, LuaSetTranslate)
P_GETTER_SETTER(Scale, LuaGetScale, LuaSetScale)
P_GETTER(Velocity, LuaGetVelocity)
P_GETTER(Height, LuaGetHeight)
P_GETTER(Visual, LuaGetVisual)

P_FUN(IsTagged, LuaIsTagged)
P_FUN(HasTag, LuaIsTagged)
P_FUN(GetTags, LuaGetTags)
END_CLS()

BEGIN_CLS(IEoCServerObject)
INHERIT(IGameObject)
P_GETTER(DisplayName, LuaGetDisplayName)
P_FUN(GetStatus, LuaGetStatus)
P_FUN(GetStatusByType, LuaGetStatusByType)
P_FUN(GetStatusByHandle, LuaGetStatusByHandle)
P_FUN(GetStatuses, LuaGetStatusIds)
P_FUN(GetStatusObjects, LuaGetStatuses)
END_CLS()

BEGIN_CLS(IEoCClientObject)
INHERIT(IGameObject)
P_GETTER(DisplayName, LuaGetDisplayName)
P_FUN(GetStatus, LuaGetStatus)
P_FUN(GetStatusByType, LuaGetStatusByType)
P_FUN(GetStatuses, LuaGetStatusIds)
P_FUN(GetStatusObjects, LuaGetStatuses)
END_CLS()
