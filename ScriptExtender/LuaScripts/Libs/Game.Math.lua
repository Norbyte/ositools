local math = math
local table = table
local error = error
local pairs = pairs
local Ext = Ext
local Osi = Osi

Game = {
    Math = {}
}

_ENV = Game.Math
if setfenv ~= nil then
    setfenv(1, Game.Math)
end

--- @class HitCalculationContext
--- @field DamageMultiplier number

DamageTypeToDeathTypeMap = {
    Physical = "Physical",
    Piercing = "Piercing",
    Fire = "Incinerate",
    Air = "Electrocution",
    Water = "FrozenShatter",
    Earth = "PetrifiedShatter",
    Poison = "Acid",
    Sulfuric = "Sulfur"
}

--- @param damageType string DamageType enumeration
--- @return string
function DamageTypeToDeathType(damageType)
    local deathType = DamageTypeToDeathTypeMap[damageType]
    if deathType ~= nil then
        return deathType
    else
        return "DoT"
    end
end

--- @param item CDivinityStatsItem
function IsRangedWeapon(item)
    local type = item.WeaponType
    return type == "Bow" or type == "Crossbow" or type == "Wand" or type == "Rifle"
end

--- @param primaryAttr integer
function ScaledDamageFromPrimaryAttribute(primaryAttr)
    return (primaryAttr - Ext.ExtraData.AttributeBaseValue) * Ext.ExtraData.DamageBoostFromAttribute
end

--- @param skill StatEntrySkillData
--- @param character CDivinityStatsCharacter
function GetPrimaryAttributeAmount(skill, character)
    if skill.UseWeaponDamage == "Yes" and character.MainWeapon ~= nil then
        local main = character.MainWeapon
        local offHand = character.OffHandWeapon
        if offHand ~= nil and IsRangedWeapon(main) == IsRangedWeapon(offHand) then
            return (GetItemRequirementAttribute(character, main) + GetItemRequirementAttribute(character, offHand)) * 0.5
        else
            return GetItemRequirementAttribute(character, main)
        end
    end

    local ability = skill.Ability
    if ability == "Warrior" or ability == "Polymorph" then
        return character.Strength
    elseif ability == "Ranger" or ability == "Rogue" then
        return character.Finesse
    else
        return character.Intelligence
    end
end

--- @param skill StatEntrySkillData
--- @param attacker CDivinityStatsCharacter
function GetSkillAttributeDamageScale(skill, attacker)
    if attacker == nil or skill.UseWeaponDamage == "Yes" or skill.Ability == "None" then
        return 1.0
    else
        local primaryAttr = GetPrimaryAttributeAmount(skill, attacker)
        return 1.0 + ScaledDamageFromPrimaryAttribute(primaryAttr)
    end
end

--- @param skill StatEntrySkillData
--- @param stealthed boolean
--- @param attackerPos number[]
--- @param targetPos number[]
function GetDamageMultipliers(skill, stealthed, attackerPos, targetPos)
    local stealthDamageMultiplier = 1.0
    if stealthed then
        stealthDamageMultiplier = skill['Stealth Damage Multiplier'] * 0.01
    end

    local targetDistance = math.sqrt((attackerPos[1] - targetPos[1])^2 + (attackerPos[3] - targetPos[3])^2)
    local distanceDamageMultiplier = 1.0
    if targetDistance > 1.0 then
        distanceDamageMultiplier = Ext.Round(targetDistance) * skill['Distance Damage Multiplier'] * 0.01 + 1
    end

    local damageMultiplier = skill['Damage Multiplier'] * 0.01
    return stealthDamageMultiplier * distanceDamageMultiplier * damageMultiplier
end

--- @param level integer
function GetVitalityBoostByLevel(level)
    local extra = Ext.ExtraData
    local expGrowth = extra.VitalityExponentialGrowth
    local growth = expGrowth ^ (level - 1)

    if level >= extra.FirstVitalityLeapLevel then
        growth = growth * extra.FirstVitalityLeapGrowth / expGrowth
    end

    if level >= extra.SecondVitalityLeapLevel then
        growth = growth * extra.SecondVitalityLeapGrowth / expGrowth
    end

    if level >= extra.ThirdVitalityLeapLevel then
        growth = growth * extra.ThirdVitalityLeapGrowth / expGrowth
    end

    if level >= extra.FourthVitalityLeapLevel then
        growth = growth * extra.FourthVitalityLeapGrowth / expGrowth
    end

    local vit = level * extra.VitalityLinearGrowth + extra.VitalityStartingAmount * growth
    return math.floor(Ext.Round(vit) / 5.0) * 5.0
end

--- @param level integer
function GetLevelScaledDamage(level)
    local vitalityBoost = GetVitalityBoostByLevel(level)
    return vitalityBoost / (((level - 1) * Ext.ExtraData.VitalityToDamageRatioGrowth) + Ext.ExtraData.VitalityToDamageRatio)
end

--- @param level integer
function GetAverageLevelDamage(level)
    local scaled = GetLevelScaledDamage(level)
    return ((level * Ext.ExtraData.ExpectedDamageBoostFromAttributePerLevel) + 1.0) * scaled
        * ((level * Ext.ExtraData.ExpectedDamageBoostFromSkillAbilityPerLevel) + 1.0)
end

--- @param level integer
function GetLevelScaledWeaponDamage(level)
    local scaledDmg = GetLevelScaledDamage(level)
    return scaledDmg / ((level * Ext.ExtraData.ExpectedDamageBoostFromWeaponAbilityPerLevel) + 1.0)
end

--- @param level integer
function GetLevelScaledMonsterWeaponDamage(level)
    local weaponDmg = GetLevelScaledWeaponDamage(level)
    return ((level * Ext.ExtraData.MonsterDamageBoostPerLevel) + 1.0) * weaponDmg
end

--- @param attacker CDivinityStatsCharacter
function GetShieldPhysicalArmor(attacker)
    local shield = attacker:GetItemBySlot("Shield", true)
    if shield == nil or shield.ItemType ~= "Shield" then
        return 0
    end
    local stats = shield.DynamicStats
    local armor = 0
    local boost = 0
    for i, stat in pairs(stats) do
        if stat.StatsType == "Shield" then
            armor = armor + stat.ArmorValue
            boost = boost + stat.ArmorBoost * 0.01
        end
    end
    return armor * (1.0 + boost)
end

DamageBoostTable = {
    --- @param character CDivinityStatsCharacter
    Physical = function (character)
        return character.WarriorLore * Ext.ExtraData.SkillAbilityPhysicalDamageBoostPerPoint
    end,
    --- @param character CDivinityStatsCharacter
    Fire = function (character)
        return character.FireSpecialist * Ext.ExtraData.SkillAbilityFireDamageBoostPerPoint
    end,
    --- @param character CDivinityStatsCharacter
    Air = function (character)
        return character.AirSpecialist * Ext.ExtraData.SkillAbilityAirDamageBoostPerPoint
    end,
    --- @param character CDivinityStatsCharacter
    Water = function (character)
        return character.WaterSpecialist * Ext.ExtraData.SkillAbilityWaterDamageBoostPerPoint
    end,
    --- @param character CDivinityStatsCharacter
    Earth = function (character)
        return character.EarthSpecialist * Ext.ExtraData.SkillAbilityPoisonAndEarthDamageBoostPerPoint
    end,
    --- @param character CDivinityStatsCharacter
    Poison = function (character)
        return character.EarthSpecialist * Ext.ExtraData.SkillAbilityPoisonAndEarthDamageBoostPerPoint
    end
}

--- Base implementation code for reference purposes;
--- GetDamageBoostByType (below) is used instead for custom damage type support.
--- @param character CDivinityStatsCharacter
--- @param damageType string See DamageType enum
function GetDamageBoostByTypeVanilla(character, damageType)
    local boostFunc = DamageBoostTable[damageType]
    if boostFunc ~= nil then
        return boostFunc(character) / 100.0
    else
        return 0.0
    end
end

--- @param character CDivinityStatsCharacter
--- @param damageType string See DamageType enum
function GetDamageBoostByType(character, damageType)
    return Ext.Stats.Math.GetDamageBoostByType(character, damageType) / 100.0
end

--- @param character CDivinityStatsCharacter
--- @param damageList StatsDamagePairList
function ApplyDamageBoosts(character, damageList)
    for i, damage in pairs(damageList:ToTable()) do
        local boost = GetDamageBoostByType(character, damage.DamageType)
        if boost > 0.0 then
            damageList:Add(damage.DamageType, Ext.Round(damage.Amount * boost))
        end
    end
end

local DamageSourceCalcTable = {
    BaseLevelDamage = function (attacker, target, level)
        return math.max(0, Ext.Round(GetLevelScaledDamage(level)))
    end,
    AverageLevelDamge = function (attacker, target, level)
        return math.max(0, Ext.Round(GetAverageLevelDamage(level)))
    end,
    MonsterWeaponDamage = function (attacker, target, level)
        return math.max(0, Ext.Round(GetLevelScaledMonsterWeaponDamage(level)))
    end,
    SourceMaximumVitality = function (attacker, target, level)
        return attacker.MaxVitality
    end,
    SourceMaximumPhysicalArmor = function (attacker, target, level)
        return attacker.MaxArmor
    end,
    SourceMaximumMagicArmor = function (attacker, target, level)
        return attacker.MaxMagicArmor
    end,
    SourceCurrentVitality = function (attacker, target, level)
        return attacker.CurrentVitality
    end,
    SourceCurrentPhysicalArmor = function (attacker, target, level)
        return attacker.CurrentArmor
    end,
    SourceCurrentMagicArmor = function (attacker, target, level)
        return attacker.CurrentMagicArmor
    end,
    SourceShieldPhysicalArmor = function (attacker, target, level)
        return Ext.Round(GetShieldPhysicalArmor(attacker))
    end,
    TargetMaximumVitality = function (attacker, target, level)
        return target.MaxVitality
    end,
    TargetMaximumPhysicalArmor = function (attacker, target, level)
        return target.MaxArmor
    end,
    TargetMaximumMagicArmor = function (attacker, target, level)
        return target.MaxMagicArmor
    end,
    TargetCurrentVitality = function (attacker, target, level)
        return target.CurrentVitality
    end,
    TargetCurrentPhysicalArmor = function (attacker, target, level)
        return target.CurrentArmor
    end,
    TargetCurrentMagicArmor = function (attacker, target, level)
        return target.CurrentMagicArmor
    end
}

--- @param skillDamageType string See DamageType enumeration
--- @param attacker CDivinityStatsCharacter
--- @param target CDivinityStatsCharacter|CDivinityStatsItem
--- @param level integer
function CalculateBaseDamage(skillDamageType, attacker, target, level)
    return DamageSourceCalcTable[skillDamageType](attacker, target, level)
end

--- @param damageList StatsDamagePairList
function GetDamageListDeathType(damageList)
    local biggestDamage = -1
    local deathType

    for i, damage in pairs(damageList:ToTable()) do
        if damage.Amount > biggestDamage then
            deathType = DamageTypeToDeathType(damage.DamageType)
            biggestDamage = damage.Amount
        end
    end

    return deathType
end

--- @param character CDivinityStatsCharacter
--- @param weapon CDivinityStatsItem
function GetWeaponAbility(character, weapon)
    if weapon == nil then
        return nil
    end

    local offHandWeapon = character.OffHandWeapon
    if offHandWeapon ~= nil and IsRangedWeapon(weapon) == IsRangedWeapon(offHandWeapon) then
        return "DualWielding"
    end

    local weaponType = weapon.WeaponType
    if weaponType == "Bow" or weaponType == "Crossbow" or weaponType == "Rifle" then
        return "Ranged"
    end

    if weapon.IsTwoHanded then
        return "TwoHanded"
    end

    return "SingleHanded"
end

--- @param character CDivinityStatsCharacter
--- @param weapon CDivinityStatsItem
function ComputeWeaponCombatAbilityBoost(character, weapon)
    local abilityType = GetWeaponAbility(character, weapon)

    if abilityType == "SingleHanded" or abilityType == "TwoHanded" or abilityType == "Ranged" or abilityType == "DualWielding" then
        local abilityLevel = character[abilityType]
        return abilityLevel * Ext.ExtraData.CombatAbilityDamageBonus
    else
        return 0
    end
end

--- @param weapon CDivinityStatsItem
function GetWeaponScalingRequirement(weapon)
    local requirementName
    -- Stat requirements without points (i.e. just "Strength") get -1 as their Param value, so we need a -2 minimum to capture them
    local largestRequirement = -2

    for i, requirement in pairs(weapon.Requirements) do
        local reqName = requirement.Requirement
        if not requirement.Not and requirement.Param > largestRequirement and
            (reqName == "Strength" or reqName == "Finesse" or reqName == "Intelligence" or
            reqName == "Constitution" or reqName == "Memory" or reqName == "Wits") then
            requirementName = reqName
            largestRequirement = requirement.Param
        end
    end

    return requirementName
end

--- @param character CDivinityStatsCharacter
--- @param weapon CDivinityStatsItem
function GetItemRequirementAttribute(character, weapon)
    local attribute = GetWeaponScalingRequirement(weapon)
    if attribute ~= nil then
        return character[attribute]
    else
        return 0
    end
end

--- @param character CDivinityStatsCharacter
--- @param weapon CDivinityStatsItem
function ComputeWeaponRequirementScaledDamage(character, weapon)
    local scalingReq = GetWeaponScalingRequirement(weapon)
    if scalingReq ~= nil then
        return ScaledDamageFromPrimaryAttribute(character[scalingReq]) * 100.0
    else
        return 0
    end
end

-- from Item::ComputeDamage
--- @param weapon CDivinityStatsItem
function ComputeBaseWeaponDamage(weapon)
    local damages = {}
    local stats = weapon.DynamicStats
    local baseStat = stats[1]
    local baseDmgFromBase = baseStat.DamageFromBase * 0.01
    local baseMinDamage = baseStat.MinDamage
    local baseMaxDamage = baseStat.MaxDamage
    local damageBoost = 0

    for i, stat in pairs(stats) do
        if stat.StatsType == "Weapon" then
            damageBoost = damageBoost + stat.DamageBoost

            if stat.DamageType ~= "None" then
                local dmgType = stat.DamageType
                local dmgFromBase = stat.DamageFromBase * 0.01
                local minDamage = stat.MinDamage
                local maxDamage = stat.MaxDamage

                if dmgFromBase ~= 0 then
                    if stat == baseStat then
                        if baseMinDamage ~= 0 then
                            minDamage = math.max(dmgFromBase * baseMinDamage, 1.0)
                        end
                        if baseMaxDamage ~= 0 then
                            maxDamage = math.max(dmgFromBase * baseMaxDamage, 1.0)
                        end
                    else
                        minDamage = math.max(baseDmgFromBase * dmgFromBase * baseMinDamage, 1.0)
                        maxDamage = math.max(baseDmgFromBase * dmgFromBase * baseMaxDamage, 1.0)
                    end
                end

                if minDamage > 0 then
                    maxDamage = math.max(maxDamage, minDamage + 1.0)
                end

                if damages[dmgType] == nil then
                    damages[dmgType] = {
                        Min = minDamage,
                        Max = maxDamage
                    }
                else
                    local damage = damages[dmgType]
                    damage.Min = damage.Min + minDamage
                    damage.Max = damage.Max + maxDamage
                end
            end
        end
    end

    return damages, damageBoost
end

-- from Item::ComputeDamage
--- @param weapon CDivinityStatsItem
function CalculateWeaponDamageWithDamageBoost(weapon)
    local damages, damageBoost = ComputeBaseWeaponDamage(weapon)
    local boost = 1.0 + damageBoost * 0.01

    for damageType, damage in pairs(damages) do
        if damageBoost ~= 0 then
            damage.Min = math.ceil(damage.Min * boost)
            damage.Max = math.ceil(damage.Max * boost)
        else
            damage.Min = Ext.Round(damage.Min)
            damage.Max = Ext.Round(damage.Max)
        end
    end

    return damages
end

-- from Item::ComputeScaledDamage
--- @param character CDivinityStatsCharacter
--- @param weapon CDivinityStatsItem
function CalculateWeaponScaledDamageRanges(character, weapon)
    local damages = CalculateWeaponDamageWithDamageBoost(weapon)

    local boost = character.DamageBoost 
        + ComputeWeaponCombatAbilityBoost(character, weapon)
        + ComputeWeaponRequirementScaledDamage(character, weapon)
    boost = boost / 100.0

    if character.IsSneaking then
        boost = boost + Ext.ExtraData['Sneak Damage Multiplier']
    end

    local boostMin = math.max(-1.0, boost)

    for damageType, damage in pairs(damages) do
        damage.Min = damage.Min + math.ceil(damage.Min * boostMin)
        damage.Max = damage.Max + math.ceil(damage.Max * boost)
    end

    return damages
end

-- from Character::CalculateWeaponDamageInner
--- @param character CDivinityStatsCharacter
--- @param weapon CDivinityStatsItem
--- @param damageList StatsDamagePairList
--- @param noRandomization boolean
function CalculateWeaponScaledDamage(character, weapon, damageList, noRandomization)
    local damages = CalculateWeaponScaledDamageRanges(character, weapon)

    for damageType, damage in pairs(damages) do
        local randRange = math.max(1, damage.Max - damage.Min)
        local finalAmount

        if noRandomization then
            finalAmount = damage.Min + math.floor(randRange / 2)
        else
            finalAmount = damage.Min + Ext.Random(0, randRange)
        end

        damageList:Add(damageType, finalAmount)
    end
end

--- @param attacker CDivinityStatsCharacter
--- @param weapon CDivinityStatsItem
--- @param noRandomization boolean
function CalculateWeaponDamage(attacker, weapon, noRandomization)
    local damageList = Ext.Stats.NewDamageList()

    CalculateWeaponScaledDamage(attacker, weapon, damageList, noRandomization)

    local offHand = attacker.OffHandWeapon
    if offHand ~= nil and weapon.InstanceId ~= offHand.InstanceId and false then -- Temporarily off
        local bonusWeapons = attacker.BonusWeapons -- FIXME - enumerate BonusWeapons /multiple/ from character stats!
        for i, bonusWeapon in pairs(bonusWeapons) do
            -- FIXME Create item from bonus weapon stat and apply attack as item???
            error("BonusWeapons not implemented")
            local bonusWeaponStats = CreateBonusWeapon(bonusWeapon)
            CalculateWeaponScaledDamage(attacker, bonusWeaponStats, damageList, noRandomization)
        end
    end

    ApplyDamageBoosts(attacker, damageList)

    if offHand ~= nil and weapon.InstanceId == offHand.InstanceId then
        damageList:Multiply(Ext.ExtraData.DualWieldingDamagePenalty)
    end

    return damageList
end

--- @param skill StatEntrySkillData
--- @param attacker CDivinityStatsCharacter
--- @param isFromItem boolean
--- @param stealthed boolean
--- @param attackerPos number[]
--- @param targetPos number[]
--- @param level integer
--- @param noRandomization boolean
--- @param mainWeapon CDivinityStatsItem  Optional mainhand weapon to use in place of the attacker's.
--- @param offHandWeapon CDivinityStatsItem   Optional offhand weapon to use in place of the attacker's.
function GetSkillDamage(skill, attacker, isFromItem, stealthed, attackerPos, targetPos, level, noRandomization, mainWeapon, offHandWeapon)
    if attacker ~= nil and level < 0 then
        level = attacker.Level
    end

    local damageMultiplier = skill['Damage Multiplier'] * 0.01
    local damageMultipliers = GetDamageMultipliers(skill, stealthed, attackerPos, targetPos)
    local skillDamageType = nil

    if level == 0 then
        level = skill.OverrideSkillLevel
        if level == 0 then
            level = skill.Level
        end
    end

    local damageList = Ext.Stats.NewDamageList()

    if damageMultiplier <= 0 then
        return
    end

    if skill.UseWeaponDamage == "Yes" then
        local damageType = skill.DamageType
        if damageType == "None" or damageType == "Sentinel" then
            damageType = nil
        end

        local weapon = mainWeapon or attacker.MainWeapon
        local offHand = offHandWeapon or attacker.OffHandWeapon

        if weapon ~= nil then
            local mainDmgs = CalculateWeaponDamage(attacker, weapon, noRandomization)
            mainDmgs:Multiply(damageMultipliers)
            if damageType ~= nil then
                mainDmgs:ConvertDamageType(damageType)
            end
            damageList:Merge(mainDmgs)
        end

        if offHand ~= nil and IsRangedWeapon(weapon) == IsRangedWeapon(offHand) then
            local offHandDmgs = CalculateWeaponDamage(attacker, offHand, noRandomization)
            offHandDmgs:Multiply(damageMultipliers)
            if damageType ~= nil then
                offHandDmgs:ConvertDamageType(damageType)
                skillDamageType = damageType
            end
            damageList:Merge(offHandDmgs)
        end

        damageList:AggregateSameTypeDamages()
    else
        local damageType = skill.DamageType

        local baseDamage = CalculateBaseDamage(skill.Damage, attacker, nil, level)
        local damageRange = skill['Damage Range']
        local randomMultiplier
        if noRandomization then
            randomMultiplier = 0.0
        else
            randomMultiplier = 1.0 + (Ext.Random(0, damageRange) - damageRange/2) * 0.01
        end

        local attrDamageScale
        local skillDamage = skill.Damage
        if skillDamage == "BaseLevelDamage" or skillDamage == "AverageLevelDamge" or skillDamage == "MonsterWeaponDamage" then
            attrDamageScale = GetSkillAttributeDamageScale(skill, attacker)
        else
            attrDamageScale = 1.0
        end

        local damageBoost
        if attacker ~= nil then
            damageBoost = attacker.DamageBoost / 100.0 + 1.0
        else
            damageBoost = 1.0
        end
        
        local finalDamage = baseDamage * randomMultiplier * attrDamageScale * damageMultipliers
        finalDamage = math.max(Ext.Round(finalDamage), 1)
        finalDamage = math.ceil(finalDamage * damageBoost)
        damageList:Add(damageType, finalDamage)

        if attacker ~= nil then
            ApplyDamageBoosts(attacker, damageList)
        end
    end

    local deathType = skill.DeathType
    if deathType == "None" then
        if skill.UseWeaponDamage == "Yes" then
            deathType = GetDamageListDeathType(damageList)
        else
            if skillDamageType == nil then
                skillDamageType = skill.DamageType
            end

            deathType = DamageTypeToDeathType(skillDamageType)
        end
    end

    return damageList, deathType
end

-- Not used anymore, kept for compatibility only
HitFlag = {
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

--- @param damageList StatsDamagePairList
--- @param attacker CDivinityStatsCharacter
function ApplyDamageSkillAbilityBonuses(damageList, attacker)

    if attacker == nil then
        return
     end
 
     local magicArmorDamage = 0
     local armorDamage = 0
 
     for i,damage in pairs(damageList:ToTable()) do
         local type = damage.DamageType
         if type == "Magic" or type == "Fire" or type == "Air" or type == "Water" or type == "Earth" then
             magicArmorDamage = magicArmorDamage + damage.Amount
         end
 
         if type == "Physical" or type == "Corrosive" or type == "Sulfuric" then
             armorDamage = armorDamage + damage.Amount
         end
     end
   
     if magicArmorDamage > 0 then
         local airSpecialist = attacker.AirSpecialist
         if airSpecialist > 0 then
             local magicBonus = airSpecialist * Ext.ExtraData.SkillAbilityDamageToMagicArmorPerPoint
             if magicBonus > 0 then
                 magicArmorDamage = math.ceil((magicArmorDamage * magicBonus) / 100.0)
                 damageList:Add("Magic", magicArmorDamage)
             end
         end
     end
   
     if armorDamage > 0 then
         local armorBonus = attacker.WarriorLore * Ext.ExtraData.SkillAbilityDamageToPhysicalArmorPerPoint
         if armorBonus > 0 then
             armorDamage = math.ceil((armorDamage * armorBonus) / 100.0)
             damageList:Add("Corrosive", armorDamage)
         end
     end
end

--- @param character CDivinityStatsCharacter
--- @param type string DamageType enumeration
function GetResistance(character, type)
    return Ext.Stats.Math.GetResistance(character, type, false)
end

--- @param character CDivinityStatsCharacter
--- @param damageList StatsDamagePairList
function ApplyHitResistances(character, damageList)
    for i,damage in pairs(damageList:ToTable()) do
        local resistance = GetResistance(character, damage.DamageType)
        damageList:Add(damage.DamageType, math.floor(damage.Amount * -resistance / 100.0))
    end
end

--- @param character CDivinityStatsCharacter
--- @param attacker CDivinityStatsCharacter
--- @param damageList StatsDamagePairList
function ApplyDamageCharacterBonuses(character, attacker, damageList)
    damageList:AggregateSameTypeDamages()
    ApplyHitResistances(character, damageList)

    ApplyDamageSkillAbilityBonuses(damageList, attacker)
end


--- @param character CDivinityStatsCharacter
--- @param ability string Ability enumeration
function GetAbilityCriticalHitMultiplier(character, ability)
    if ability == "TwoHanded" then
        return Ext.Round(character.TwoHanded * Ext.ExtraData.CombatAbilityCritMultiplierBonus)
    end
        
    if ability == "RogueLore" then
        return Ext.Round(character.RogueLore * Ext.ExtraData.SkillAbilityCritMultiplierPerPoint)
    end

    return 0
end

--- @param weapon CDivinityStatsItem
--- @param character CDivinityStatsCharacter
function GetCriticalHitMultiplier(weapon, character)
    local criticalMultiplier = 0
    if weapon.ItemType == "Weapon" then
        for i,stat in pairs(weapon.DynamicStats) do
            criticalMultiplier = criticalMultiplier + stat.CriticalDamage
        end
  
        if character ~= nil then
            local ability = GetWeaponAbility(character, weapon)
            criticalMultiplier = criticalMultiplier + GetAbilityCriticalHitMultiplier(character, ability) + GetAbilityCriticalHitMultiplier(character, "RogueLore")
                
            if character.TALENT_Human_Inventive then
                criticalMultiplier = criticalMultiplier + Ext.ExtraData.TalentHumanCriticalMultiplier
            end
        end
    end
  
    return criticalMultiplier * 0.01
end

--- @param hit StatsHitDamageInfo
--- @param attacker CDivinityStatsCharacter
--- @param ctx HitCalculationContext
function ApplyCriticalHit(hit, attacker, ctx)
    local mainWeapon = attacker.MainWeapon
    if mainWeapon ~= nil then
        hit.CriticalHit = true
        ctx.DamageMultiplier = ctx.DamageMultiplier + (GetCriticalHitMultiplier(mainWeapon, attacker) - 1.0)
    end
end

--- @param hit StatsHitDamageInfo
--- @param attacker CDivinityStatsCharacter
--- @param hitType string HitType enumeration
--- @param criticalRoll string CriticalRoll enumeration
function ShouldApplyCriticalHit(hit, attacker, hitType, criticalRoll)
    if criticalRoll ~= "Roll" then
        return criticalRoll == "Critical"
    end

    if attacker.TALENT_Haymaker then
        return false
    end

    if hitType == "DoT" or hitType == "Surface" then
        return false
    end
    
    local critChance = attacker.CriticalChance
    if attacker.TALENT_ViolentMagic and hitType == "Magic" then
        critChance = critChance * Ext.ExtraData.TalentViolentMagicCriticalChancePercent * 0.01
        critChance = math.max(critChance, 1)
    else
        if hit.Backstab then
            return true
        end

        if hitType == "Magic" then
            return false
        end
    end

    return math.random(0, 99) < critChance
end

--- @param hit StatsHitDamageInfo
--- @param target CDivinityStatsCharacter
--- @param attacker CDivinityStatsCharacter
--- @param hitType string HitType enumeration
--- @param criticalRoll string CriticalRoll enumeration
--- @param ctx HitCalculationContext
function ConditionalApplyCriticalHitMultiplier(hit, target, attacker, hitType, criticalRoll, ctx)
    if ShouldApplyCriticalHit(hit, attacker, hitType, criticalRoll) then
        ApplyCriticalHit(hit, attacker, ctx)
    end
end

--- @param hit StatsHitDamageInfo
--- @param target CDivinityStatsCharacter
--- @param attacker CDivinityStatsCharacter
--- @param hitType string HitType enumeration
function ApplyLifeSteal(hit, target, attacker, hitType)
    if attacker == nil or hitType == "DoT" or hitType == "Surface" then
        return
    end
    
    local magicDmg = hit.DamageList:GetByType("Magic")
    local corrosiveDmg = hit.DamageList:GetByType("Corrosive")
    local lifesteal = hit.TotalDamageDone - hit.ArmorAbsorption - corrosiveDmg - magicDmg

    if hit.FromShacklesOfPain or hit.NoDamageOnOwner or hit.Reflection then
        local modifier = Ext.ExtraData.LifestealFromReflectionModifier
        lifesteal = math.floor(lifesteal * modifier)
    end

    if lifesteal > target.CurrentVitality then
        lifesteal = target.CurrentVitality
    end

    if lifesteal > 0 then
        hit.LifeSteal = math.max(math.ceil(lifesteal * attacker.LifeSteal / 100), 0)
    end
end

--- @param damageList StatsDamagePairList
--- @param hit StatsHitDamageInfo
function ApplyDamagesToHitInfo(damageList, hit)
    local totalDamage = 0
    for i,damage in pairs(damageList:ToTable()) do
        totalDamage = totalDamage + damage.Amount
        if damage.DamageType == "Chaos" then
            hit.DamageList:Add(hit.DamageType, damage.Amount)
        else
            hit.DamageList:Add(damage.DamageType, damage.Amount)
        end
    end

    hit.TotalDamageDone = hit.TotalDamageDone + totalDamage
end

--- @param damageList StatsDamagePairList
--- @param armor integer
function ComputeArmorDamage(damageList, armor)
    local damage = damageList:GetByType("Corrosive") + damageList:GetByType("Physical") + damageList:GetByType("Sulfuric")
    return math.min(armor, damage)
end

--- @param damageList StatsDamagePairList
--- @param magicArmor integer
function ComputeMagicArmorDamage(damageList, magicArmor)
    local damage = damageList:GetByType("Magic") 
        + damageList:GetByType("Fire") 
        + damageList:GetByType("Water")
        + damageList:GetByType("Air")
        + damageList:GetByType("Earth")
        + damageList:GetByType("Poison")
    return math.min(magicArmor, damage)
end

--- @param hit StatsHitDamageInfo
--- @param damageList StatsDamagePairList
--- @param statusBonusDmgTypes StatsDamagePairList
--- @param hitType string HitType enumeration
--- @param target CDivinityStatsCharacter
--- @param attacker CDivinityStatsCharacter
--- @param ctx HitCalculationContext
function DoHit(hit, damageList, statusBonusDmgTypes, hitType, target, attacker, ctx)
    hit.Hit = true;
    damageList:AggregateSameTypeDamages()
    damageList:Multiply(ctx.DamageMultiplier)

    local totalDamage = 0
    for i,damage in pairs(damageList:ToTable()) do
        totalDamage = totalDamage + damage.Amount
    end

    if totalDamage < 0 then
        damageList:Clear()
    end

    ApplyDamageCharacterBonuses(target, attacker, damageList)
    damageList:AggregateSameTypeDamages()
    hit.DamageList = Ext.Stats.NewDamageList()

    for i,damageType in pairs(statusBonusDmgTypes) do
        damageList:Add(damageType, math.ceil(totalDamage * 0.1))
    end

    ApplyDamagesToHitInfo(damageList, hit)
    hit.ArmorAbsorption = hit.ArmorAbsorption + ComputeArmorDamage(damageList, target.CurrentArmor)
    hit.ArmorAbsorption = hit.ArmorAbsorption + ComputeMagicArmorDamage(damageList, target.CurrentMagicArmor)

    if hit.TotalDamageDone > 0 then
        ApplyLifeSteal(hit, target, attacker, hitType)
    else
        hit.DontCreateBloodSurface = true
    end

    if hitType == "Surface" then
        hit.Surface = true
    end

    if hitType == "DoT" then
        hit.DoT = true
    end
end

--- @param attacker CDivinityStatsCharacter
--- @param target CDivinityStatsCharacter
--- @param highGround string HighGround enumeration
function GetAttackerDamageMultiplier(attacker, target, highGround)
    if target == nil then
        return 0.0
    end

    if highGround == "HighGround" then
        local rangerLoreBonus = attacker.RangerLore * Ext.ExtraData.SkillAbilityHighGroundBonusPerPoint * 0.01
        return math.max(rangerLoreBonus + Ext.ExtraData.HighGroundBaseDamageBonus, 0.0)
    elseif highGround == "LowGround" then
        return Ext.ExtraData.LowGroundBaseDamagePenalty
    else
        return 0.0
    end
end

--- @param character CDivinityStatsCharacter
--- @param item CDivinityStatsItem
function DamageItemDurability(character, item)
    local degradeSpeed = 0
    for i,stats in pairs(item.DynamicStats) do
        degradeSpeed = degradeSpeed + stats.DurabilityDegradeSpeed
    end

    if degradeSpeed > 0 then
        local durability = math.max(0, item.Durability)
        item.Durability = durability
        item.ShouldSyncStats = true

        if durability == 0 then
            -- FIXME not implemented yet
            -- Ext.ReevaluateItems(character)
        end
    end
end

--- @param character CDivinityStatsCharacter
--- @param item CDivinityStatsItem
function ConditionalDamageItemDurability(character, item)
    if not character.InParty or not item.LoseDurabilityOnCharacterHit or item.Unbreakable or not IsRangedWeapon(item) then
        return
    end

    local chance = 100
    if character.TALENT_Durability then
        chance = 50
    end

    if math.random(0, 99) < chance then
        DamageItemDurability(character, item)
    end
end

--- @param attacker CDivinityStatsCharacter
--- @param target CDivinityStatsCharacter
function CalculateHitChance(attacker, target)
    if attacker.TALENT_Haymaker then
        return 100
    end

    local ranged = IsRangedWeapon(attacker.MainWeapon)
    local accuracy = attacker.Accuracy
    local dodge = 0
    if (not attacker.Invisible or ranged) and target.IsIncapacitatedRefCount == 0 then
        dodge = target.Dodge
    end

    local chanceToHit1 = Ext.Round(((100.0 - dodge) * accuracy) / 100)
    chanceToHit1 = math.max(0, math.min(100, chanceToHit1))
    return chanceToHit1 + attacker.ChanceToHitBoost
end

--- @param target CDivinityStatsCharacter
--- @param attacker CDivinityStatsCharacter
function IsInFlankingPosition(target, attacker)
    local tPos = target.Position
    local aPos = attacker.Position
    local rotation = target.Rotation

    local dx, dy, dz = tPos[1] - aPos[1], tPos[2] - aPos[2], tPos[3] - aPos[3]
    local distanceSq = 1.0 / math.sqrt(dx^2 + dy^2 + dz^2)
    local nx, ny, nz = dx * distanceSq, dy * distanceSq, dz * distanceSq

    local ang = -rotation[6] * nx - rotation[7] * ny - rotation[8] * nz
    return ang > math.cos(0.52359879)
end

--- @param target CDivinityStatsCharacter
--- @param attacker CDivinityStatsCharacter
function CanBackstab(target, attacker)
    local targetPos = target.Position
    local attackerPos = attacker.Position

    local atkDir = {}
    for i=1,3 do
        atkDir[i] = attackerPos[i] - targetPos[i]
    end

    local atkAngle = math.deg(math.atan(atkDir[3], atkDir[1]))
    if atkAngle < 0 then
        atkAngle = 360 + atkAngle
    end

    local targetRot = target.Rotation
    local angle = math.deg(math.atan(-targetRot[1], targetRot[3]))
    if angle < 0 then
        angle = 360 + angle
    end

    local relAngle = atkAngle - angle
    if relAngle < 0 then
        relAngle = 360 + relAngle
    end

    return relAngle >= 150 and relAngle <= 210
end

--- @param target CDivinityStatsCharacter
--- @param attacker CDivinityStatsCharacter
--- @param weapon CDivinityStatsItem
--- @param damageList StatsDamagePairList
--- @param hitType string HitType enumeration
--- @param noHitRoll boolean
--- @param forceReduceDurability boolean
--- @param hit StatsHitDamageInfo
--- @param alwaysBackstab boolean
--- @param highGroundFlag string HighGround enumeration
--- @param criticalRoll string CriticalRoll enumeration
function ComputeCharacterHit(target, attacker, weapon, damageList, hitType, noHitRoll, forceReduceDurability, hit, alwaysBackstab, highGroundFlag, criticalRoll)

    local ctx = {
        DamageMultiplier = 1.0
    }
    local statusBonusDmgTypes = {}
    
    if attacker == nil then
        DoHit(hit, damageList, statusBonusDmgTypes, hitType, target, attacker, ctx)
        return hit
    end

    ctx.DamageMultiplier = 1.0 + GetAttackerDamageMultiplier(attacker, target, highGroundFlag)
    if hitType == "Magic" or hitType == "Surface" or hitType == "DoT" or hitType == "Reflected" then
        ConditionalApplyCriticalHitMultiplier(hit, target, attacker, hitType, criticalRoll, ctx)
        DoHit(hit, damageList, statusBonusDmgTypes, hitType, target, attacker, ctx)
        return hit
    end

    if weapon == nil then
        weapon = attacker.MainWeapon
    end

    local backstabbed = false
    if alwaysBackstab or (weapon ~= nil and weapon.WeaponType == "Knife" and CanBackstab(target, attacker)) then
        hit.Backstab = true
        backstabbed = true
    end

    if hitType == "Melee" then
        if IsInFlankingPosition(target, attacker) then
            hit.Flanking = true
        end
    
        -- Apply Sadist talent
        if attacker.TALENT_Sadist then
            if hit.Poisoned then
                table.insert(statusBonusDmgTypes, "Poison")
            end
            if hit.Burning then
                table.insert(statusBonusDmgTypes, "Fire")
            end
            if hit.Bleeding then
                table.insert(statusBonusDmgTypes, "Physical")
            end
        end
    end

    if attacker.TALENT_Damage then
        ctx.DamageMultiplier = ctx.DamageMultiplier + 0.1
    end

    local hitBlocked = false

    if not noHitRoll then
        local hitChance = CalculateHitChance(attacker, target)
        local hitRoll = math.random(0, 99)
        if hitRoll >= hitChance then
            if target.TALENT_RangerLoreEvasionBonus and hitRoll < hitChance + 10 then
                hit.Dodged = true
            else
                hit.Missed = true
            end
            hitBlocked = true
        else
            local blockChance = target.BlockChance
            if not backstabbed and blockChance > 0 and math.random(0, 99) < blockChance then
                hit.Blocked = true
                hitBlocked = true
            end
        end
    end

    if weapon ~= nil and weapon.Name ~= "DefaultWeapon" and hitType ~= "Magic" and forceReduceDurability and not hit.Missed and not hit.Dodged then
        ConditionalDamageItemDurability(attacker, weapon)
    end

    if not hitBlocked then
        ConditionalApplyCriticalHitMultiplier(hit, target, attacker, hitType, criticalRoll, ctx)
        DoHit(hit, damageList, statusBonusDmgTypes, hitType, target, attacker, ctx)
    end

    return hit
end

--- @param character CDivinityStatsCharacter
--- @param skill StatEntrySkillData
--- @param mainWeapon CDivinityStatsItem  Optional mainhand weapon to use in place of the character's.
--- @param offHandWeapon CDivinityStatsItem   Optional offhand weapon to use in place of the character's.
function GetSkillDamageRange(character, skill, mainWeapon, offHandWeapon)
    local damageMultiplier = skill['Damage Multiplier'] * 0.01
    local result

    if skill.UseWeaponDamage == "Yes" then
        local mainWeapon = mainWeapon or character.MainWeapon
        local offHandWeapon = offHandWeapon or character.OffHandWeapon
        local mainDamageRange = CalculateWeaponScaledDamageRanges(character, mainWeapon)

        if offHandWeapon ~= nil and IsRangedWeapon(mainWeapon) == IsRangedWeapon(offHandWeapon) then
            local offHandDamageRange = CalculateWeaponScaledDamageRanges(character, offHandWeapon)

            -- Note: This differs from the way the game applies DualWieldingDamagePenalty.
            -- In the original tooltip code, it is applied for the whole damage value,
            -- not per damage type, so the result may differ from the original tooltip code
            -- if DualWieldingDamagePenalty is not 1.0 or 0.5.
            -- However, this formula is the correct one and the vanilla tooltip returns
            -- buggy values if DualWieldingDamagePenalty ~= 1.0 and ~= 0.5
            local dualWieldPenalty = Ext.ExtraData.DualWieldingDamagePenalty
            for damageType, range in pairs(offHandDamageRange) do
                local min = math.ceil(range.Min * dualWieldPenalty)
                local max = math.ceil(range.Max * dualWieldPenalty)
                local range = mainDamageRange[damageType]
                if mainDamageRange[damageType] ~= nil then
                    range.Min = range.Min + min
                    range.Max = range.Max + max
                else
                    mainDamageRange[damageType] = {Min = min, Max = max}
                end
            end
        end

        for damageType, range in pairs(mainDamageRange) do
            local min = Ext.Round(range.Min * damageMultiplier)
            local max = Ext.Round(range.Max * damageMultiplier)
            range.Min = min + math.ceil(min * GetDamageBoostByType(character, damageType))
            range.Max = max + math.ceil(max * GetDamageBoostByType(character, damageType))
        end

        local damageType = skill.DamageType
        if damageType ~= "None" and damageType ~= "Sentinel" then
            local min, max = 0, 0
            local boost = GetDamageBoostByType(character, damageType)
            for _, range in pairs(mainDamageRange) do
                min = min + range.Min + math.ceil(range.Min * GetDamageBoostByType(character, damageType))
                max = max + range.Max + math.ceil(range.Min * GetDamageBoostByType(character, damageType))
            end
    
            mainDamageRange = {}
            mainDamageRange[damageType] = {Min = min, Max = max}
        end

        result = mainDamageRange
    else
        local damageType = skill.DamageType
        if damageMultiplier <= 0 then
            return {}
        end

        local level = character.Level
        if (level < 0 or skill.OverrideSkillLevel == "Yes") and skill.Level > 0 then
            level = skill.Level
        end

        local skillDamageType = skill.Damage
        local attrDamageScale
        if skillDamageType == "BaseLevelDamage" or skillDamageType == "AverageLevelDamge" or skillDamageType == "MonsterWeaponDamage" then
            attrDamageScale = GetSkillAttributeDamageScale(skill, character)
        else
            attrDamageScale = 1.0
        end

        local baseDamage = CalculateBaseDamage(skill.Damage, character, nil, level) * attrDamageScale * damageMultiplier
        local damageRange = skill['Damage Range'] * baseDamage * 0.005

        local damageTypeBoost = 1.0 + GetDamageBoostByType(character, damageType)
        local damageBoost = 1.0 + (character.DamageBoost / 100.0)

        local finalMin = math.ceil(math.ceil(Ext.Round(baseDamage - damageRange) * damageBoost) * damageTypeBoost)
        local finalMax = math.ceil(math.ceil(Ext.Round(baseDamage + damageRange) * damageBoost) * damageTypeBoost)

        if finalMin > 0 then
            finalMax = math.max(finalMin + 1.0, finalMax)
        end

        local damageRanges = {}
        damageRanges[damageType] = {
            Min = finalMin, Max = finalMax
        }
        result = damageRanges
    end

    -- Compatibility hack for old (v50) table format
    for i,range in pairs(result) do
        range[1] = range.Min
        range[2] = range.Max
    end

    return result
end



StatusSavingThrows = {
    MUTED = "MagicArmor",
    CHARMED = "MagicArmor",
    DRAIN = "MagicArmor",
    SOURCE_MUTED = "MagicArmor",

    KNOCKED_DOWN = "SavingThrow",
    CONSUME = "SavingThrow",
    DAMAGE = "SavingThrow",
    INCAPACITATED = "SavingThrow",
    POLYMORPHED = "SavingThrow",
    DAMAGE_ON_MOVE = "SavingThrow",
    DEMONIC_BARGAIN = "SavingThrow",
    CHALLENGE = "SavingThrow",
    DISARMED = "SavingThrow",
    HEAL_SHARING = "SavingThrow",
    
    INFECTIOUS_DISEASED = "PhysicalArmor",
    SHACKLES_OF_PAIN = "PhysicalArmor",
    DECAYING_TOUCH = "PhysicalArmor",
    REMORSE = "Remorse"
}

--- @param status EsvStatus
function GetSavingThrowForStatus(status)
    if status.StatusId ~= status.StatusType then
        return Ext.StatGetAttribute(status.StatusId, "SavingThrow")
    end

    local savingThrow = StatusSavingThrows[status.StatusType]
    if savingThrow == nil then
        return "None"
    elseif savingThrow == "SavingThrow" then
        return Ext.StatGetAttribute(status.StatusId, "SavingThrow")
    else
        return savingThrow
    end
end

MagicSavingThrows = {
    Frozen = true,
    Mute = true,
    Stunned = true,
    Fear = true,
    Charm = true,
    Petrified = true,
    Taunted = true,
    MagicArmor = true
}

--- @param character EsvCharacter
--- @param savingThrow string SavingThrow enumeration
function GetSavingThrowChanceMultiplier(character, savingThrow)
    if savingThrow == "PhysicalArmor" then
        if character.Stats.CurrentArmor > 0 and not character.Stats.TALENT_Raistlin then
            return 0.0
        end
    elseif MagicSavingThrows[savingThrow] ~= nil then
        if character.Stats.CurrentMagicArmor > 0 and not character.Stats.TALENT_Raistlin then
            return 0.0
        end
    end

    return 1.0
end

--- @param status EsvStatus
function CanTriggerTorturer(status)
    local source = Ext.Entity.GetGameObject(status.StatusSourceHandle)
    local causeType = status.DamageSourceType
    local statusType = status.StatusType

    return source ~= nil and 
        source.Stats ~= nil and
        source.Stats.TALENT_Torturer and 
        causeType ~= "SurfaceMove" and causeType ~= "SurfaceCreate" and causeType ~= "SurfaceStatus" and
        (statusType == "DAMAGE" or statusType == "DAMAGE_ON_MOVE")
end

--- @param status EsvStatus
--- @param isEnterCheck boolean
function StatusGetEnterChance(status, isEnterCheck)
    local target = Ext.Entity.GetGameObject(status.TargetHandle)
    if target ~= nil and not target.Dead and not target:HasTag("GHOST") then
        if status.ForceStatus then
            return 100
        end
        
        local savingThrow = GetSavingThrowForStatus(status)
        if savingThrow ~= "None" then
            if isEnterCheck then
                if CanTriggerTorturer(status) then
                    return 100
                else
                    return status.CanEnterChance * GetSavingThrowChanceMultiplier(target, savingThrow)
                end
            end
        elseif not isEnterCheck then
            return 100
        end
    end
      
    return status.CanEnterChance
end


--- @param character CDivinityStatsCharacter
--- @param weapon CDivinityStatsItem
function GetWeaponAPCost(character, weapon)
    if weapon.ItemType ~= "Weapon" then
        return 0
    end

    local ap = weapon.AttackAPCost
    if weapon.WeaponType == "Knife" and character.TALENT_RogueLoreDaggerAPBonus then
        ap = ap - 1
    end

    return math.max(ap, 1)
end


--- @param character CDivinityStatsCharacter
function GetCharacterWeaponAPCost(character)
    local mainWeapon = character.MainWeapon
    local offHandWeapon = character.OffHandWeapon

    local ap = GetWeaponAPCost(character, mainWeapon)

    if offHandWeapon ~= nil and IsRangedWeapon(mainWeapon) == IsRangedWeapon(offHandWeapon) then
        ap = ap + GetWeaponAPCost(character, offHandWeapon) - Ext.ExtraData.DualWieldingAPPenalty
    else
        --- @type CDivinityStatsItem
        local offHandItem = character:GetItemBySlot("Shield")
        if offHandItem ~= nil and offHandItem.ItemType == "Shield" then
            ap = ap + Ext.ExtraData.ShieldAPCost
        end
    end

    return ap
end


local ElementalAffinityAiFlags = {
    Fire = { "Lava", "Fire" },
    Water = { "Water" },
    Air = { "Electrified" },
    Earth = { "Oil", "Poison" },
    Death = { "Blood" },
    Sulfurology = { "Sulfurium" }
}

--- @param skill StatEntrySkillData
--- @param character CDivinityStatsCharacter
--- @param grid EocAiGrid
--- @param position number[]
--- @param radius number
function GetSkillAPCost(skill, character, grid, position, radius)
    local baseAP = skill.ActionPoints
    if character == nil or baseAP <= 0 then
        return baseAP, false
    end
    
    local ability = skill.Ability
    local elementalAffinity = false
    if ability ~= "None" and baseAP > 1 and character.TALENT_ElementalAffinity and grid ~= nil and position ~= nil and radius ~= nil then
        local aiFlags = ElementalAffinityAiFlags[ability]
        if aiFlags ~= nil then
            elementalAffinity = grid:SearchForCell(position[1], position[3], radius, aiFlags, -1.0)
            if elementalAffinity then
                baseAP = baseAP - 1
            end
        end
    end

    local characterAP = 1
    if skill.Requirement ~= "None" and skill.OverrideMinAP == "No" then
        characterAP = GetCharacterWeaponAPCost(character)
    end

    return math.max(characterAP, baseAP), elementalAffinity
end

--- @param entity EsvCharacter|EsvItem
--- @param combatComp EsvCombatComponent
function CanEntityFight(entity, combatComp)
    if not combatComp.CanFight or not combatComp.CanJoinCombat then
        return false
    end

    if Ext.Types.GetObjectType(entity) == "esv::Item" then
        return not entity.Destroyed and not entity.OffStage
    else
        if not entity.Deactivated and not entity.CF_IsStoryNPC and not entity.Dead and not entity.OffStage then
          local task = "FIXME" -- FIXME esv::TaskController::GetNextTask(character->OsirisController);
          -- esv::OsirisDisappearTask
          -- esv::OsirisFleeTask
          return task == nil or (task.Type ~= "Disappear" and task.Type ~= "Flee")
        else
            return false
        end
    end
end

--- @param status EsvStatus
function ShouldDeleteStatus(status)
    if status.RequestDelete then
        return true
    end

    local target = Ext.Entity.GetGameObject(status.TargetHandle)
    local combatComp = Ext.Entity.GetCombatComponent(target.Base.Entity:GetComponent("Combat"))

    if combatComp ~= nil and combatComp.CombatAndTeamIndex.CombatId == 0 and status.RequestDeleteAtTurnEnd then
        return true
    end

    if target ~= nil and not CanEntityFight(target, combatComp) then
        return status.RequestDeleteAtTurnEnd
    else
        return false
    end
end

--- @param character EsvCharacter
function IsResistingDeath(character)
    for i,status in pairs(character:GetStatusObjects()) do
        if status.Started and status.IsResistingDeath and not ShouldDeleteStatus(status) then
            return true
        end
    end

    return false
end

--- @param causeType CauseType
function IsSurfaceCauseType(causeType)
    return causeType == "SurfaceMove" or causeType == "SurfaceCreate" or causeType == "SurfaceStatus"
end

--- @param target EsvCharacter
--- @param deathType StatsDeathType
--- @param damageType StatsDamageType
--- @param attackDirection number
--- @param impactDirection vec3
--- @param attacker EsvCharacter
--- @param inflicter EsvCharacter
--- @param disputeTarget EsvCharacter
function ApplyDying(target, deathType, damageType, attackDirection, impactDirection, attacker, inflicter, disputeTarget)
    --- @type EsvStatusDying
    local dying = target.DelayedDyingStatus
    if dying == nil then
        dying = Ext.PrepareStatus(target.Handle, "DYING", -1.0)
    end
    
    if target.HasOwner then
        deathType = "Lifetime"
    end

    dying.DeathType = deathType
    dying.AttackDirection = attackDirection
    dying.ImpactDirection = impactDirection
    dying.RequestClientSync = true
    dying.SourceType = damageType
    dying.SourceHandle = attacker and attacker.Handle
    dying.InflicterHandle = inflicter and inflicter.Handle
    dying.DisputeTargetHandle = disputeTarget and disputeTarget.Handle
    
    if target.DelayDeathCount then
        -- FIXME allow binding DelayedDyingStatus
        target.DelayedDyingStatus = dying
    else
        Ext.ApplyStatus(dying)
        -- FIXME allow binding DelayedDyingStatus
        target.DelayedDyingStatus = nil
    end
end


--- @param object EsvCharacter|EsvItem|nil
function MakeNameGuid(object)
    if object ~= nil then
        return object.CurrentTemplate.Name .. "_" .. object.MyGuid
    else
        return "NULL_00000000-0000-0000-0000-000000000000"
    end
end


--- @param status EsvStatusHeal
--- @param target EsvCharacter
function HealEffectZombie(status, target)
    if target.Stats.TALENT_Zombie then
        status.HealEffect = "Decaying"
    end
end

--- @param status EsvStatusHeal
--- @param target EsvCharacter
function HealEffectDecaying(status, target)
    if target:GetStatusByType("DECAYING_TOUCH") ~= nil then
        status.HealEffect = "Decaying"
    end
end

HealEffectTable = {
    [Ext.Enums.HealEffect.Heal] = HealEffectZombie,
    [Ext.Enums.HealEffect.Behavior] = HealEffectZombie,
    [Ext.Enums.HealEffect.Unknown4] = HealEffectZombie,
    [Ext.Enums.HealEffect.NegativeDamage] = HealEffectDecaying,
    [Ext.Enums.HealEffect.HealSharing] = HealEffectZombie,
    [Ext.Enums.HealEffect.Necromantic] = HealEffectDecaying
}


--- @param character CDivinityStatsCharacter
function ComputeHydrosophistHealBonus(character)
    return character.WaterSpecialist * Ext.ExtraData.SkillAbilityVitalityRestoredPerPoint
end

--- @param character CDivinityStatsCharacter
function ComputePhysicalArmorHealBonus(character)
    return character.EarthSpecialist * Ext.ExtraData.SkillAbilityArmorRestoredPerPoint
end

--- @param character CDivinityStatsCharacter
function ComputeMagicArmorHealBonus(character)
    return character.WaterSpecialist * Ext.ExtraData.SkillAbilityArmorRestoredPerPoint
end


HealBonusTable = {
    [Ext.Enums.StatusHealType.Vitality] = function (caster)
        return ComputeHydrosophistHealBonus(caster.Stats)
    end,
    [Ext.Enums.StatusHealType.PhysicalArmor] = function (caster)
        return ComputePhysicalArmorHealBonus(caster.Stats)
    end,
    [Ext.Enums.StatusHealType.MagicArmor] = function (caster)
        return ComputeMagicArmorHealBonus(caster.Stats)
    end,
}


--- @param amount number
--- @param healType StatusHealType
--- @param source CDivinityStatsCharacter
function ComputeHealBonus(amount, healType, source)
    if source ~= nil and Ext.Types.GetObjectType(source) == "CDivinityStatsCharacter" then
        local computeBonus = HealBonusTable[healType]
        if computeBonus ~= nil then
            return math.ceil(computeBonus(source) * amount / 100)
        end
    end

    return 0
end


--- @param target EsvCharacter|EsvItem
--- @param amount number
--- @param healType StatusHealType
--- @param healEffect HealEffect
--- @param causeType CauseType
--- @param source EsvCharacter|EsvItem
function ApplyHeal(target, amount, healType, healEffect, causeType, source)
    --- @type EsvStatusHeal
    local heal = Ext.PrepareStatus(target.Handle, "HEAL", -1.0)
    if source ~= nil then
        heal.StatusSourceHandle = source.Handle
    end
    heal.CauseType = causeType
    heal.HealAmount = amount + ComputeHealBonus(amount, healType, source and source.Stats)
    heal.HealEffect = healEffect
    heal.HealType = healType

    local applyEffect = HealEffectTable[healEffect]
    if applyEffect ~= nil then
        applyEffect(heal, target)
    end

    Ext.ApplyStatus(heal)
end


--- @param target EsvCharacter
--- @param hit StatsHitDamageInfo
--- @param attacker EsvCharacter|EsvItem
--- @param causeType CauseType
--- @param impactDirection vec3
--- @param enterCombat boolean
function ApplyDamage(target, hit, attacker, causeType, impactDirection, enterCombat)
  if not target.Activated and target.OffStage then
      Ext.Utils.PrintWarning("Trying to damage offstage character")
      return
  end

  if target.Stats == nil then
      Ext.Utils.PrintWarning("Could not find Stats for character")
      return
  end

  if target.Dead then
      return
  end

  local stats = target.Stats
  
  if not hit.FromShacklesOfPain then
    local shacklesStatus = target:GetStatus("SHACKLES_OF_PAIN_CASTER")
    if shacklesStatus ~= nil then
        -- FIXME does not exist!
        StatusStacklesOfPainCaster__ApplyDamage(shacklesStatus, hit, causeType);
    end
  end
  
  -- TODO - SummonLifeLinkModifier skipped



  local initialVitality = stats.CurrentVitality
  local initialArmor = stats.CurrentArmor
  local initialMagicArmor = stats.CurrentMagicArmor
  local totalDamageDone = hit.TotalDamageDone

  local damageList = Ext.Stats.NewDamageList()
  damageList:Merge(hit.DamageList)

  local hasDamage = false
  for i,damage in pairs(damageList:AsArray()) do
    if damage.Amount >= 0 then
        hasDamage = true
    end
  end

  damageList:AggregateSameTypeDamages()

  local MagicalDamageTypes = {
    Physical = false,
    Corrosive = false,
    Magic = true,
    Fire = true,
    Air = true,
    Water = true,
    Earth = true,
    Poison = true
  }
  
    for i,damage in pairs(damageList:AsArray()) do
        if damage.Amount > 0 then
            local isMagicDamage = MagicalDamageTypes[damage.DamageType]

            if damage.DamageType == "Sulfur" then
                isMagicDamage = stats.MagicalSulfur
            end

            local armorDmg = 0
            if isMagicDamage then
                stats.MagicArmorAfterHitCooldownMultiplier = Ext.ExtraData.MagicArmorAfterHitCooldown
                armorDmg = math.min(damage.Amount, stats.CurrentMagicArmor)
                stats.CurrentMagicArmor = stats.CurrentMagicArmor - armorDmg
            elseif isMagicDamage == false then
                stats.ArmorAfterHitCooldownMultiplier = Ext.ExtraData.ArmorAfterHitCooldown
                armorDmg = math.min(damage.Amount, stats.CurrentArmor)
                stats.CurrentArmor = stats.CurrentArmor - armorDmg
            end

            totalDamageDone = totalDamageDone - armorDmg
            damage.Amount = damage.Amount - armorDmg
        end
    end
  



    local armorAfterDmg = stats.CurrentArmor
    local magicArmorAfterDmg = stats.CurrentMagicArmor

    if initialArmor > armorAfterDmg then
        hit.DamagedPhysicalArmor = true
    end
    
    if initialMagicArmor > magicArmorAfterDmg then
        hit.DamagedMagicArmor = true
    end

    if initialArmor <= 0 then
        hit.DamageList:Clear("Corrosive")
    end

    if initialMagicArmor <= 0 then
        hit.DamageList:Clear("Magic")
    end
    
    if Ext.Types.GetObjectType(attacker) == "esv::Item" then
        if attacker.InUseByCharacterHandle ~= nil then
            attacker = Ext.Entity.GetCharacter(attacker.InUseByCharacterHandle)
        else
            attacker = nil
        end
    end


    local attackerOwner = attacker
    if attacker ~= nil and attacker.OwnerHandle ~= nil and attacker.HasOwner then
        attackerOwner = Ext.Entity.GetCharacter(attacker.OwnerHandle)
    end

    if totalDamageDone < 0 then
        ApplyHeal(target, -totalDamageDone, "Vitality", "NegativeDamage", causeType, target)
    else

        local overDamage = math.max(totalDamageDone - initialVitality, 0)

        local newVitality = math.min(math.max(initialVitality - totalDamageDone, 0), stats.MaxVitality)
        if newVitality ~= initialVitality then
            stats.CurrentVitality = newVitality
            hit.DamagedVitality = true

            if not target.IsGameMaster then
                local changePercent = math.round((newVitality - initialVitality) / stats.MaxVitality)
                Osi.CharacterVitalityChanged:Defer(MakeNameGuid(target), changePercent)
            end
        end

        local combatComp = Ext.Entity.GetCombatComponent(target.Base.Entity:GetComponent("Combat"))
        local throwReceivedDamage = false
        local lastDamageType = "Physical"

        if true then -- FIXME: if ( this->PlanManager )
            local maxVitality = target.Stats.MaxVitality
            local behaviorVitality = initialVitality

            for i,damage in pairs(damageList:AsArray()) do
                lastDamageType = damage.DamageType

                if damage.Amount > 0 then
                    -- FIXME
                    -- esv::PlanManager::ThrowOnDamage(this->PlanManager, damage.DamageType, damage.Amount / maxVitality, attacker)
                    if combatComp ~= nil and combatComp.CombatData ~= nil then
                        local found = false
                        local hasBeenHitBy = combatComp.CombatData.HasBeenHitBy
                        for i,damageType in pairs(hasBeenHitBy) do
                            if damageType == damage.DamageType then
                                found = true
                            end
                        end

                        if not found then
                            hasBeenHitBy[3] = hasBeenHitBy[2]
                            hasBeenHitBy[2] = hasBeenHitBy[1]
                            hasBeenHitBy[1] = damage.DamageType
                        end
                    end

                    throwReceivedDamage = true

                    behaviorVitality = behaviorVitality - damage.Amount
                    if behaviorVitality <= 0 then
                        break
                    end
                end
            end
        end

        local isDying = false

        if stats.CurrentVitality <= 0 then
            local canResistDeath = combatComp ~= nil and combatComp.CanUseResistDeadTalent
            local isResistingDeath = IsResistingDeath(target)

            if canResistDeath and stats.TALENT_ResistDead then
                if not isResistingDeath then
                    combatComp.CanUseResistDeadTalent = false
                end
            elseif not isResistingDeath then
                ApplyDying(target, hit.DeathType, lastDamageType, hit.AttackDirection, impactDirection, attacker, attacker, attackerOwner)

                isDying = true

                if attacker ~= nil and attacker.Stats.TALENT_PainDrinker then
                    local overDamagePercent = math.min(math.max(overDamage / GetAverageLevelDamage(attacker.Stats.Level), 0.1), 1.0)
                    local painDrinker = Ext.PrepareStatus(attacker, "PAIN_DRINKER", -1.0)
                    if painDrinker ~= nil then
                        painDrinker.StatsMultiplier = overDamagePercent
                        Ext.ApplyStatus(painDrinker)
                    end
                end
            else
                local healAmount
                if isResistingDeath then
                    healAmount = 1
                else
                    healAmount = math.max(math.round(stats.MaxVitality / 100.0 * Ext.ExtraData.TalentResistDeathVitalityPercentage), 1)
                end

                ApplyHeal(target, healAmount, "Vitality", "ResistDeath", "None", nil)

                if not isResistingDeath then
                    local text = Ext.L10N.GetTranslatedString("hf5114b41g1b8ag4d6dgb571gf9417296a15f", "[1] resisted Death!")
                    text = text:gsub("[1]", target.DisplayName)
                    -- FIXME
                    -- sub_181001B30(text, 11, -1);
                    -- CombatLogProtocol_AddText2(target.Handle, text, 0, 0, -1, 1)
                    -- OverheadTextManager__AddText(target.Handle, 0, text, 3, "", -1, 0)
                end
            end
        end

        if throwReceivedDamage then
            local vitalityPercent = math.round(totalDamageDone / stats.MaxVitality)
            Osi.CharacterReceivedDamage:Defer(MakeNameGuid(target), vitalityPercent, MakeNameGuid(attacker))
        end

        if enterCombat then
            local level = Ext.Entity.GetCurrentLevel()
            if level ~= nil and level.ShroudManager ~= nil and not isDying and attacker ~= nil and not attacker.Dead and not attacker.OffStage then
                local targetRegion = level.ShroudManager.ShroudData:GetByteAtPos("RegionMask", target.WorldPos[1], target.WorldPos[3])
                local attackerRegion = level.ShroudManager.ShroudData:GetByteAtPos("RegionMask", attacker.WorldPos[1], attacker.WorldPos[3])
                if targetRegion == attackerRegion then
                    Ext.Combat.TryEnterCombat(target, attacker, true, true)
                end
            end
        end
    end

    if hasDamage then
        if attacker ~= nil and Ext.Types.GetObjectType(attacker) == "esv::Character" then
            -- FIXME implement
            if false and AlignmentContainer__IsEnemy(attacker, target, 0, 0) then
                local attackerParent = attacker
                attacker.DamageCounter = attacker.DamageCounter + hit.TotalDamageDone
                while attackerParent.HasOwner and attackerParent.OwnerHandle ~= nil do
                    attackerParent = Ext.Entity.GetCharacter(attackerParent.OwnerHandle)
                    attackerParent.DamageCounter = attackerParent.DamageCounter + hit.TotalDamageDone
                end
            end

            -- FIXME implement
            --if ( esv::gEoCServer->ArenaManager ) then
            --  this->VMT->GetHandle(this, &v152);
            --  sub_180AA5680(v115->ArenaManager, attacker, target.Handle, hit.TotalDamageDone, 0)
            --end

            if not IsSurfaceCauseType(causeType) or not attacker.IsPlayer or not target.IsPlayer then
                Osi.AttackedByObject:Defer(MakeNameGuid(target), MakeNameGuid(attackerOwner), "", causeType, MakeNameGuid(attacker))
            end
        elseif attacker ~= nil then
            Osi.AttackedByObject:Defer(MakeNameGuid(target), MakeNameGuid(attacker), "", causeType, MakeNameGuid(nil))
        end
    end
  
    hit.DamageDealt = math.max(initialVitality - math.max(stats.CurrentVitality, 0), 0) + 
        math.max(initialArmor - math.max(stats.CurrentArmor, 0), 0) +
        math.max(initialMagicArmor - math.max(stats.CurrentMagicArmor, 0), 0)
  
    if hit.ProcWindWalker then
        local windWalker = target:GetStatus("WIND_WALKER")
        if windWalker ~= nil then
            -- FIXME
            -- esv::StatusWindWalker::Apply(windWalker)
        end
    end
  
    if attacker ~= nil and hit.CounterAttack then
        -- FIXME
        -- bsAttackCharacter = j_esv::BehaviourMachine::CreateBehaviour(this->BehaviourMachine, 3);
        -- esv::BSAttackCharacter::Setup(bsAttackCharacter, attackerHandle, attacker.WorldPos, 1, 1);
        -- esv::BSAttackCharacter::CounterAttack(bsAttackCharacter);
    end
end
