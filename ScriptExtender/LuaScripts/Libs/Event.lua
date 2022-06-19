local _I = Ext._Internal

local SubscribableEvent = {}

function SubscribableEvent:New(name)
	local o = {
		First = nil,
		NextIndex = 1,
		Name = name
	}
	setmetatable(o, self)
    self.__index = self
    return o
end

function SubscribableEvent:Subscribe(handler, opts)
	opts = opts or {}
	local index = self.NextIndex
	self.NextIndex = self.NextIndex + 1

	local sub = {
		Handler = handler,
		Index = index,
		Priority = opts.Priority or 100,
		Once = opts.Once or false,
		Options = opts
	}

	self:DoSubscribe(sub)
	return index
end

function SubscribableEvent:DoSubscribeBefore(node, sub)
	sub.Prev = node.Prev
	sub.Next = node
	
	if node.Prev ~= nil then
		node.Prev.Next = sub
	else
		self.First = sub
	end

	node.Prev = sub
end

function SubscribableEvent:DoSubscribe(sub)
	if self.First == nil then 
		self.First = sub
		return
	end

	local cur = self.First
	local last

	while cur ~= nil do
		last = cur
		if sub.Priority > cur.Priority then
			self:DoSubscribeBefore(cur, sub)
			return
		end

		cur = cur.Next
	end

	last.Next = sub
	sub.Prev = last
end

function SubscribableEvent:RemoveNode(node)
	if node.Prev ~= nil then
		node.Prev.Next = node.Next
	end

	if node.Next ~= nil then
		node.Next.Prev = node.Prev
	end

	if self.First == node then
		self.First = node.Next
	end

	node.Prev = nil
	node.Next = nil
end

function SubscribableEvent:Unsubscribe(handlerIndex)
	local cur = self.First
	while cur ~= nil do
		if cur.Index == handlerIndex then
			self:RemoveNode(cur)
			return true
		end

		cur = cur.Next
	end
	
	Ext.PrintWarning("Attempted to remove subscriber ID " .. handlerIndex .. " for event '" .. self.Name .. "', but no such subscriber exists (maybe it was removed already?)")
	return false
end

function SubscribableEvent:Throw(event)
	local cur = self.First
	while cur ~= nil do
		if event.Stopped then
			break
		end

        local ok, result = xpcall(cur.Handler, debug.traceback, event)
        if not ok then
            Ext.PrintError("Error while dispatching event " .. self.Name .. ": ", result)
        end

		if cur.Once then
			local last = cur
			cur = last.Next
			self:RemoveNode(last)
		else
			cur = cur.Next
		end
	end
end

local MissingSubscribableEvent = {}

function MissingSubscribableEvent:New(name)
	local o = {
		Name = name
	}
	setmetatable(o, self)
    self.__index = self
    return o
end

function MissingSubscribableEvent:Subscribe(handler, opts)
    Ext.PrintError("Attempted to subscribe to nonexistent event: " .. self.Name)
end

function MissingSubscribableEvent:Unsubscribe(handlerIndex)
    Ext.PrintError("Attempted to unsubscribe from nonexistent event: " .. self.Name)
end

function MissingSubscribableEvent:Throw(event)
    Ext.PrintError("Attempted to throw nonexistent event: " .. self.Name)
end

_I._Events = {}

Ext.Events = {}
setmetatable(Ext.Events, {
	__index = function (self, event)
		return _I._Events[event] or MissingSubscribableEvent:New(event)
	end,

	__newindex = function (self, k, v)
		error("Cannot write to Ext.Events directly!")
	end
})

_I._ThrowEvent = function (event)
	Ext.Events[event.Name]:Throw(event)
end

_I._RegisterEngineEvent = function (event)
	_I._Events[event] = SubscribableEvent:New(event)
end

_I._MakeLegacyHitEvent = function (hit)
	local t = {
		HitId = hit.Id,
		HitStatus = hit.Status
	}

	if (hit.CapturedCharacterHit) then
		t.Weapon = hit.WeaponStats
		t.HitType = hit.HitType
		t.NoHitRoll = hit.NoHitRoll
		t.ProcWindWalker = hit.ProcWindWalker
		t.ForceReduceDurability = hit.ForceReduceDurability
		t.HighGround = hit.HighGround
		t.CriticalRoll = hit.CriticalRoll
		t.Hit = _I._MakeLegacyHitInfo(hit.CharacterHit)
	end

	return t
end

_I._MakeLegacyHitStatusProxy = function (hit)
	local proxy = {}
	setmetatable(proxy, {
		__index = function (obj, attr)
			if attr == "Hit" then
				return _I._MakeLegacyHitInfo(hit.Hit)
			else
				return hit[attr]
			end
		end,
		__newindex = function (obj, attr, val)
			if attr == "Hit" then
				_I._UpdateHitFromTable(hit.Hit, val)
			else
				hit[attr] = val
			end
		end
	})
	return proxy
end

-- Hit flag to EffectFlags value mappings
local HitFlagMap = {
    Hit = 1,
    Blocked = 2,
    Dodged = 4,
    Missed = 8,
    CriticalHit = 0x10,
    Backstab = 0x20,
    FromSetHP = 0x40,
    DontCreateBloodSurface = 0x80,
    Reflection = 0x200,
    NoDamageOnOwner = 0x400,
    FromShacklesOfPain = 0x800,
    DamagedMagicArmor = 0x1000,
    DamagedPhysicalArmor = 0x2000,
    DamagedVitality = 0x4000,
    Flanking = 0x8000,
    PropagatedFromOwner = 0x10000,
    Surface = 0x20000,
    DoT = 0x40000,
    ProcWindWalker = 0x80000,
    CounterAttack = 0x100000,
    Poisoned = 0x200000,
    Burning = 0x400000,
    Bleeding = 0x800000,
    NoEvents = 0x80000000
}

_I._MakeLegacyHitInfo = function (hit)
	local damageList = Ext.NewDamageList()
	damageList:CopyFrom(hit.DamageList)
	local t = {
		Equipment = hit.Equipment,
		TotalDamageDone = hit.TotalDamageDone,
		DamageDealt = hit.DamageDealt,
		DeathType = hit.DeathType,
		DamageType = hit.DamageType,
		AttackDirection = hit.AttackDirection,
		ArmorAbsorption = hit.ArmorAbsorption,
		LifeSteal = hit.LifeSteal,
		EffectFlags = hit.EffectFlags,
		HitWithWeapon = hit.HitWithWeapon,
		DamageList = damageList
	}
	local proxy = {}
	setmetatable(proxy, {
		__index = function (obj, attr)
			if HitFlagMap[attr] ~= nil then
				return (t.EffectFlags & HitFlagMap[attr]) ~= 0
			else
				return t[attr]
			end
		end,
		__newindex = function (obj, attr, val)
			if HitFlagMap[attr] ~= nil then
				if val then
					t.EffectFlags = t.EffectFlags | HitFlagMap[attr]
				else
					t.EffectFlags = t.EffectFlags & ~HitFlagMap[attr]
				end
			else
				t[attr] = val
			end
		end
	})
	return proxy
end

_I._UpdateHitFromTable = function (hit, t)
	hit.Equipment = t.Equipment
	hit.TotalDamageDone = t.TotalDamageDone
	hit.DamageDealt = t.DamageDealt
	hit.DeathType = t.DeathType
	hit.DamageType = t.DamageType
	hit.AttackDirection = t.AttackDirection
	hit.ArmorAbsorption = t.ArmorAbsorption
	hit.LifeSteal = t.LifeSteal
	hit.EffectFlags = t.EffectFlags
	hit.HitWithWeapon = t.HitWithWeapon
	hit.DamageList:CopyFrom(t.DamageList)
end

_I._CallLegacyEvent = function (fn, event)
	if event.Name == "GameStateChanged" then
		fn(event.FromState, event.ToState)
	elseif event.Name == "UIObjectCreated" then
		fn(event.UI)
	elseif event.Name == "UICall" then
		fn(event.UI, event.Function, event.When, table.unpack(event.Args))
	elseif event.Name == "UIInvoke" then
		if event.When == "Before" then
			fn(event.UI, event.Function, event.When, table.unpack(event.Args))
		end
	elseif event.Name == "SkillGetDescriptionParam" then
		local desc = fn(event.Skill, event.Character, event.IsFromItem, table.unpack(event.Params))
		if desc ~= nil then
			event.Description = desc
			event:StopPropagation()
		end
	elseif event.Name == "StatusGetDescriptionParam" then
		local desc = fn(event.Status, event.StatusSource, event.Owner, table.unpack(event.Params))
		if desc ~= nil then
			event.Description = desc
			event:StopPropagation()
		end
	elseif event.Name == "SkillGetPropertyDescription" then
		local desc = fn(event.Property)
		if desc ~= nil then
			event.Description = desc
			event:StopPropagation()
		end
	elseif event.Name == "InputEvent" then
		fn(event.Event)
	elseif event.Name == "StatusGetEnterChance" then
		local chance = fn(event.Status, event.IsEnterCheck)
		if chance ~= nil then
			event.EnterChance = chance
			event:StopPropagation()
		end
	elseif event.Name == "StatusHitEnter" then
		fn(_I._MakeLegacyHitStatusProxy(event.Hit), _I._MakeLegacyHitEvent(event.Context))
	elseif event.Name == "ComputeCharacterHit" then
		local hit = event.Hit
		local legacyHit = _I._MakeLegacyHitInfo(hit)
		local hitResult = fn(event.Target, event.Attacker, event.Weapon, event.DamageList, event.HitType, event.NoHitRoll,
			event.ForceReduceDurability, legacyHit, event.AlwaysBackstab, event.HighGround, event.CriticalRoll)
		if hitResult ~= nil then
			_I._UpdateHitFromTable(hit, hitResult)
			event.Handled = true
			event:StopPropagation()
		end
	elseif event.Name == "GetSkillDamage" then
		local dmg, deathType = fn(event.Skill, event.Attacker, event.IsFromItem, event.Stealthed, event.AttackerPosition, event.TargetPosition, event.Level, 
			event.NoRandomization)
		if deathType ~= nil and dmg ~= nil then
			event.DamageList:CopyFrom(dmg)
			event.DeathType = deathType
			event:StopPropagation()
		end
	elseif event.Name == "GetSkillAPCost" then
		local ap, elementalAffinity = fn(event.Skill, event.Character, event.AiGrid, event.Position, event.Radius)
		if ap ~= nil and elementalAffinity ~= nil then
			event.AP = ap
			event.ElementalAffinity = elementalAffinity
			event:StopPropagation()
		end
	elseif event.Name == "BeforeCharacterApplyDamage" then
		fn(event.Target, event.Attacker, event.Hit, event.Cause, event.ImpactDirection, _I._MakeLegacyHitEvent(event.Context))
	elseif event.Name == "TreasureItemGenerated" then
		local resultItem = fn(event.Item)
		if resultItem ~= nil then
			event.ResultingItem = resultItem
			event:StopPropagation()
		end
	elseif event.Name == "BeforeCraftingExecuteCombination" then
		local processed = fn(event.Character, event.CraftingStation, event.Items, event.Quantity, event.CombinationId)
		if processed == true then
			event.Processed = true
			event:StopPropagation()
		end
	elseif event.Name == "AfterCraftingExecuteCombination" then
		local processed = fn(event.Character, event.CraftingStation, event.Items, event.Quantity, event.CombinationId, event.Succeeded)
		if processed == true then
			event.Processed = true
			event:StopPropagation()
		end
	elseif event.Name == "BeforeShootProjectile" then
		fn(event.Projectile)
	elseif event.Name == "ShootProjectile" then
		fn(event.Projectile)
	elseif event.Name == "ProjectileHit" then
		fn(event.Projectile, event.HitObject, event.Position)
	elseif event.Name == "GroundHit" then
		fn(event.Caster, event.Position, event.DamageList)
	elseif event.Name == "OnExecutePropertyDataOnTarget" then
		fn(event.Property, event.Attacker, event.Target, event.ImpactOrigin, event.IsFromItem, event.Skill, event.Hit)
	elseif event.Name == "OnExecutePropertyDataOnPosition" then
		fn(event.Property, event.Attacker, event.Position, event.AreaRadius, event.IsFromItem, event.Skill, event.Hit)
	elseif event.Name == "InputEvent" then
		fn(event.InputEvent)
	elseif event.Name == "GetHitChance" then
		local chance = fn(event.Attacker, event.Target)
		if chance ~= nil then
			event.HitChance = chance
			event:StopPropagation()
		end
	else
		fn()
	end
end

Ext.RegisterListener = function (evt, fn)
	Ext._WarnDeprecated56("Ext.RegisterListener(" .. evt .. ") is deprecated! Use Ext.Events." .. evt .. ":Subscribe() instead!", "RegisterListener")

	Ext.Events[evt]:Subscribe(function (event) 
		_I._CallLegacyEvent(fn, event)
	end)
end

_I._RegisterEvents = function ()
	_I._RegisterEngineEvent("NetMessageReceived")
	
	for i,ev in pairs(_I._PublishedSharedEvents) do
		_I._RegisterEngineEvent(ev)
	end
	
	for i,ev in pairs(_I._PublishedEvents) do
		_I._RegisterEngineEvent(ev)
	end

	-- Support for Ext.RegisterConsoleCommand()
	Ext.Events.DoConsoleCommand:Subscribe(function (e)
		_I.DoConsoleCommand(e.Command)
	end)

	-- Support for Ext.RegisterNetListener()
	Ext.Events.NetMessageReceived:Subscribe(function (e)
		_I._NetMessageReceived(e.Channel, e.Payload, e.UserID)
	end)
end

_I._NetListeners = {}

Ext.RegisterNetListener = function (channel, fn)
	if _I._NetListeners[channel] == nil then
		_I._NetListeners[channel] = {}
	end

	table.insert(_I._NetListeners[channel], fn)
end

_I._NetMessageReceived = function (channel, payload, userId)
	if _I._NetListeners[channel] ~= nil then
		for i,callback in pairs(_I._NetListeners[channel]) do
			local ok, err = xpcall(callback, debug.traceback, channel, payload, userId)
			if not ok then
				Ext.PrintError("Error during NetMessageReceived: ", err)
			end
		end
	end
end

_I._ConsoleCommandListeners = {}

_I.DoConsoleCommand = function (cmd)
	local params = {}
	for param in string.gmatch(cmd, "%S+") do
		table.insert(params, param)
	end

	local listeners = _I._ConsoleCommandListeners[params[1]]
	if listeners ~= nil then
		for i,callback in pairs(listeners) do
			local status, result = xpcall(callback, debug.traceback, table.unpack(params))
			if not status then
				Ext.PrintError("Error during console command callback: ", result)
			end
		end
	end
end

Ext.RegisterConsoleCommand = function (cmd, fn)
	if _I._ConsoleCommandListeners[cmd] == nil then
		_I._ConsoleCommandListeners[cmd] = {}
	end

	table.insert(_I._ConsoleCommandListeners[cmd], fn)
end
