BEGIN_CLS(esv::BaseController)
P_RO(TypeId)
P_RO(CharacterHandle)
END_CLS()

BEGIN_CLS(esv::StatusController)
INHERIT(esv::BaseController)
P(Flags)
P_RO(SummoningTransactionId)
P_RO(PolymorphingTransactionId)
P_RO(ActionTransactionId)
P_RO(TeleportFallingTransactionId)
P_RO(DeathAnimationTransactionId)
P_RO(SteerToEnemyTransactionId)
P_RO(Flags2)
P(CombatStartPosition)
P(KnockDownQueued)
P(CombatStartPositionFloodDone)
P(ResurrectedEvent)
END_CLS()
