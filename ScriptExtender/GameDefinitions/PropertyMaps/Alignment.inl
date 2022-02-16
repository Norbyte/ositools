BEGIN_CLS(esv::HasRelationsObject)
P_RO(Handle)
P_RO(Name)
P_RO(NetID)
P_REF(TemporaryRelations2)
P_REF(TemporaryRelations)
END_CLS()


BEGIN_CLS(esv::AlignmentEntity)
INHERIT(esv::HasRelationsObject)
END_CLS()


BEGIN_CLS(esv::Alignment)
INHERIT(esv::HasRelationsObject)
P_RO(MatrixIndex2)
P_REF(Entities)
P_RO(MatrixIndex)
END_CLS()


BEGIN_CLS(esv::AlignmentData)
P_REF(ParentAlignment)
P_RO(HasOwnAlignment)
P_REF(Alignment)
P_RO(Handle)
P_RO(Name)
P_RO(NetID)
P_RO(MatrixIndex)
END_CLS()


BEGIN_CLS(esv::AlignmentContainer)
P_FUN(IsPermanentEnemy, IsPermanentEnemy)
P_FUN(IsTemporaryEnemy, IsTemporaryEnemy)
P_FUN(SetPermanentEnemy, SetPermanentEnemy)
P_FUN(SetTemporaryEnemy, SetTemporaryEnemy)
P_FUN(SetAlly, SetAlly)
P_FUN(Get, LuaGet)
P_FUN(GetAll, LuaGetAll)
END_CLS()
