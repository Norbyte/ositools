BEGIN_CLS(PhysicsShape)
P_RO(Name)
P_RO(Translate)
P_RO(Rotate)
P_RO(Scale)
END_CLS()


BEGIN_CLS(PhysicsScene)
P_REF(Objects)
END_CLS()


BEGIN_CLS(PhysicsObject)
P_RO(TemplateID)
P_REF(PhysicsScene)
P_REF(GameObject)
P_REF(Shapes)
P_RO(Translate)
P_RO(Rotate)
P_RO(Scale)
P_RO(PhysicsGroups)
P_RO(CollidesWith)
P_RO(ManagedScale)
END_CLS()


BEGIN_CLS(PhysicsRagdoll)
// TODO
END_CLS()
