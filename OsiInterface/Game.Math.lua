local math = math
local table = table
local error = error
local pairs = pairs
local Ext = Ext

Game = {
    Math = {}
}

_ENV = Game.Math

DamageTypeToDeathTypeMap = {
    Physical = "Physical",
    Piercing = "Piercing",
    Fire = "Incinerate",
    Air = "Electrocution",
    Water = "FrozenShatter",
    Earth = "PetrifiedShatter",
    Poison = "Acid"
}

function DamageTypeToDeathType(damageType)
    local deathType = DamageTypeToDeathTypeMap[damageType]
    if deathType ~= nil then
        return deathType
    else
        return "Sentinel"
    end
end

function IsRangedWeapon(item)
    local type = item.WeaponType
    return type == "Bow" or type == "Crossbow" or type == "Wand" or type == "Rifle"
end

function ScaledDamageFromPrimaryAttribute(primaryAttr)
    return (primaryAttr - Ext.ExtraData.AttributeBaseValue) * Ext.ExtraData.DamageBoostFromAttribute
end

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

function GetSkillAttributeDamageScale(skill, attacker)
    if attacker == nil or skill.UseWeaponDamage == "Yes" or skill.Ability == 0 then
        return 1.0
    else
        local primaryAttr = GetPrimaryAttributeAmount(skill, attacker)
        return 1.0 + ScaledDamageFromPrimaryAttribute(primaryAttr)
    end
end

function GetDamageMultipliers(skill, stealthed)
    local stealthDamageMultiplier = 1.0
    if stealthed then
        stealthDamageMultiplier = skill['Stealth Damage Multiplier'] * 0.01
    end

    -- FIXME math /targetDistance/
    local targetDistance = 1.0

    local distanceDamageMultiplier = 1.0
    if targetDistance > 1.0 then
        distanceDamageMultiplier = Ext.Round(targetDistance) * skill['Distance Damage Multiplier'] * 0.01 + 1
    end

    local damageMultiplier = skill['Damage Multiplier'] * 0.01
    return stealthDamageMultiplier * distanceDamageMultiplier * damageMultiplier
end

function GetVitalityBoostByLevel(level)
    local extra = Ext.ExtraData
    local expGrowth = extra.VitalityExponentialGrowth
    local growth = expGrowth ^ level

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
    return Ext.Round(vit / 5.0) * 5.0
end

function GetLevelScaledDamage(level)
    local vitalityBoost = GetVitalityBoostByLevel(level)
    return vitalityBoost / (((level - 1) * Ext.ExtraData.VitalityToDamageRatioGrowth) + Ext.ExtraData.VitalityToDamageRatio)
end

function GetAverageLevelDamage(level)
    local scaled = GetLevelScaledDamage(level)
    return ((level * Ext.ExtraData.ExpectedDamageBoostFromAttributePerLevel) + 1.0) * scaled
        * ((level * Ext.ExtraData.ExpectedDamageBoostFromSkillAbilityPerLevel) + 1.0)
end

function GetLevelScaledWeaponDamage(level)
    local scaledDmg = GetLevelScaledDamage(level)
    return ((level * Ext.ExtraData.ExpectedDamageBoostFromWeaponAbilityPerLevel) + 1.0) * scaledDmg
end

function GetLevelScaledMonsterWeaponDamage(level)
    local weaponDmg = GetLevelScaledWeaponDamage(level)
    return ((level * Ext.ExtraData.MonsterDamageBoostPerLevel) + 1.0) * weaponDmg
end

DamageBoostTable = {
    Physical = function (character)
        return character.WarriorLore * Ext.ExtraData.SkillAbilityPhysicalDamageBoostPerPoint
    end,
    Fire = function (character)
        return character.FireSpecialist * Ext.ExtraData.SkillAbilityFireDamageBoostPerPoint
    end,
    Air = function (character)
        return character.AirSpecialist * Ext.ExtraData.SkillAbilityAirDamageBoostPerPoint
    end,
    Water = function (character)
        return character.WaterSpecialist * Ext.ExtraData.SkillAbilityWaterDamageBoostPerPoint
    end,
    Earth = function (character)
        return character.EarthSpecialist * Ext.ExtraData.SkillAbilityPoisonAndEarthDamageBoostPerPoint
    end,
    Poison = function (character)
        return character.EarthSpecialist * Ext.ExtraData.SkillAbilityPoisonAndEarthDamageBoostPerPoint
    end
}

function GetDamageBoostByType(character, damageType)
    local boostFunc = DamageBoostTable[damageType]
    if boostFunc ~= nil then
        return boostFunc(character) / 100.0
    else
        return 0.0
    end
end

function ApplyDamageBoosts(character, damageList)
    for i, damage in pairs(damageList:ToTable()) do
        local boost = GetDamageBoostByType(character, damage.DamageType)
        if boost > 0.0 then
            damageList:Add(damage.DamageType, Ext.Round(damage.Amount * boost))
        end
    end
end

DamageSourceCalcTable = {
    BaseLevelDamage = function (attacker, target, level)
        return GetLevelScaledDamage(level)
    end,
    AverageLevelDamge = function (attacker, target, level)
        return GetAverageLevelDamage(level)
    end,
    MonsterWeaponDamage = function (attacker, target, level)
        return GetLevelScaledMonsterWeaponDamage(level)
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
        error("SourceShieldPhysicalArmor NOT IMPLEMENTED YET")
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

function CalculateBaseDamage(skillDamageType, attacker, target, level)
    return DamageSourceCalcTable[skillDamageType](attacker, target, level)
end

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

function GetWeaponAbility(character, weapon)
    if weapon == nil then
        return nil
    end

    local offHandWeapon = character.OffHandWeapon
    if offHandWeapon ~= nil and IsRangedWeapon(weapon) and IsRangedWeapon(offHandWeapon) then
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

function ComputeWeaponCombatAbilityBoost(character, weapon)
    local abilityType = GetWeaponAbility(character, weapon)

    if abilityType == "SingleHanded" or abilityType == "TwoHanded" or abilityType == "Ranged" or abilityType == "DualWielding" then
        local abilityLevel = character[abilityType]
        return abilityLevel * Ext.ExtraData.CombatAbilityDamageBonus
    else
        return 0
    end
end

function GetWeaponScalingRequirement(weapon)
    local requirementName
    local largestRequirement = -1

    for i, requirement in pairs(weapon.Requirements) do
        local reqName = requirement.Name
        if not requirement.Not and requirement.Param > largestRequirement and
            (reqName == "Strength" or reqName == "Finesse" or reqName == "Constitution" or
            reqName == "Memory" or reqName == "Wits") then
            requirementName = reqName
            largestRequirement = requirement.Param
        end
    end

    return requirementName
end

function GetItemRequirementAttribute(character, weapon)
    local attribute = GetWeaponScalingRequirement(weapon)
    if attribute ~= nil then
        return character[attribute]
    else
        return 0
    end
end

function ComputeWeaponRequirementScaledDamage(character, weapon)
    local scalingReq = GetWeaponScalingRequirement(weapon)
    if scalingReq ~= nil then
        return ScaledDamageFromPrimaryAttribute(character[scalingReq]) * 100.0
    else
        return 0
    end
end

function ComputeBaseWeaponDamage(weapon)
    local damages = {}
    local stats = weapon.DynamicStats
    local baseStat = stats[1]
    local baseDmgFromBase = baseStat.DamageFromBase * 0.01
    local baseMinDamage = baseStat.MinDamage
    local baseMaxDamage = baseStat.MaxDamage
    local damageBoost = 0

    for i, stat in pairs(stats) do
        if stat.StatsType == "Weapon" and stat.DamageType ~= "None" then
            local dmgType = stat.DamageType
            local dmgFromBase = stat.DamageFromBase * 0.01
            local minDamage = stat.MinDamage
            local maxDamage = stat.MaxDamage

            if dmgFromBase ~= 0 then
                if stat == baseStat then
                    if baseMinDamage ~= 0 then
                        minDamage = math.max(dmgFromBase * baseMinDamage, 1)
                    end
                    if baseMaxDamage ~= 0 then
                        maxDamage = math.max(dmgFromBase * baseMaxDamage, 1.0)
                    end
                else
                    minDamage = math.max(dmgFromBase * dmgFromBase * baseMinDamage, 1.0)
                    maxDamage = math.max(dmgFromBase * dmgFromBase * baseMaxDamage, 1.0)
                end
            end

            if minDamage > 0 then
                maxDamage = math.max(maxDamage, minDamage + 1.0)
            end

            damageBoost = damageBoost + stat.DamageBoost

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

    return damages, damageBoost
end

-- from CDivinityStats_Character::CalculateWeaponDamageInner and CDivinityStats_Item::ComputeScaledDamage
function CalculateWeaponScaledDamage(character, weapon, damageList, noRandomization)
    local damages, damageBoost = ComputeBaseWeaponDamage(weapon)

    local abilityBoosts = character.DamageBoost 
        + ComputeWeaponCombatAbilityBoost(character, weapon)
        + ComputeWeaponRequirementScaledDamage(character, weapon)
    abilityBoosts = math.max(abilityBoosts + 100.0, 0.0) / 100.0

    local boost = 1.0 + damageBoost * 0.01
    if character.Sneaking then
        boost = boost + Ext.ExtraData['Sneak Damage Multiplier']
    end

    for damageType, damage in pairs(damages) do
        local min = math.ceil(damage.Min * boost * abilityBoosts)
        local max = math.ceil(damage.Max * boost * abilityBoosts)

        local randRange = 1
        if max - min >= 1 then
            randRange = max - min
        end

        local finalAmount
        if noRandomization then
            finalAmount = min + math.floor(randRange / 2)
        else
            finalAmount = min + Ext.Random(0, randRange)
        end

        damageList:Add(damageType, finalAmount)
    end
end

function CalculateWeaponDamage(attacker, weapon, noRandomization)
    local damageList = Ext.NewDamageList()

    CalculateWeaponScaledDamage(attacker, weapon, damageList, noRandomization)

    local offHand = attacker.OffHandWeapon
    if weapon ~= offHand and false then -- Temporarily off
        local bonusWeapons = attacker.BonusWeapons -- FIXME - enumerate BonusWeapons /multiple/ from character stats!
        for i, bonusWeapon in pairs(bonusWeapons) do
            -- FIXME Create item from bonus weapon stat and apply attack as item???
            error("BonusWeapons not implemented")
            local bonusWeaponStats = CreateBonusWeapon(bonusWeapon)
            CalculateWeaponScaledDamage(attacker, bonusWeaponStats, damageList, noRandomization)
        end
    end

    ApplyDamageBoosts(attacker, damageList)

    if weapon == offHand then
        damageList:Multiply(Ext.ExtraData.DualWieldingDamagePenalty)
    end

    return damageList
end

function CustomGetSkillDamage(skill, attacker, isFromItem, stealthed, attackerPos, targetPos, level, noRandomization)
    if attacker ~= nil and level < 0 then
        level = attacker.Level
    end

    local damageMultiplier = skill['Damage Multiplier'] * 0.01
    local damageMultipliers = GetDamageMultipliers(skill, stealthed)
    local skillDamageType = nil

    if level == 0 then
        level = skill.OverrideSkillLevel
        if level == 0 then
            level = skill.Level
        end
    end

    local damageList = Ext.NewDamageList()

    if damageMultiplier <= 0 then
        return
    end

    if skill.UseWeaponDamage == "Yes" then
        local damageType = skill.DamageType
        if damageType == "None" or damageType == "Sentinel" then
            damageType = nil
        end

        local weapon = attacker.MainWeapon
        local offHand = attacker.OffHandWeapon

        if weapon ~= nil then
            local mainDmgs = CalculateWeaponDamage(attacker, weapon, noRandomization)
            mainDmgs:Multiply(damageMultipliers)
            if damageType ~= nil then
                mainDmgs:ConvertDamageType(damageType)
            end
            damageList:Merge(mainDmgs)
        end

        if offHand ~= nil and IsRangedWeapon(weapon) and IsRangedWeapon(offHand) then
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

        local baseDamage = CalculateBaseDamage(skill.Damage, attacker, 0, level)
        local damageRange = skill['Damage Range']
        local randomMultiplier
        if noRandomization then
            randomMultiplier = 0.0
        else
            randomMultiplier = 1.0 + (Ext.Random(0, damageRange) - damageRange/2) * 0.01
        end

        -- FIXME damage types???
        local attrDamageScale
        local skillDamage = skill.Damage
        if skillDamage == "BaseLevelDamage" or skillDamage == "AverageLevelDamge" then
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

        local finalDamage = baseDamage * randomMultiplier * attrDamageScale * damageMultipliers * damageBoost
        finalDamage = math.max(Ext.Round(finalDamage), 1)
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

-- FIXME - impl GetCriticalHitMultiplier

HitFlag = {
    Hit = 1,
    Blocked = 2,
    Dodged = 4,
    Missed = 8,
    CriticalHit = 0x10,
    AlwaysBackstab = 0x20,
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

function GetResistance(character, type)
    if type == "None" or type == "Chaos" then
        return 0
    end

    return character[type .. "Resistance"]
end

function ApplyHitResistances(character, damageList)
    for i,damage in pairs(damageList:ToTable()) do
        local resistance = GetResistance(character, damage.DamageType)
        damageList:Add(damage.DamageType, math.floor(damage.Amount * -resistance / 100.0))
    end
end

function ApplyDamageCharacterBonuses(character, attacker, damageList)
    damageList:AggregateSameTypeDamages()
    ApplyHitResistances(character, damageList)

    ApplyDamageSkillAbilityBonuses(damageList, attacker)
end


function GetAbilityCriticalHitMultiplier(character, ability)
    if ability == "TwoHanded" then
        return Ext.Round(character.TwoHanded * Ext.ExtraData.CombatAbilityCritMultiplierBonus)
    end
        
    if ability == "RogueLore" then
        return Ext.Round(character.RogueLore * Ext.ExtraData.SkillAbilityCritMultiplierPerPoint)
    end

    return 0
end


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


function ApplyCriticalHit(hit, attacker)
    local mainWeapon = attacker.MainWeapon
    if mainWeapon ~= nil then
        hit.EffectFlags = hit.EffectFlags | HitFlag.CriticalHit;
        hit.DamageMultiplier = hit.DamageMultiplier + (GetCriticalHitMultiplier(mainWeapon, attacker, 0, 0) - 1.0)
    end
end


function ShouldApplyCriticalHit(hit, attacker, hitType, criticalRoll)
    if criticalRoll ~= "Roll" then
        return criticalRoll == "Critical"
    end

    if attacker.TALENT_Haymaker then
        return false
    end
    
    if attacker.TALENT_ViolentMagic == false or hitType ~= "Magic" then
        if (hit.EffectFlags & HitFlag.AlwaysBackstab) == 0 or hitType == "Magic" or hitType == "DoT" or hitType == "Surface" then
            return false
        end

        return math.random(0, 99) < attacker.CriticalChance
    else
        local critChance = attacker.CriticalChance
        if attacker.TALENT_ViolentMagic then
            critChance = critChance + Ext.ExtraData.TalentViolentMagicCriticalChancePercent
        end

        critChance = math.min(critChance, 1)
        return math.random(0, 99) < attacker.CriticalChance
    end
end


function ConditionalApplyCriticalHitMultiplier(hit, target, attacker, hitType, criticalRoll)
    if ShouldApplyCriticalHit(hit, attacker, hitType, criticalRoll) then
        ApplyCriticalHit(hit, attacker)
    end
end


function ApplyLifeSteal(hit, target, attacker, hitType)
    if attacker == nil or hitType == "DoT" or hitType == "Surface" then
        return
    end
    
    local magicDmg = hit.DamageList:GetByType("Magic")
    local corrosiveDmg = hit.DamageList:GetByType("Corrosive")
    local lifesteal = hit.TotalDamageDone - hit.ArmorAbsorption - corrosiveDmg - magicDmg

    if (hit.EffectFlags & (HitFlag.FromShacklesOfPain|HitFlag.NoDamageOnOwner|HitFlag.Reflection)) ~= 0 then
        local modifier = Ext.ExtraData.LifestealFromReflectionModifier
        lifesteal = math.floor(lifesteal * modifier)
    end

    if lifesteal >= target.CurrentVitality then
        lifesteal = target.CurrentVitality
    end

    if lifesteal > 0 then
        hit.LifeSteal = math.max(math.ceil(lifesteal * attacker.Lifesteal / 100), 0)
    end
end


function ApplyDamagesToHitInfo(damageList, hit)
    for i,damage in pairs(damageList:ToTable()) do
        if damage.DamageType == "Chaos" then
            hit.DamageList:Add(hit.DamageType, damage.Amount)
        else
            hit.DamageList:Add(damage.DamageType, damage.Amount)
        end
    end
end


function ComputeArmorDamage(damageList, armor)
    local absorption = 0
    for i,damage in pairs(damageList:ToTable()) do
        local type = damage.DamageType
        if type == "Corrosive" or type == "Physical" or type == "Sulfur" then
            local damageAmount = math.min(armor, damage.Amount)
            armor = armor - damageAmount
            absorption = absorption + damageAmount
        end
    end

    return absorption
end


function ComputeMagicArmorDamage(damageList, magicArmor)
    local absorption = 0
    for i,damage in pairs(damageList:ToTable()) do
        local type = damage.DamageType
        if type == "Magic" or type == "Fire" or type == "Water" or type == "Air" or type == "Earth" or type == "Poison" then
            local damageAmount = math.min(magicArmor, damage.Amount)
            magicArmor = magicArmor - damageAmount
            absorption = absorption + damageAmount
        end
    end

    return absorption
end


function DoHit(hit, damageList, statusBonusDmgTypes, hitType, target, attacker)
    hit.EffectFlags = hit.EffectFlags | HitFlag.Hit;
    damageList:AggregateSameTypeDamages()
    damageList:Multiply(hit.DamageMultiplier)

    local totalDamage = 0
    for i,damage in pairs(damageList:ToTable()) do
        totalDamage = totalDamage + damage.Amount
    end

    if totalDamage < 0 then
        damageList:Clear()
    end

    ApplyDamageCharacterBonuses(target, attacker, damageList)
    damageList:AggregateSameTypeDamages()
    hit.DamageList = Ext.NewDamageList()

    for i,damageType in pairs(statusBonusDmgTypes) do
        damageList.Add(damageType, math.ceil(totalDamage * 0.1))
    end

    ApplyDamagesToHitInfo(damageList, hit)
    hit.ArmorAbsorption = hit.ArmorAbsorption + ComputeArmorDamage(damageList, target.CurrentArmor)
    hit.ArmorAbsorption = hit.ArmorAbsorption + ComputeMagicArmorDamage(damageList, target.CurrentMagicArmor)

    if hit.TotalDamageDone > 0 then
        ApplyLifeSteal(hit, attacker, hitType)
    else
        hit.EffectFlags = hit.EffectFlags | HitFlag.DontCreateBloodSurface
    end

    if hitType == "Surface" then
        hit.EffectFlags = hit.EffectFlags | HitFlag.Surface
    end

    if hitType == "DoT" then
        hit.EffectFlags = hit.EffectFlags | HitFlag.DoT
    end
end


function GetAttackerDamageMultiplier(attacker, target, highGround)
    if target == nil then
        return 0.0
    end

    if highGround == "HighGround" then
        local highGroundBonus = attacker.RangerLore * Ext.ExtraData.HighGroundBaseDamageBonus
        return math.max(highGroundBonus * Ext.ExtraData.SkillAbilityHighGroundBonusPerPoint, 0.0)
    elseif highGround == "LowGround" then
        return Ext.ExtraData.LowGroundBaseDamagePenalty
    else
        return 0.0
    end
end


function DamageItemDurability(character, item)
    local degradeSpeed = 0
    for i,stats in pairs(item.DynamicStats) do
        degradeSpeed = degradeSpeed + stats.DurabilityDegradeSpeed
    end

    if degradeSpeed > 0 then
        local durability = math.max(0, item.Durability)
        item.Durability = durability
        item.ShouldSyncStats = 1

        if durability == 0 then
            -- FIXME not implemented yet
            -- Ext.ReevaluateItems(character)
        end
    end
end


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

function CalculateHitChance(attacker, target)
    if attacker.TALENT_Haymaker then
        return 100
    end

    local ranged = IsRangedWeapon(attacker.MainWeapon.WeaponType)
    local accuracy = attacker.Accuracy
    local dodge = 0
    if (not attacker.Invisible or ranged) and target.IsIncapacitatedRefCount == 0 then
        dodge = target.Dodge
    end

    local chanceToHit1 = Ext.Round(((100.0 - dodge) * accuracy) / 100)
    chanceToHit1 = math.max(0, math.min(100, chanceToHit1))
    return chanceToHit1 + attacker.ChanceToHitBoost
end

function IsInFlankingPosition(target, attacker)
    -- FIXME - IsInFlankingPosition
    return false
end

function CanBackstab(target, attacker)
    -- FIXME - CanBackstab
    return false
end

function ComputeCharacterHit(target, attacker, weapon, damageList, hitType, rollForDamage, forceReduceDurability, hit, alwaysBackstab, highGroundFlag, criticalRoll)

    hit.DamageMultiplier = 1.0
    local statusBonusDmgTypes = {}
    
    if attacker == nil then
        DoHit(hit, damageList, statusBonusDmgTypes, hitType, target, attacker)
        return hit
    end

    hit.DamageMultiplier = 1.0 + GetAttackerDamageMultiplier(target, attacker, highGroundFlag)
    if hitType == "Magic" or hitType == "Surface" or hitType == "DoT" or hitType == "Reflected" then
        ConditionalApplyCriticalHitMultiplier(hit, target, attacker, hitType, criticalRoll)
        DoHit(hit, damageList, statusBonusDmgTypes, hitType, target, attacker)
        return hit
    end

    local backstabbed = false
    if alwaysBackstab or (weapon ~= nil and weapon.WeaponType == "Knife" and CanBackstab(target, attacker)) then
        hit.EffectFlags = hit.EffectFlags | HitFlag.AlwaysBackstab
        backstabbed = true
    end

    if hitType == "Melee" then
        if IsInFlankingPosition(target, attacker) then
            hit.EffectFlags = hit.EffectFlags | HitFlag.Flanking
        end
    
        -- Apply Sadist talent
        if attacker.TALENT_Sadist then
            if (hit.EffectFlags & HitFlag.Poisoned) ~= 0 then
                table.insert(statusBonusDmgTypes, "Poison")
            end
            if (hit.EffectFlags & HitFlag.Burning) ~= 0 then
                table.insert(statusBonusDmgTypes, "Fire")
            end
            if (hit.EffectFlags & HitFlag.Bleeding) ~= 0 then
                table.insert(statusBonusDmgTypes, "Physical")
            end
        end
    end

    if attacker.TALENT_Damage then
        hit.DamageMultiplier = hit.DamageMultiplier + 0.1
    end

    local hitBlocked = false

    if rollForDamage then
        local hitChance = CalculateHitChance(target, attacker)
        local hitRoll = math.random(0, 99)
        if hitRoll >= hitChance then
            if target.TALENT_RangerLoreEvasionBonus and hitRoll < hitChance + 10 then
                hit.EffectFlags = hit.EffectFlags | HitFlag.Dodged
            else
                hit.EffectFlags = hit.EffectFlags | HitFlag.Missed
            end
            hitBlocked = true
        else
            local blockChance = target.BlockChance
            if not backstabbed and blockChance > 0 and math.random(0, 99) <= blockChance then
                hit.EffectFlags = hit.EffectFlags | HitFlag.Blocked;
                hitBlocked = true
            end
        end
    end

    if weapon ~= nil and weapon.Name ~= "DefaultWeapon" and hitType ~= "Magic" and forceReduceDurability and (hit.EffectFlags & (HitFlag.Missed|HitFlag.Dodged)) == 0 then
        ConditionalDamageItemDurability(attacker, weapon)
    end

    if not hitBlocked then
        ConditionalApplyCriticalHitMultiplier(hit, target, attacker, hitType, criticalRoll)
        DoHit(hit, damageList, statusBonusDmgTypes, hitType, target, attacker)
    end

    return hit
end
