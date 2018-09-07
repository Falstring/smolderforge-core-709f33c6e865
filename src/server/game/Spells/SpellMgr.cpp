/*
 * Copyright (C) 2010-2012 Project SkyFire <http://www.projectskyfire.org/>
 * Copyright (C) 2010-2012 Oregon <http://www.oregoncore.com/>
 * Copyright (C) 2008-2012 TrinityCore <http://www.trinitycore.org/>
 * Copyright (C) 2005-2012 MaNGOS <http://getmangos.com/>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "Unit.h"
#include "SpellMgr.h"
#include "ObjectMgr.h"
#include "SpellAuraDefines.h"

#include "DBCStores.h"
#include "World.h"
#include "Chat.h"
#include "Spell.h"
#include "CreatureAI.h"
#include "BattlegroundMgr.h"
#include "VMapFactory.h"

bool IsAreaEffectTarget[TOTAL_SPELL_TARGETS];

SpellMgr::SpellMgr()
{
    for (int i = 0; i < TOTAL_SPELL_EFFECTS; ++i)
    {
        switch (i)
        {
            case SPELL_EFFECT_PERSISTENT_AREA_AURA: //27
            case SPELL_EFFECT_SUMMON:               //28
            case SPELL_EFFECT_TRIGGER_MISSILE:      //32
            case SPELL_EFFECT_TRANS_DOOR:           //50 summon object
            case SPELL_EFFECT_SUMMON_PET:           //56
            case SPELL_EFFECT_ADD_FARSIGHT:         //72
            case SPELL_EFFECT_SUMMON_OBJECT_WILD:   //76
            case SPELL_EFFECT_SUMMON_OBJECT_SLOT1:  //104
            case SPELL_EFFECT_SUMMON_OBJECT_SLOT2:  //105
            case SPELL_EFFECT_SUMMON_OBJECT_SLOT3:  //106
            case SPELL_EFFECT_SUMMON_OBJECT_SLOT4:  //107
            case SPELL_EFFECT_SUMMON_DEAD_PET:      //109
            case SPELL_EFFECT_TRIGGER_SPELL_2:      //151 ritual of summon
                EffectTargetType[i] = SPELL_REQUIRE_DEST;
                break;
            case SPELL_EFFECT_PARRY: // 0
            case SPELL_EFFECT_BLOCK: // 0
            case SPELL_EFFECT_SKILL: // always with dummy 3 as A
            //case SPELL_EFFECT_LEARN_SPELL: // 0 may be 5 pet
            case SPELL_EFFECT_TRADE_SKILL: // 0 or 1
            case SPELL_EFFECT_PROFICIENCY: // 0
                EffectTargetType[i] = SPELL_REQUIRE_NONE;
                break;
            case SPELL_EFFECT_ENCHANT_ITEM:
            case SPELL_EFFECT_ENCHANT_ITEM_TEMPORARY:
            case SPELL_EFFECT_DISENCHANT:
            case SPELL_EFFECT_FEED_PET:
            case SPELL_EFFECT_PROSPECTING:
                EffectTargetType[i] = SPELL_REQUIRE_ITEM;
                break;
            //caster must be pushed otherwise no sound
            case SPELL_EFFECT_APPLY_AREA_AURA_PARTY:
            case SPELL_EFFECT_APPLY_AREA_AURA_FRIEND:
            case SPELL_EFFECT_APPLY_AREA_AURA_ENEMY:
            case SPELL_EFFECT_APPLY_AREA_AURA_PET:
            case SPELL_EFFECT_APPLY_AREA_AURA_OWNER:
                EffectTargetType[i] = SPELL_REQUIRE_CASTER;
                break;
            default:
                EffectTargetType[i] = SPELL_REQUIRE_UNIT;
                break;
        }
    }

    for (int i = 0; i < TOTAL_SPELL_TARGETS; ++i)
    {
        switch (i)
        {
            case TARGET_UNIT_CASTER:
            case TARGET_UNIT_CASTER_FISHING:
            case TARGET_UNIT_MASTER:
            case TARGET_UNIT_PET:
            case TARGET_UNIT_PARTY_CASTER:
            case TARGET_UNIT_RAID_CASTER:
                SpellTargetType[i] = TARGET_TYPE_UNIT_CASTER;
                break;
            case TARGET_UNIT_MINIPET:
            case TARGET_UNIT_TARGET_ALLY:
            case TARGET_UNIT_TARGET_RAID:
            case TARGET_UNIT_TARGET_ANY:
            case TARGET_UNIT_TARGET_ENEMY:
            case TARGET_UNIT_TARGET_PARTY:
            case TARGET_UNIT_PARTY_TARGET:
            case TARGET_UNIT_CLASS_TARGET:
            case TARGET_UNIT_CHAINHEAL:
                SpellTargetType[i] = TARGET_TYPE_UNIT_TARGET;
                break;
            case TARGET_UNIT_NEARBY_ENEMY:
            case TARGET_UNIT_NEARBY_ALLY:
            case TARGET_UNIT_NEARBY_ALLY_UNK:
            case TARGET_UNIT_NEARBY_ENTRY:
            case TARGET_UNIT_NEARBY_RAID:
                SpellTargetType[i] = TARGET_TYPE_UNIT_NEARBY;
                break;
            case TARGET_UNIT_AREA_ENEMY_SRC:
            case TARGET_UNIT_AREA_ALLY_SRC:
            case TARGET_UNIT_AREA_ENTRY_SRC:
            case TARGET_UNIT_AREA_PARTY_SRC:
            case TARGET_OBJECT_AREA_SRC:
                SpellTargetType[i] = TARGET_TYPE_AREA_SRC;
                break;
            case TARGET_UNIT_AREA_ENEMY_DST:
            case TARGET_UNIT_AREA_ALLY_DST:
            case TARGET_UNIT_AREA_ENTRY_DST:
            case TARGET_UNIT_AREA_PARTY_DST:
            case TARGET_OBJECT_AREA_DST:
                SpellTargetType[i] = TARGET_TYPE_AREA_DST;
                break;
            case TARGET_UNIT_CONE_ENEMY:
            case TARGET_UNIT_CONE_ALLY:
            case TARGET_UNIT_CONE_ENTRY:
            case TARGET_UNIT_CONE_ENEMY_UNKNOWN:
                SpellTargetType[i] = TARGET_TYPE_AREA_CONE;
                break;
            case TARGET_DST_CASTER:
            case TARGET_SRC_CASTER:
            case TARGET_MINION:
            case TARGET_DEST_CASTER_FRONT_LEAP:
            case TARGET_DEST_CASTER_FRONT:
            case TARGET_DEST_CASTER_BACK:
            case TARGET_DEST_CASTER_RIGHT:
            case TARGET_DEST_CASTER_LEFT:
            case TARGET_DEST_CASTER_FRONT_LEFT:
            case TARGET_DEST_CASTER_BACK_LEFT:
            case TARGET_DEST_CASTER_BACK_RIGHT:
            case TARGET_DEST_CASTER_FRONT_RIGHT:
            case TARGET_DEST_CASTER_RANDOM:
            case TARGET_DEST_CASTER_RADIUS:
                SpellTargetType[i] = TARGET_TYPE_DEST_CASTER;
                break;
            case TARGET_DST_TARGET_ENEMY:
            case TARGET_DEST_TARGET_ANY:
            case TARGET_DEST_TARGET_FRONT:
            case TARGET_DEST_TARGET_BACK:
            case TARGET_DEST_TARGET_RIGHT:
            case TARGET_DEST_TARGET_LEFT:
            case TARGET_DEST_TARGET_FRONT_LEFT:
            case TARGET_DEST_TARGET_BACK_LEFT:
            case TARGET_DEST_TARGET_BACK_RIGHT:
            case TARGET_DEST_TARGET_FRONT_RIGHT:
            case TARGET_DEST_TARGET_RANDOM:
            case TARGET_DEST_TARGET_RADIUS:
                SpellTargetType[i] = TARGET_TYPE_DEST_TARGET;
                break;
            case TARGET_DEST_DYNOBJ_ENEMY:
            case TARGET_DEST_DYNOBJ_ALLY:
            case TARGET_DEST_DYNOBJ_NONE:
            case TARGET_DEST_DEST:
            case TARGET_DEST_TRAJ:
            case TARGET_DEST_DEST_FRONT_LEFT:
            case TARGET_DEST_DEST_BACK_LEFT:
            case TARGET_DEST_DEST_BACK_RIGHT:
            case TARGET_DEST_DEST_FRONT_RIGHT:
            case TARGET_DEST_DEST_FRONT:
            case TARGET_DEST_DEST_BACK:
            case TARGET_DEST_DEST_RIGHT:
            case TARGET_DEST_DEST_LEFT:
            case TARGET_DEST_DEST_RANDOM:
                SpellTargetType[i] = TARGET_TYPE_DEST_DEST;
                break;
            case TARGET_DST_DB:
            case TARGET_DST_HOME:
            case TARGET_DST_NEARBY_ENTRY:
                SpellTargetType[i] = TARGET_TYPE_DEST_SPECIAL;
                break;
            case TARGET_UNIT_CHANNEL:
            case TARGET_DEST_CHANNEL:
                SpellTargetType[i] = TARGET_TYPE_CHANNEL;
                break;
            default:
                SpellTargetType[i] = TARGET_TYPE_DEFAULT;
        }
    }

    for (int i = 0; i < TOTAL_SPELL_TARGETS; ++i)
    {
        switch (i)
        {
            case TARGET_UNIT_AREA_ENEMY_DST:
            case TARGET_UNIT_AREA_ENEMY_SRC:
            case TARGET_UNIT_AREA_ALLY_DST:
            case TARGET_UNIT_AREA_ALLY_SRC:
            case TARGET_UNIT_AREA_ENTRY_DST:
            case TARGET_UNIT_AREA_ENTRY_SRC:
            case TARGET_UNIT_AREA_PARTY_DST:
            case TARGET_UNIT_AREA_PARTY_SRC:
            case TARGET_UNIT_PARTY_TARGET:
            case TARGET_UNIT_PARTY_CASTER:
            case TARGET_UNIT_CONE_ENEMY:
            case TARGET_UNIT_CONE_ALLY:
            case TARGET_UNIT_CONE_ENEMY_UNKNOWN:
            case TARGET_UNIT_RAID_CASTER:
                IsAreaEffectTarget[i] = true;
                break;
            default:
                IsAreaEffectTarget[i] = false;
                break;
        }
    }
}

SpellMgr::~SpellMgr()
{
}

SpellMgr& SpellMgr::Instance()
{
    static SpellMgr spellMgr;
    return spellMgr;
}

int32 GetSpellDuration(SpellEntry const *spellInfo)
{
    if (!spellInfo)
        return 0;
    SpellDurationEntry const *du = sSpellDurationStore.LookupEntry(spellInfo->DurationIndex);
    if (!du)
        return 0;
    return (du->Duration[0] == -1) ? -1 : abs(du->Duration[0]);
}

int32 GetSpellMaxDuration(SpellEntry const *spellInfo)
{
    if (!spellInfo)
        return 0;
    SpellDurationEntry const *du = sSpellDurationStore.LookupEntry(spellInfo->DurationIndex);
    if (!du)
        return 0;
    return (du->Duration[2] == -1) ? -1 : abs(du->Duration[2]);
}

uint32 GetSpellCastTime(SpellEntry const* spellInfo, Spell const* spell)
{
    SpellCastTimesEntry const *spellCastTimeEntry = sSpellCastTimesStore.LookupEntry(spellInfo->CastingTimeIndex);

    // not all spells have cast time index and this is all is pasiive abilities
    if (!spellCastTimeEntry)
        return 0;

    int32 castTime = spellCastTimeEntry->CastTime;

    if (spell)
    {
        if (Player* modOwner = spell->GetCaster()->GetSpellModOwner())
            modOwner->ApplySpellMod(spellInfo->Id, SPELLMOD_CASTING_TIME, castTime, spell);

        if (!(spellInfo->Attributes & (SPELL_ATTR_ABILITY|SPELL_ATTR_TRADESPELL)))
            castTime = int32(castTime * spell->GetCaster()->GetFloatValue(UNIT_MOD_CAST_SPEED));
        else
        {
            if (spell->IsRangedSpell() && !spell->IsAutoRepeat())
                castTime = int32(castTime * spell->GetCaster()->m_modAttackSpeedPct[RANGED_ATTACK]);
        }
    }

    if (spellInfo->Attributes & SPELL_ATTR_RANGED && (!spell || !spell->IsAutoRepeat()))
        castTime += 500;

    return (castTime > 0) ? uint32(castTime) : 0;
}

WeaponAttackType GetWeaponAttackType(SpellEntry const* spellInfo)
{
    if (!spellInfo)
        return BASE_ATTACK;

    switch (spellInfo->DmgClass)
    {
        case SPELL_DAMAGE_CLASS_MELEE:
            if (spellInfo->AttributesEx3 & SPELL_ATTR_EX3_REQ_OFFHAND)
                return OFF_ATTACK;
            else
                return BASE_ATTACK;
            break;
        case SPELL_DAMAGE_CLASS_RANGED:
            return RANGED_ATTACK;
            break;
        default:
            // Wands
            if (spellInfo->AttributesEx2 & SPELL_ATTR_EX2_AUTOREPEAT_FLAG)
                return RANGED_ATTACK;
            else
                return BASE_ATTACK;
            break;
    }
}

bool IsPassiveSpell(uint32 spellId)
{
    SpellEntry const *spellInfo = sSpellStore.LookupEntry(spellId);
    if (!spellInfo)
        return false;
    return IsPassiveSpell(spellInfo);
}

bool IsPassiveSpell(SpellEntry const * spellInfo)
{
    if (spellInfo->Attributes & SPELL_ATTR_PASSIVE)
        return true;
    return false;
}

bool IsAggressiveSpell(SpellEntry const* spellInfo, bool triggered)
{
    switch (spellInfo->SpellFamilyName)
    {
        case SPELLFAMILY_MAGE:
        {
            // Arcane Missiles (aura application)
            if (spellInfo->SpellFamilyFlags & 0x0000000800LL)
            {
                if (!triggered)
                    return false;
            }
        }
    }

    if (spellInfo->AttributesEx3 & SPELL_ATTR_EX3_NO_INITIAL_AGGRO)
        return false;

    return true;
}

bool IsAutocastableSpell(uint32 spellId)
{
    SpellEntry const *spellInfo = sSpellStore.LookupEntry(spellId);
    if (!spellInfo)
        return false;
    if (spellInfo->Attributes & SPELL_ATTR_PASSIVE)
        return false;
    if (spellInfo->AttributesEx & SPELL_ATTR_EX_UNAUTOCASTABLE_BY_PET)
        return false;
    return true;
}

uint32 CalculatePowerCost(SpellEntry const * spellInfo, Unit const * caster, SpellSchoolMask schoolMask)
{
    // Spell drain all exist power on cast (Only paladin lay of Hands)
    if (spellInfo->AttributesEx & SPELL_ATTR_EX_DRAIN_ALL_POWER)
    {
        // If power type - health drain all
        if (spellInfo->powerType == POWER_HEALTH)
            return caster->GetHealth();
        // Else drain all power
        if (spellInfo->powerType < MAX_POWERS)
            return caster->GetPower(Powers(spellInfo->powerType));
        sLog->outError("CalculateManaCost: Unknown power type '%d' in spell %d", spellInfo->powerType, spellInfo->Id);
        return 0;
    }

    // Base powerCost
    int32 powerCost = spellInfo->manaCost;
    // PCT cost from total amount
    if (spellInfo->ManaCostPercentage)
    {
        switch (spellInfo->powerType)
        {
            // health as power used
            case POWER_HEALTH:
                powerCost += spellInfo->ManaCostPercentage * caster->GetCreateHealth() / 100;
                break;
            case POWER_MANA:
                powerCost += spellInfo->ManaCostPercentage * caster->GetCreateMana() / 100;
                break;
            case POWER_RAGE:
            case POWER_FOCUS:
            case POWER_ENERGY:
            case POWER_HAPPINESS:
                powerCost += spellInfo->ManaCostPercentage * caster->GetMaxPower(Powers(spellInfo->powerType)) / 100;
                break;
            default:
                sLog->outError("CalculateManaCost: Unknown power type '%d' in spell %d", spellInfo->powerType, spellInfo->Id);
                return 0;
        }
    }
    SpellSchools school = GetFirstSchoolInMask(schoolMask);
    // Flat mod from caster auras by spell school
    powerCost += caster->GetInt32Value(UNIT_FIELD_POWER_COST_MODIFIER + school);
    // Shiv - costs 20 + weaponSpeed*10 energy (apply only to non-triggered spell with energy cost)
    if (spellInfo->AttributesEx4 & SPELL_ATTR_EX4_SPELL_VS_EXTEND_COST)
        powerCost += caster->GetAttackTime(OFF_ATTACK)/100;
    // Apply cost mod by spell
    if (Player* modOwner = caster->GetSpellModOwner())
        modOwner->ApplySpellMod(spellInfo->Id, SPELLMOD_COST, powerCost);

    if (spellInfo->Attributes & SPELL_ATTR_LEVEL_DAMAGE_CALCULATION)
        powerCost = int32(powerCost/ (1.117f* spellInfo->spellLevel / caster->getLevel() -0.1327f));

    // PCT mod from user auras by school
    powerCost = int32(powerCost * (1.0f+caster->GetFloatValue(UNIT_FIELD_POWER_COST_MULTIPLIER+school)));
    if (powerCost < 0)
        powerCost = 0;
    return powerCost;
}

/*bool IsNoStackAuraDueToAura(uint32 spellId_1, uint32 effIndex_1, uint32 spellId_2, uint32 effIndex_2)
{
    SpellEntry const *spellInfo_1 = sSpellStore.LookupEntry(spellId_1);
    SpellEntry const *spellInfo_2 = sSpellStore.LookupEntry(spellId_2);
    if (!spellInfo_1 || !spellInfo_2) return false;
    if (spellInfo_1->Id == spellId_2) return false;

    if (spellInfo_1->Effect[effIndex_1] != spellInfo_2->Effect[effIndex_2] ||
        spellInfo_1->EffectItemType[effIndex_1] != spellInfo_2->EffectItemType[effIndex_2] ||
        spellInfo_1->EffectMiscValue[effIndex_1] != spellInfo_2->EffectMiscValue[effIndex_2] ||
        spellInfo_1->EffectApplyAuraName[effIndex_1] != spellInfo_2->EffectApplyAuraName[effIndex_2])
        return false;

    return true;
}*/

int32 CompareAuraRanks(uint32 spellId_1, uint32 effIndex_1, uint32 spellId_2, uint32 effIndex_2)
{
    SpellEntry const*spellInfo_1 = sSpellStore.LookupEntry(spellId_1);
    SpellEntry const*spellInfo_2 = sSpellStore.LookupEntry(spellId_2);
    if (!spellInfo_1 || !spellInfo_2) return 0;
    if (spellId_1 == spellId_2) return 0;

    int32 diff = spellInfo_1->EffectBasePoints[effIndex_1] - spellInfo_2->EffectBasePoints[effIndex_2];
    if (spellInfo_1->EffectBasePoints[effIndex_1] + 1 < 0 && spellInfo_2->EffectBasePoints[effIndex_2] + 1 < 0) return -diff;
    else return diff;
}

SpellSpecific GetSpellSpecific(uint32 spellId)
{
    SpellEntry const *spellInfo = sSpellStore.LookupEntry(spellId);
    if (!spellInfo)
        return SPELL_NORMAL;

    if (sSpellMgr->GetSpellCustomAttr(spellId) & SPELL_ATTR_CU_WELL_FED)
        return SPELL_WELL_FED;

    switch (spellInfo->SpellFamilyName)
    {
        case SPELLFAMILY_GENERIC:
        {
            // Food / Drinks (mostly)
            if (spellInfo->AuraInterruptFlags & AURA_INTERRUPT_FLAG_NOT_SEATED)
            {
                for (int i = 0; i < 3; i++)
                    if (spellInfo->EffectApplyAuraName[i] == SPELL_AURA_MOD_POWER_REGEN)
                        return SPELL_DRINK;
                    else if (spellInfo->EffectApplyAuraName[i] == SPELL_AURA_MOD_REGEN)
                        return SPELL_FOOD;
            }
            else
            {
                // this may be a hack
                if (spellInfo->AttributesEx2 & SPELL_ATTR_EX2_FOOD && !spellInfo->Category)
                    return SPELL_WELL_FED;
            }
            

            switch (spellInfo->Id)
            {
                case 12880: // warrior's Enrage rank 1
                case 14201: //           Enrage rank 2
                case 14202: //           Enrage rank 3
                case 14203: //           Enrage rank 4
                case 14204: //           Enrage rank 5
                case 12292: //             Death Wish
                    return SPELL_WARRIOR_ENRAGE;
                break;
                default: break;
            }
            break;
        }
        case SPELLFAMILY_MAGE:
        {
            // family flags 18(Molten), 25(Frost/Ice), 28(Mage)
            if (spellInfo->SpellFamilyFlags & 0x12040000)
                return SPELL_MAGE_ARMOR;

            if ((spellInfo->SpellFamilyFlags & 0x1000000) && spellInfo->EffectApplyAuraName[0] == SPELL_AURA_MOD_CONFUSE)
                return SPELL_MAGE_POLYMORPH;

            break;
        }
        case SPELLFAMILY_WARRIOR:
        {
            if (spellInfo->SpellFamilyFlags & 0x00008000010000LL)
                return SPELL_POSITIVE_SHOUT;
            // Sunder Armor (vs Expose Armor)
            if (spellInfo->SpellFamilyFlags & 0x00000000004000LL)
                return SPELL_ARMOR_REDUCE;

            break;
        }
        case SPELLFAMILY_WARLOCK:
        {
            // only warlock curses have this
            if (spellInfo->Dispel == DISPEL_CURSE)
                return SPELL_CURSE;

            // family flag 37 (only part spells have family name)
            if (spellInfo->SpellFamilyFlags & 0x2000000000LL)
                return SPELL_WARLOCK_ARMOR;

            //seed of corruption and corruption
            if (spellInfo->SpellFamilyFlags & 0x1000000002LL)
                return SPELL_WARLOCK_CORRUPTION;
            break;
        }
        case SPELLFAMILY_HUNTER:
        {
            // only hunter stings have this
            if (spellInfo->Dispel == DISPEL_POISON)
                return SPELL_STING;

            break;
        }
        case SPELLFAMILY_PALADIN:
        {
            if (IsSealSpell(spellInfo))
                return SPELL_SEAL;

            if (spellInfo->SpellFamilyFlags & 0x10000180LL)
                return SPELL_BLESSING;

            if ((spellInfo->SpellFamilyFlags & 0x00000820180400LL) && (spellInfo->AttributesEx3 & 0x200))
                return SPELL_JUDGEMENT;

            for (int i = 0; i < 3; i++)
            {
                // only paladin auras have this
                if (spellInfo->Effect[i] == SPELL_EFFECT_APPLY_AREA_AURA_PARTY)
                    return SPELL_AURA;
            }
            break;
        }
        case SPELLFAMILY_SHAMAN:
        {
            if (IsElementalShield(spellInfo))
                return SPELL_ELEMENTAL_SHIELD;

            break;
        }
        case SPELLFAMILY_ROGUE:
        {
            // Expose Armor (vs Sunder Armor)
            if (spellInfo->SpellFamilyFlags & 0x00000000080000LL)
                return SPELL_ARMOR_REDUCE;
            break;
        }
        case SPELLFAMILY_POTION:
            return sSpellMgr->GetSpellElixirSpecific(spellInfo->Id);
    }

    // only warlock armor/skin have this (in additional to family cases)
    if (spellInfo->SpellVisual == 130 && spellInfo->SpellIconID == 89)
    {
        return SPELL_WARLOCK_ARMOR;
    }

    // only hunter aspects have this (but not all aspects in hunter family)
    if (spellInfo->activeIconID == 122 && (GetSpellSchoolMask(spellInfo) & SPELL_SCHOOL_MASK_NATURE) &&
        (spellInfo->Attributes & 0x50000) != 0 && (spellInfo->Attributes & 0x9000010) == 0)
    {
        return SPELL_ASPECT;
    }

    for (int i = 0; i < 3; ++i)
    {
        if (spellInfo->Effect[i] == SPELL_EFFECT_APPLY_AURA)
        {
            switch (spellInfo->EffectApplyAuraName[i])
            {
                case SPELL_AURA_MOD_CHARM:
                case SPELL_AURA_MOD_POSSESS_PET:
                case SPELL_AURA_MOD_POSSESS:
                    return SPELL_CHARM;
                case SPELL_AURA_TRACK_CREATURES:
                case SPELL_AURA_TRACK_RESOURCES:
                case SPELL_AURA_TRACK_STEALTHED:
                    return SPELL_TRACKER;
            }
        }
    }

    // elixirs can have different families, but potion most ofc.
    if (SpellSpecific sp = sSpellMgr->GetSpellElixirSpecific(spellInfo->Id))
        return sp;

    return SPELL_NORMAL;
}

bool IsSingleFromSpellSpecificPerCaster(uint32 spellSpec1, uint32 spellSpec2)
{
    switch (spellSpec1)
    {
        case SPELL_SEAL:
        case SPELL_BLESSING:
        case SPELL_AURA:
        case SPELL_STING:
        case SPELL_CURSE:
        case SPELL_ASPECT:
        case SPELL_POSITIVE_SHOUT:
        case SPELL_JUDGEMENT:
        case SPELL_WARLOCK_CORRUPTION:
            return spellSpec1 == spellSpec2;
        default:
            return false;
    }
}

bool IsSingleFromSpellSpecificPerTarget(uint32 spellSpec1, uint32 spellSpec2)
{
    switch (spellSpec1)
    {
        case SPELL_TRACKER:
        case SPELL_WARLOCK_ARMOR:
        case SPELL_MAGE_ARMOR:
        case SPELL_ELEMENTAL_SHIELD:
        case SPELL_MAGE_POLYMORPH:
        case SPELL_WELL_FED:
        case SPELL_DRINK:
        case SPELL_FOOD:
        case SPELL_CHARM:
        case SPELL_WARRIOR_ENRAGE:
        case SPELL_ARMOR_REDUCE:
            return spellSpec1 == spellSpec2;
        case SPELL_BATTLE_ELIXIR:
            return spellSpec2 == SPELL_BATTLE_ELIXIR
                || spellSpec2 == SPELL_FLASK_ELIXIR;
        case SPELL_GUARDIAN_ELIXIR:
            return spellSpec2 == SPELL_GUARDIAN_ELIXIR
                || spellSpec2 == SPELL_FLASK_ELIXIR;
        case SPELL_FLASK_ELIXIR:
            return spellSpec2 == SPELL_BATTLE_ELIXIR
                || spellSpec2 == SPELL_GUARDIAN_ELIXIR
                || spellSpec2 == SPELL_FLASK_ELIXIR;
        default:
            return false;
    }
}

bool IsPositiveTarget(uint32 targetA, uint32 targetB)
{
    // non-positive targets
    switch (targetA)
    {
        case TARGET_UNIT_NEARBY_ENEMY:
        case TARGET_UNIT_TARGET_ENEMY:
        case TARGET_UNIT_AREA_ENEMY_SRC:
        case TARGET_UNIT_AREA_ENEMY_DST:
        case TARGET_UNIT_CONE_ENEMY:
        case TARGET_DEST_DYNOBJ_ENEMY:
        case TARGET_DST_TARGET_ENEMY:
        case TARGET_UNIT_CHANNEL:
            return false;
        default:
            break;
    }
    if (targetB)
        return IsPositiveTarget(targetB, 0);
    return true;
}

bool IsPositiveEffect(uint32 spellId, uint32 effIndex)
{
    SpellEntry const *spellproto = sSpellStore.LookupEntry(spellId);
    if (!spellproto)
        return false;

    // talents
    if (IsPassiveSpell(spellId) && GetTalentSpellCost(spellId))
        return true;

    switch (spellId)
    {
        case 23333:                                         // Warsong Flag
        case 23335:                                         // Silverwing Flag
        case 34976:                                         // Netherstorm Flag
        case 23505:                                         // Beserking
        case 23451:                                         // Speed
        case 31579:                                         // Arcane Empowerment Rank1 talent aura with one positive and one negative (check not needed in wotlk)
        case 31582:                                         // Arcane Empowerment Rank2
        case 31583:                                         // Arcane Empowerment Rank3
        case 43945:                                         // You're...
        case 44305:                                         // You're...
        case 30458:                                         // Nigh invulnerability
            return true;
        case  1852:                                         // Silenced (GM)
        case 14621:                                         // Polymorph (Six Demon Bag)
        case 46392:                                         // Focused Assault
        case 46393:                                         // Brutal Assault
        case 43437:                                         // Paralyzed
        case 28441:                                         // not positive dummy spell
        case 37675:                                         // Chaos Blast
        case 41519:                                         // Mark of Stormrage
        case 34877:                                         // Custodian of Time
        case 34700:                                         // Allergic Reaction
        case 31719:                                         // Suspension
        case 43501:                                         // Siphon Soul (Hexlord Spell)
        case 30457:                                         // Complete vulnerability
        case 30500:                                         // Death Coil by Grand Warlock Nethekurse in Shattered Halls
        case 24131:                                         // Wyvern Sting (Rank1)
        case 24134:                                         // Wyvern Sting (Rank2)
        case 24135:                                         // Wyvern Sting (Rank3)
        case 34709:                                         // Shadow Sight
            return false;
    }

    switch (spellproto->Effect[effIndex])
    {
        // always positive effects (check before target checks that provided non-positive result in some case for positive effects)
        case SPELL_EFFECT_HEAL:
        case SPELL_EFFECT_LEARN_SPELL:
        case SPELL_EFFECT_SKILL_STEP:
        case SPELL_EFFECT_HEAL_PCT:
        case SPELL_EFFECT_ENERGIZE_PCT:
            return true;

            // non-positive aura use
        case SPELL_EFFECT_APPLY_AURA:
        case SPELL_EFFECT_APPLY_AREA_AURA_FRIEND:
        {
            switch (spellproto->EffectApplyAuraName[effIndex])
            {
                case SPELL_AURA_DUMMY:
                {
                    // dummy aura can be positive or negative dependent from casted spell
                    switch (spellproto->Id)
                    {
                        case 13139:                         // net-o-matic special effect
                        case 23445:                         // evil twin
                        case 38637:                         // Nether Exhaustion (red)
                        case 38638:                         // Nether Exhaustion (green)
                        case 38639:                         // Nether Exhaustion (blue)
                            return false;
                        default:
                            break;
                    }
                }   break;
                case SPELL_AURA_MOD_DAMAGE_DONE:            // dependent from bas point sign (negative -> negative)
                case SPELL_AURA_MOD_STAT:
                case SPELL_AURA_MOD_SKILL:
                case SPELL_AURA_MOD_HEALING_PCT:
                case SPELL_AURA_MOD_HEALING_DONE:
                {
                    if (spellproto->EffectBasePoints[effIndex]+int32(spellproto->EffectBaseDice[effIndex]) < 0)
                        return false;
                    break;
                }
                case SPELL_AURA_ADD_TARGET_TRIGGER:
                    return true;
                case SPELL_AURA_PERIODIC_TRIGGER_SPELL:
                    if (spellId != spellproto->EffectTriggerSpell[effIndex])
                    {
                        uint32 spellTriggeredId = spellproto->EffectTriggerSpell[effIndex];
                        SpellEntry const *spellTriggeredProto = sSpellStore.LookupEntry(spellTriggeredId);

                        if (spellTriggeredProto)
                        {
                            // non-positive targets of main spell return early
                            for (int i = 0; i < 3; ++i)
                            {
                                // if non-positive trigger cast targeted to positive target this main cast is non-positive
                                // this will place this spell auras as debuffs
                                if (IsPositiveTarget(spellTriggeredProto->EffectImplicitTargetA[effIndex], spellTriggeredProto->EffectImplicitTargetB[effIndex]) && !IsPositiveEffect(spellTriggeredId, i))
                                    return false;
                            }
                        }
                    }
                    break;
                case SPELL_AURA_PROC_TRIGGER_SPELL:
                    // many positive auras have negative triggered spells at damage for example and this not make it negative (it can be canceled for example)
                    break;
                case SPELL_AURA_MOD_STUN:                   //have positive and negative spells, we can't sort its correctly at this moment.
                    if (effIndex == 0 && spellproto->Effect[1] == 0 && spellproto->Effect[2] == 0)
                        return false;                       // but all single stun aura spells is negative

                    // Petrification
                    if (spellproto->Id == 17624)
                        return false;
                    break;
                case SPELL_AURA_MOD_ROOT:
                case SPELL_AURA_MOD_SILENCE:
                case SPELL_AURA_GHOST:
                case SPELL_AURA_PERIODIC_LEECH:
                case SPELL_AURA_MOD_PACIFY_SILENCE:
                case SPELL_AURA_MOD_STALKED:
                case SPELL_AURA_PERIODIC_DAMAGE_PERCENT:
                    return false;
                case SPELL_AURA_PERIODIC_DAMAGE:            // used in positive spells also.
                    // part of negative spell if casted at self (prevent cancel)
                    if (spellproto->EffectImplicitTargetA[effIndex] == TARGET_UNIT_CASTER)
                        return false;
                    break;
                case SPELL_AURA_MOD_DECREASE_SPEED:         // used in positive spells also
                    // part of positive spell if casted at self
                    if (spellproto->EffectImplicitTargetA[effIndex] != TARGET_UNIT_CASTER)
                        return false;
                    // but not this if this first effect (didn't find better check)
                    if (spellproto->Attributes & SPELL_ATTR_NEGATIVE_1 && effIndex == 0)
                        return false;
                    break;
                case SPELL_AURA_TRANSFORM:
                    // some spells negative
                    switch (spellproto->Id)
                    {
                        case 36897:                         // Transporter Malfunction (race mutation to horde)
                        case 36899:                         // Transporter Malfunction (race mutation to alliance)
                            return false;
                    }
                    break;
                case SPELL_AURA_MOD_SCALE:
                    // some spells negative
                    switch (spellproto->Id)
                    {
                        case 36900:                         // Soul Split: Evil!
                        case 36901:                         // Soul Split: Good
                        case 36893:                         // Transporter Malfunction (decrease size case)
                        case 36895:                         // Transporter Malfunction (increase size case)
                            return false;
                    }
                    break;
                case SPELL_AURA_MECHANIC_IMMUNITY:
                {
                    // non-positive immunities
                    switch (spellproto->EffectMiscValue[effIndex])
                    {
                        case MECHANIC_BANDAGE:
                        case MECHANIC_SHIELD:
                        case MECHANIC_MOUNT:
                        case MECHANIC_INVULNERABILITY:
                            return false;
                        default:
                            break;
                    }
                }   break;
                case SPELL_AURA_ADD_FLAT_MODIFIER:          // mods
                case SPELL_AURA_ADD_PCT_MODIFIER:
                {
                    // non-positive mods
                    switch (spellproto->EffectMiscValue[effIndex])
                    {
                        case SPELLMOD_COST:                 // dependent from bas point sign (negative -> positive)
                            if (spellproto->EffectBasePoints[effIndex]+int32(spellproto->EffectBaseDice[effIndex]) > 0)
                            {
                                if (spellproto->Id == 12042)    // Arcane Power is a positive spell
                                    return true;

                                return false;
                            }
                            break;
                        default:
                            break;
                    }
                }   break;
                case SPELL_AURA_FORCE_REACTION:
                    if (spellproto->Id == 42792)               // Recently Dropped Flag (prevent cancel)
                        return false;
                    break;
                default:
                    break;
            }
            break;
        }
        default:
            break;
    }

    // non-positive targets
    if (!IsPositiveTarget(spellproto->EffectImplicitTargetA[effIndex], spellproto->EffectImplicitTargetB[effIndex]))
        return false;

    // AttributesEx check - this is only for sure, everything should work without this
    if (spellproto->AttributesEx & SPELL_ATTR_EX_CANT_BE_REFLECTED) // boss spells should be always negative
        return false;

    // ok, positive
    return true;
}

bool IsPositiveSpell(uint32 spellId)
{
    SpellEntry const *spellproto = sSpellStore.LookupEntry(spellId);
    if (!spellproto) return false;

    // talents
    if (IsPassiveSpell(spellId) && GetTalentSpellCost(spellId))
        return true;

    // spells with at least one negative effect are considered negative
    // some self-applied spells have negative effects but in self casting case negative check ignored.
    for (int i = 0; i < 3; i++)
        if (!IsPositiveEffect(spellId, i))
            return false;
    return true;
}

bool IsSingleTargetSpell(SpellEntry const *spellInfo)
{
    // all other single target spells have if it has AttributesEx5
    if (spellInfo->AttributesEx5 & SPELL_ATTR_EX5_SINGLE_TARGET_SPELL)
        return true;

    // TODO - need found Judgements rule
    switch (GetSpellSpecific(spellInfo->Id))
    {
        case SPELL_JUDGEMENT:
            return true;
    }

    // single target triggered spell.
    // Not real client side single target spell, but it' not triggered until prev. aura expired.
    // This is allow store it in single target spells list for caster for spell proc checking
    if (spellInfo->Id == 38324)                                // Regeneration (triggered by 38299 (HoTs on Heals))
        return true;

    return false;
}

bool IsSingleTargetSpells(SpellEntry const *spellInfo1, SpellEntry const *spellInfo2)
{
    // TODO - need better check
    // Equal icon and spellfamily
    if (spellInfo1->SpellFamilyName == spellInfo2->SpellFamilyName &&
        spellInfo1->SpellIconID == spellInfo2->SpellIconID)
        return true;

    // TODO - need found Judgements rule
    SpellSpecific spec1 = GetSpellSpecific(spellInfo1->Id);
    // spell with single target specific types
    switch (spec1)
    {
        case SPELL_JUDGEMENT:
        case SPELL_MAGE_POLYMORPH:
            if (GetSpellSpecific(spellInfo2->Id) == spec1)
                return true;
            break;
    }

    return false;
}

bool IsAuraAddedBySpell(uint32 auraType, uint32 spellId)
{
    SpellEntry const *spellproto = sSpellStore.LookupEntry(spellId);
    if (!spellproto) return false;

    for (int i = 0; i < 3; i++)
        if (spellproto->EffectApplyAuraName[i] == auraType)
            return true;
    return false;
}

uint8 GetErrorAtShapeshiftedCast (SpellEntry const *spellInfo, uint32 form)
{
    // talents that learn spells can have stance requirements that need ignore
    // (this requirement only for client-side stance show in talent description)
    if (GetTalentSpellCost(spellInfo->Id) > 0 &&
        (spellInfo->Effect[0] == SPELL_EFFECT_LEARN_SPELL || spellInfo->Effect[1] == SPELL_EFFECT_LEARN_SPELL || spellInfo->Effect[2] == SPELL_EFFECT_LEARN_SPELL))
        return 0;

    uint32 stanceMask = (form ? 1 << (form - 1) : 0);

    if (stanceMask & spellInfo->StancesNot)                 // can explicitly not be casted in this stance
        return SPELL_FAILED_NOT_SHAPESHIFT;

    if (stanceMask & spellInfo->Stances)                    // can explicitly be casted in this stance
        return 0;

    bool actAsShifted = false;
    if (form > 0)
    {
        SpellShapeshiftEntry const *shapeInfo = sSpellShapeshiftStore.LookupEntry(form);
        if (!shapeInfo)
        {
            sLog->outError("GetErrorAtShapeshiftedCast: unknown shapeshift %u", form);
            return 0;
        }
        actAsShifted = !(shapeInfo->flags1 & 1);            // shapeshift acts as normal form for spells
    }

    if (actAsShifted)
    {
        if (spellInfo->Attributes & SPELL_ATTR_NOT_SHAPESHIFT) // not while shapeshifted
            return SPELL_FAILED_NOT_SHAPESHIFT;
        else if (spellInfo->Stances != 0)                   // needs other shapeshift
            return SPELL_FAILED_ONLY_SHAPESHIFT;
    }
    else
    {
        // needs shapeshift
        if (!(spellInfo->AttributesEx2 & SPELL_ATTR_EX2_NOT_NEED_SHAPESHIFT) && spellInfo->Stances != 0)
            return SPELL_FAILED_ONLY_SHAPESHIFT;
    }

    return 0;
}

bool IsBinaryResistable(SpellEntry const* spellInfo)
{
    if (spellInfo->SchoolMask & SPELL_SCHOOL_MASK_HOLY)                  // can't resist holy spells
        return false;

    if (spellInfo->SpellFamilyName)         // only player's spells, bosses don't follow that simple rule
    {
        for(int eff = 0; eff < 3; eff++)
        {
            if (!spellInfo->Effect[eff])
                continue;

            if (IsPositiveEffect(spellInfo->Id, eff))
                continue;

            switch(spellInfo->Effect[eff])
            {
                case SPELL_EFFECT_SCHOOL_DAMAGE:
                    break;
                case SPELL_EFFECT_APPLY_AURA:
                case SPELL_EFFECT_APPLY_AREA_AURA_ENEMY:
                    if (spellInfo->EffectApplyAuraName[eff] != SPELL_AURA_PERIODIC_DAMAGE)       // spells that apply aura other then DOT are binary resistable
                        return true;
                    break;
                default:
                    return true;                                                                // spells that have other effects then damage or apply aura are binary resistable
            }
        }
    }

    switch (spellInfo->Id)
    {
        case 31306:     // Anetheron - Carrion Swarm
        case 31344:     // Howl of Azgalor
        case 31447:     // Mark of Kaz'Rogal
        case 34190:     // Void - Arcane Orb
        case 37730:     // Morogrim - Tidal Wave
        case 38441:     // Fathom - Cataclysm bolt
        case 38509:     // Vashj - Shock Blast
        case 37675:     // Leotheras - Chaos Blast
            return true;
    }
    return false;
}

bool IsSpellIgnoringLOS(SpellEntry const* spellInfo)
{
    if (spellInfo->AttributesEx2 & SPELL_ATTR_EX2_IGNORE_LOS)
        return true;

    if (!VMAP::VMapFactory::checkSpellForLoS(spellInfo->Id))
        return true;

    // Spells with area destinations also belong here
    if (spellInfo->Targets & (TARGET_FLAG_SOURCE_LOCATION | TARGET_FLAG_DEST_LOCATION))
        return true;

    return false;
}

void SpellMgr::LoadSpellTargetPositions()
{
    mSpellTargetPositions.clear();                                // need for reload case

    uint32 count = 0;

    //                                                       0   1           2                  3                  4                  5
    QueryResult_AutoPtr result = WorldDatabase.Query("SELECT id, target_map, target_position_x, target_position_y, target_position_z, target_orientation FROM spell_target_position");
    if (!result)
    {
        sLog->outString();
        sLog->outString(">> Loaded %u spell target coordinates", count);
        return;
    }

    do
    {
        Field *fields = result->Fetch();

        uint32 Spell_ID = fields[0].GetUInt32();

        SpellTargetPosition st;

        st.target_mapId       = fields[1].GetUInt32();
        st.target_X           = fields[2].GetFloat();
        st.target_Y           = fields[3].GetFloat();
        st.target_Z           = fields[4].GetFloat();
        st.target_Orientation = fields[5].GetFloat();

        MapEntry const* mapEntry = sMapStore.LookupEntry(st.target_mapId);
        if (!mapEntry)
        {
            sLog->outErrorDb("Spell (ID:%u) target map (ID: %u) does not exist in Map.dbc.", Spell_ID, st.target_mapId);
            continue;
        }

        if (st.target_X == 0 && st.target_Y == 0 && st.target_Z == 0)
        {
            sLog->outErrorDb("Spell (ID:%u) target coordinates not provided.", Spell_ID);
            continue;
        }

        SpellEntry const* spellInfo = sSpellStore.LookupEntry(Spell_ID);
        if (!spellInfo)
        {
            sLog->outErrorDb("Spell (ID:%u) listed in spell_target_position does not exist.", Spell_ID);
            continue;
        }

        bool found = false;
        for (int i = 0; i < 3; ++i)
        {
            if (spellInfo->EffectImplicitTargetA[i] == TARGET_DST_DB || spellInfo->EffectImplicitTargetB[i] == TARGET_DST_DB)
            {
                found = true;
                break;
            }
        }
        if (!found)
        {
            sLog->outErrorDb("Spell (Id: %u) listed in spell_target_position does not have target TARGET_DST_DB (17).", Spell_ID);
            continue;
        }

        mSpellTargetPositions[Spell_ID] = st;
        ++count;
    } while (result->NextRow());

    sLog->outString();
    sLog->outString(">> Loaded %u spell teleport coordinates", count);
}

void SpellMgr::LoadSpellAffects()
{
    mSpellAffectMap.clear();                                // need for reload case

    uint32 count = 0;

    //                                                       0      1         2
    QueryResult_AutoPtr result = WorldDatabase.Query("SELECT entry, effectId, SpellFamilyMask FROM spell_affect");
    if (!result)
    {
        sLog->outString();
        sLog->outString(">> Loaded %u spell affect definitions", count);
        return;
    }

    do
    {
        Field *fields = result->Fetch();

        uint16 entry = fields[0].GetUInt16();
        uint8 effectId = fields[1].GetUInt8();

        SpellEntry const* spellInfo = sSpellStore.LookupEntry(entry);

        if (!spellInfo)
        {
            sLog->outErrorDb("Spell %u listed in spell_affect does not exist", entry);
            continue;
        }

        if (effectId >= 3)
        {
            sLog->outErrorDb("Spell %u listed in spell_affect has invalid effect index (%u)", entry, effectId);
            continue;
        }

        if (spellInfo->Effect[effectId] != SPELL_EFFECT_APPLY_AURA ||
            spellInfo->EffectApplyAuraName[effectId] != SPELL_AURA_ADD_FLAT_MODIFIER &&
            spellInfo->EffectApplyAuraName[effectId] != SPELL_AURA_ADD_PCT_MODIFIER  &&
            spellInfo->EffectApplyAuraName[effectId] != SPELL_AURA_ADD_TARGET_TRIGGER)
        {
            sLog->outErrorDb("Spell %u listed in spell_affect does not have SPELL_AURA_ADD_FLAT_MODIFIER (%u) or SPELL_AURA_ADD_PCT_MODIFIER (%u) or SPELL_AURA_ADD_TARGET_TRIGGER (%u) for effect index (%u)", entry, SPELL_AURA_ADD_FLAT_MODIFIER, SPELL_AURA_ADD_PCT_MODIFIER, SPELL_AURA_ADD_TARGET_TRIGGER, effectId);
            continue;
        }

        uint64 spellAffectMask = fields[2].GetUInt64();

        // Spell.dbc have own data for low part of SpellFamilyMask
        if (spellInfo->EffectItemType[effectId])
        {
            if (spellInfo->EffectItemType[effectId] == spellAffectMask)
            {
                sLog->outErrorDb("Spell %u listed in spell_affect has redundant (same with EffectItemType%d) data for effect index (%u), skipped.", entry, effectId+1, effectId);
                continue;
            }

            // 24429 have wrong data in EffectItemType and overwrites by DB, possible bug in client
            if (spellInfo->Id != 24429 && spellInfo->EffectItemType[effectId] != spellAffectMask)
            {
                sLog->outErrorDb("Spell %u listed in spell_affect has different low part from EffectItemType%d for effect index (%u), skipped.", entry, effectId+1, effectId);
                continue;
            }
        }

        mSpellAffectMap.insert(SpellAffectMap::value_type((entry<<8) + effectId, spellAffectMask));

        ++count;
    } while (result->NextRow());

    sLog->outString();
    sLog->outString(">> Loaded %u spell affect definitions", count);

    for (uint32 id = 0; id < sSpellStore.GetNumRows(); ++id)
    {
        SpellEntry const* spellInfo = sSpellStore.LookupEntry(id);
        if (!spellInfo)
            continue;

        for (int effectId = 0; effectId < 3; ++effectId)
        {
            if (spellInfo->Effect[effectId] != SPELL_EFFECT_APPLY_AURA ||
                (spellInfo->EffectApplyAuraName[effectId] != SPELL_AURA_ADD_FLAT_MODIFIER &&
                spellInfo->EffectApplyAuraName[effectId] != SPELL_AURA_ADD_PCT_MODIFIER  &&
                spellInfo->EffectApplyAuraName[effectId] != SPELL_AURA_ADD_TARGET_TRIGGER))
                continue;

            if (spellInfo->EffectItemType[effectId] != 0)
                continue;

            if (mSpellAffectMap.find((id<<8) + effectId) != mSpellAffectMap.end())
                continue;

            sLog->outErrorDb("Spell %u (%s) misses spell_affect for effect %u", id, spellInfo->SpellName[sWorld->GetDefaultDbcLocale()], effectId);
        }
    }
}

bool SpellMgr::IsAffectedBySpell(SpellEntry const *spellInfo, uint32 spellId, uint8 effectId, uint64 familyFlags) const
{
    // false for spellInfo == NULL
    if (!spellInfo)
        return false;

    SpellEntry const *affect_spell = sSpellStore.LookupEntry(spellId);
    // false for affect_spell == NULL
    if (!affect_spell)
        return false;

    // False if spellFamily not equal
    if (affect_spell->SpellFamilyName != spellInfo->SpellFamilyName)
        return false;

    // If familyFlags == 0
    if (!familyFlags)
    {
        // Get it from spellAffect table
        familyFlags = GetSpellAffectMask(spellId, effectId);
        // false if familyFlags == 0
        if (!familyFlags)
            return false;
    }

    // true
    if (familyFlags & spellInfo->SpellFamilyFlags)
        return true;

    return false;
}

void SpellMgr::LoadSpellProcEvents()
{
    mSpellProcEventMap.clear();                             // need for reload case

    uint32 count = 0;

    //                                                       0      1           2                3                4          5       6        7             8
    QueryResult_AutoPtr result = WorldDatabase.Query("SELECT entry, SchoolMask, SpellFamilyName, SpellFamilyMask, procFlags, procEx, ppmRate, CustomChance, Cooldown FROM spell_proc_event");
    if (!result)
    {
        sLog->outString();
        sLog->outString(">> Loaded %u spell proc event conditions", count);
        return;
    }

    uint32 customProc = 0;
    do
    {
        Field *fields = result->Fetch();

        uint16 entry = fields[0].GetUInt16();

        const SpellEntry *spell = sSpellStore.LookupEntry(entry);
        if (!spell)
        {
            sLog->outErrorDb("Spell %u listed in spell_proc_event does not exist", entry);
            continue;
        }

        SpellProcEventEntry spe;

        spe.schoolMask      = fields[1].GetUInt32();
        spe.spellFamilyName = fields[2].GetUInt32();
        spe.spellFamilyMask = fields[3].GetUInt64();
        spe.procFlags       = fields[4].GetUInt32();
        spe.procEx          = fields[5].GetUInt32();
        spe.ppmRate         = fields[6].GetFloat();
        spe.customChance    = fields[7].GetFloat();
        spe.cooldown        = fields[8].GetUInt32();

        mSpellProcEventMap[entry] = spe;

        if (spell->procFlags == 0)
        {
            if (spe.procFlags == 0)
            {
                sLog->outErrorDb("Spell %u listed in spell_proc_event probally not triggered spell", entry);
                continue;
            }
            customProc++;
        }
        ++count;
    } while (result->NextRow());

    sLog->outString();
    if (customProc)
        sLog->outString(">> Loaded %u custom spell proc event conditions +%u custom", count, customProc);
    else
        sLog->outString(">> Loaded %u spell proc event conditions", count);

    /*
    // Commented for now, as it still produces many errors (still quite many spells miss spell_proc_event)
    for (uint32 id = 0; id < sSpellStore.GetNumRows(); ++id)
    {
        SpellEntry const* spellInfo = sSpellStore.LookupEntry(id);
        if (!spellInfo)
            continue;

        bool found = false;
        for (int effectId = 0; effectId < 3; ++effectId)
        {
            // at this moment check only SPELL_AURA_PROC_TRIGGER_SPELL
            if (spellInfo->EffectApplyAuraName[effectId] == SPELL_AURA_PROC_TRIGGER_SPELL)
            {
                found = true;
                break;
            }
        }

        if (!found)
            continue;

        if (GetSpellProcEvent(id))
            continue;

        sLog->outErrorDb("Spell %u (%s) misses spell_proc_event", id, spellInfo->SpellName[sWorld->GetDBClang()]);
    }
    */
}

/*
bool SpellMgr::IsSpellProcEventCanTriggeredBy(SpellProcEventEntry const * spellProcEvent, SpellEntry const * procSpell, uint32 procFlags)
{
    if ((procFlags & spellProcEvent->procFlags) == 0)
        return false;

    // Additional checks in case spell cast/hit/crit is the event
    // Check (if set) school, category, skill line, spell talent mask
    if (spellProcEvent->schoolMask && (!procSpell || (GetSpellSchoolMask(procSpell) & spellProcEvent->schoolMask) == 0))
        return false;
    if (spellProcEvent->category && (!procSpell || procSpell->Category != spellProcEvent->category))
        return false;
    if (spellProcEvent->skillId)
    {
        if (!procSpell)
            return false;

        SkillLineAbilityMap::const_iterator lower = sSpellMgr->GetBeginSkillLineAbilityMap(procSpell->Id);
        SkillLineAbilityMap::const_iterator upper = sSpellMgr->GetEndSkillLineAbilityMap(procSpell->Id);

        bool found = false;
        for (SkillLineAbilityMap::const_iterator _spell_idx = lower; _spell_idx != upper; ++_spell_idx)
        {
            if (_spell_idx->second->skillId == spellProcEvent->skillId)
            {
                found = true;
                break;
            }
        }
        if (!found)
            return false;
    }
    if (spellProcEvent->spellFamilyName && (!procSpell || spellProcEvent->spellFamilyName != procSpell->SpellFamilyName))
        return false;
    if (spellProcEvent->spellFamilyMask && (!procSpell || (spellProcEvent->spellFamilyMask & procSpell->SpellFamilyFlags) == 0))
        return false;

    return true;
}
*/

bool SpellMgr::IsSpellProcEventCanTriggeredBy(SpellProcEventEntry const * spellProcEvent, uint32 EventProcFlag, SpellEntry const * procSpell, uint32 procFlags, uint32 procExtra, bool active)
{
    // No extra req need
    uint32 procEvent_procEx = PROC_EX_NONE;

    // check prockFlags for condition
    if ((procFlags & EventProcFlag) == 0)
        return false;

    /* Check Periodic Auras

    * Both hots and dots can trigger if spell has no PROC_FLAG_SUCCESSFUL_POSITIVE_SPELL
        nor PROC_FLAG_SUCCESSFUL_NEGATIVE_SPELL_HIT

    *Only Hots can trigger if spell has PROC_FLAG_SUCCESSFUL_POSITIVE_SPELL

    *Only dots can trigger if spell has both positivity flags or PROC_FLAG_SUCCESSFUL_NEGATIVE_SPELL_HIT

    */

    if (procFlags & PROC_FLAG_ON_DO_PERIODIC)
    {
        if (EventProcFlag & PROC_FLAG_SUCCESSFUL_NEGATIVE_SPELL_HIT)
        {
            if (!(procExtra & PROC_EX_INTERNAL_DOT))
                return false;
        }
        else if (EventProcFlag & PROC_FLAG_SUCCESSFUL_POSITIVE_SPELL
            && !(procExtra & PROC_EX_INTERNAL_HOT))
            return false;
    }

    if (procFlags & PROC_FLAG_ON_TAKE_PERIODIC)
    {
        if (EventProcFlag & PROC_FLAG_TAKEN_NEGATIVE_SPELL_HIT)
        {
            if (!(procExtra & PROC_EX_INTERNAL_DOT))
                return false;
        }
        else if (EventProcFlag & PROC_FLAG_TAKEN_POSITIVE_SPELL
            && !(procExtra & PROC_EX_INTERNAL_HOT))
            return false;
    }

    // Always trigger for this
    if (EventProcFlag & (PROC_FLAG_KILLED | PROC_FLAG_KILL | PROC_FLAG_ON_TRAP_ACTIVATION))
        return true;

    if (spellProcEvent)     // Exist event data
    {
        // Store extra req
        procEvent_procEx = spellProcEvent->procEx;

        // For melee triggers
        if (procSpell == NULL)
        {
            // Check (if set) for school (melee attack have Normal school)
            if (spellProcEvent->schoolMask && (spellProcEvent->schoolMask & SPELL_SCHOOL_MASK_NORMAL) == 0)
                return false;
            active = true; // Spell added manualy -> so its active spell    
        }
        else // For spells need check school/spell family/family mask
        {
            // Potions can trigger only if spellfamily given
            if (procSpell->SpellFamilyName == SPELLFAMILY_POTION)
            {
                if (procSpell->SpellFamilyName == spellProcEvent->spellFamilyName)
                    return true;
                return false;
            }

            // Check (if set) for school
            if (spellProcEvent->schoolMask && (spellProcEvent->schoolMask & procSpell->SchoolMask) == 0)
                return false;

            // Check (if set) for spellFamilyName
            if (spellProcEvent->spellFamilyName && (spellProcEvent->spellFamilyName != procSpell->SpellFamilyName))
                return false;

            // spellFamilyName is Ok need check for spellFamilyMask if present
            if (spellProcEvent->spellFamilyMask)
            {
                if ((spellProcEvent->spellFamilyMask & procSpell->SpellFamilyFlags) == 0)
                    return false;
                active = true; // Spell added manualy -> so its active spell
            }
        }
    }
    // potions can trigger only if have spell_proc entry
    else if (procSpell && procSpell->SpellFamilyName==SPELLFAMILY_POTION)
        return false;

    // Check for extra req (if none) and hit/crit
    if (procEvent_procEx == PROC_EX_NONE)
    {
        // No extra req, so can trigger only for active (damage/healing present) and hit/crit
        if ((procExtra & (PROC_EX_NORMAL_HIT|PROC_EX_CRITICAL_HIT)) && active)
            return true;
    }
    else // Passive spells hits here only if resist/reflect/immune/evade
    {
        // Exist req for PROC_EX_EX_TRIGGER_ALWAYS
        if (procEvent_procEx & PROC_EX_EX_TRIGGER_ALWAYS)
            return true;
        // Passive spells cant trigger if need hit
        if ((procEvent_procEx & PROC_EX_NORMAL_HIT) && !active)
            return false;
        // Check Extra Requirement like (hit/crit/miss/resist/parry/dodge/block/immune/reflect/absorb and other)
        if (procEvent_procEx & procExtra)
            return true;
    }
    return false;
}

void SpellMgr::LoadSpellElixirs()
{
    mSpellElixirs.clear();                                  // need for reload case

    uint32 count = 0;

    //                                                       0      1
    QueryResult_AutoPtr result = WorldDatabase.Query("SELECT entry, mask FROM spell_elixir");
    if (!result)
    {
        sLog->outString();
        sLog->outString(">> Loaded %u spell elixir definitions", count);
        return;
    }

    do
    {
        Field *fields = result->Fetch();

        uint16 entry = fields[0].GetUInt16();
        uint8 mask = fields[1].GetUInt8();

        SpellEntry const* spellInfo = sSpellStore.LookupEntry(entry);

        if (!spellInfo)
        {
            sLog->outErrorDb("Spell %u listed in spell_elixir does not exist", entry);
            continue;
        }

        mSpellElixirs[entry] = mask;

        ++count;
    } while (result->NextRow());

    sLog->outString();
    sLog->outString(">> Loaded %u spell elixir definitions", count);
}

void SpellMgr::LoadSpellThreats()
{
    sSpellThreatStore.Free();                               // for reload

    sSpellThreatStore.Load();

    sLog->outString(">> Loaded %u aggro generating spells", sSpellThreatStore.RecordCount);
    sLog->outString();
}

void SpellMgr::LoadSpellEnchantProcData()
{
    mSpellEnchantProcEventMap.clear();                             // need for reload case

    uint32 count = 0;

    //                                                       0      1             2          3
    QueryResult_AutoPtr result = WorldDatabase.Query("SELECT entry, customChance, PPMChance, procEx FROM spell_enchant_proc_data");
    if (!result)
    {
        sLog->outString();
        sLog->outString(">> Loaded %u spell enchant proc event conditions", count);
        return;
    }

    do
    {
        Field *fields = result->Fetch();

        uint32 enchantId = fields[0].GetUInt32();

        SpellItemEnchantmentEntry const *ench = sSpellItemEnchantmentStore.LookupEntry(enchantId);
        if (!ench)
        {
            sLog->outErrorDb("Enchancment %u listed in spell_enchant_proc_data does not exist", enchantId);
            continue;
        }

        SpellEnchantProcEntry spe;

        spe.customChance = fields[1].GetUInt32();
        spe.PPMChance = fields[2].GetFloat();
        spe.procEx = fields[3].GetUInt32();

        mSpellEnchantProcEventMap[enchantId] = spe;

        ++count;
    } while (result->NextRow());

    sLog->outString(">> Loaded %u enchant proc data definitions", count);
}

bool SpellMgr::IsRankSpellDueToSpell(SpellEntry const *spellInfo_1, uint32 spellId_2) const
{
    SpellEntry const *spellInfo_2 = sSpellStore.LookupEntry(spellId_2);
    if (!spellInfo_1 || !spellInfo_2) return false;
    if (spellInfo_1->Id == spellId_2) return false;

    return GetFirstSpellInChain(spellInfo_1->Id) == GetFirstSpellInChain(spellId_2);
}

bool SpellMgr::canStackSpellRanks(SpellEntry const *spellInfo)
{
    if (spellInfo->powerType != POWER_MANA && spellInfo->powerType != POWER_HEALTH)
        return false;
    if (IsProfessionSpell(spellInfo->Id))
        return false;

    // All stance spells. if any better way, change it.
    for (int i = 0; i < 3; i++)
    {
        // Paladin aura Spell
        if (spellInfo->SpellFamilyName == SPELLFAMILY_PALADIN
            && spellInfo->Effect[i] == SPELL_EFFECT_APPLY_AREA_AURA_PARTY)
            return false;
        // Druid form Spell
        if (spellInfo->SpellFamilyName == SPELLFAMILY_DRUID
            && spellInfo->Effect[i] == SPELL_EFFECT_APPLY_AURA
            && spellInfo->EffectApplyAuraName[i] == SPELL_AURA_MOD_SHAPESHIFT)
            return false;
        // Rogue Stealth
        if (spellInfo->SpellFamilyName == SPELLFAMILY_ROGUE
            && spellInfo->Effect[i] == SPELL_EFFECT_APPLY_AURA
            && spellInfo->EffectApplyAuraName[i] == SPELL_AURA_MOD_SHAPESHIFT)
            return false;
    }
    return true;
}

bool SpellMgr::IsNoStackSpellDueToSpell(uint32 spellId_1, uint32 spellId_2, bool sameCaster) const
{
    SpellEntry const *spellInfo_1 = sSpellStore.LookupEntry(spellId_1);
    SpellEntry const *spellInfo_2 = sSpellStore.LookupEntry(spellId_2);

    if (!spellInfo_1 || !spellInfo_2)
        return false;

    SpellSpecific spellId_spec_1 = GetSpellSpecific(spellId_1);
    SpellSpecific spellId_spec_2 = GetSpellSpecific(spellId_2);
    if (spellId_spec_1 && spellId_spec_2)
        if (IsSingleFromSpellSpecificPerTarget(spellId_spec_1, spellId_spec_2)
            ||(IsSingleFromSpellSpecificPerCaster(spellId_spec_1, spellId_spec_2) && sameCaster))
            return true;

    // spells with different specific always stack
    if (spellId_spec_1 != spellId_spec_2)
        return false;

    if (spellInfo_1->SpellFamilyName != spellInfo_2->SpellFamilyName)
        return false;

    if (!sameCaster)
    {
        for (uint32 i = 0; i < 3; ++i)
            if (spellInfo_1->Effect[i] == SPELL_EFFECT_APPLY_AURA
                || spellInfo_1->Effect[i] == SPELL_EFFECT_PERSISTENT_AREA_AURA)
                // not area auras (shaman totem)
                switch (spellInfo_1->EffectApplyAuraName[i])
                {
                    // DOT or HOT from different casters will stack
                    case SPELL_AURA_PERIODIC_DAMAGE:
                    case SPELL_AURA_PERIODIC_HEAL:
                    case SPELL_AURA_PERIODIC_TRIGGER_SPELL:
                    case SPELL_AURA_PERIODIC_ENERGIZE:
                    case SPELL_AURA_PERIODIC_MANA_LEECH:
                    case SPELL_AURA_PERIODIC_LEECH:
                    case SPELL_AURA_POWER_BURN_MANA:
                    case SPELL_AURA_OBS_MOD_MANA:
                    case SPELL_AURA_OBS_MOD_HEALTH:
                    case SPELL_AURA_PERIODIC_TRIGGER_SPELL_WITH_VALUE:
                        return false;
                    default:
                        break;
                }
    }

//    not needed now because we compare effects last rank of spells
//    if (spellInfo_1->SpellFamilyName && IsRankSpellDueToSpell(spellInfo_1, spellId_2))
//        return true;

    // generic spells
    if (!spellInfo_1->SpellFamilyName)
    {
        if (!spellInfo_1->SpellIconID
            || spellInfo_1->SpellIconID == 1
            || spellInfo_1->SpellIconID != spellInfo_2->SpellIconID)
            return false;
    }

    // check for class spells
    else
    {
        if (spellInfo_1->SpellFamilyFlags != spellInfo_2->SpellFamilyFlags)
            return false;
    }

    //use data of highest rank spell(needed for spells which ranks have different effects)
    spellInfo_1 = sSpellStore.LookupEntry(GetLastSpellInChain(spellId_1));
    spellInfo_2 = sSpellStore.LookupEntry(GetLastSpellInChain(spellId_2));

    //if spells have exactly the same effect they cannot stack
    for (uint32 i = 0; i < 3; ++i)
        if (spellInfo_1->Effect[i] != spellInfo_2->Effect[i]
            || spellInfo_1->EffectApplyAuraName[i] != spellInfo_2->EffectApplyAuraName[i]
            || spellInfo_1->EffectMiscValue[i] != spellInfo_2->EffectMiscValue[i]) // paladin resist aura
            return false; // need itemtype check? need an example to add that check

    return true;
}

bool SpellMgr::IsProfessionSpell(uint32 spellId)
{
    SpellEntry const *spellInfo = sSpellStore.LookupEntry(spellId);
    if (!spellInfo)
        return false;

    if (spellInfo->Effect[1] != SPELL_EFFECT_SKILL)
        return false;

    uint32 skill = spellInfo->EffectMiscValue[1];

    return IsProfessionSkill(skill);
}

bool SpellMgr::IsPrimaryProfessionSpell(uint32 spellId)
{
    SpellEntry const *spellInfo = sSpellStore.LookupEntry(spellId);
    if (!spellInfo)
        return false;

    if (spellInfo->Effect[1] != SPELL_EFFECT_SKILL)
        return false;

    uint32 skill = spellInfo->EffectMiscValue[1];

    return IsPrimaryProfessionSkill(skill);
}

bool SpellMgr::IsProfessionSpecializationSpell(uint32 spellId)
{
    switch (spellId)
    {
        case 9787: // Weapon
        case 9788: // Armor
        case 17040: // Hammer
        case 17041: // Axe
        case 17039: // Sword
        case 17451: // Rep Armor ?
        case 17452: // Rep Weapon ?
        case 10656: // Dragon
        case 10658: // Elemental
        case 10660: // Tribal
        case 20222: // Goblin
        case 20219: // Gnomish
        case 26797: // SpellFire
        case 26798: // MoonCloth
        case 26801: // ShadowWeave
        case 28672: // Transmute
        case 28677: // Elixir
        case 28675: // Potion
            return true;
    }

    return false;
}

bool SpellMgr::IsPrimaryProfessionFirstRankSpell(uint32 spellId) const
{
    return IsPrimaryProfessionSpell(spellId) && GetSpellRank(spellId) == 1;
}

SpellEntry const* SpellMgr::SelectAuraRankForPlayerLevel(SpellEntry const* spellInfo, uint32 playerLevel) const
{
    // ignore passive spells
    if (IsPassiveSpell(spellInfo->Id))
        return spellInfo;

    bool needRankSelection = false;
    for (int i = 0;i < 3;i++)
    {
        if (IsPositiveEffect(spellInfo->Id, i) && (
            spellInfo->Effect[i] == SPELL_EFFECT_APPLY_AURA ||
            spellInfo->Effect[i] == SPELL_EFFECT_APPLY_AREA_AURA_PARTY
))
        {
            needRankSelection = true;
            break;
        }
    }

    // not required
    if (!needRankSelection)
        return spellInfo;

    for (uint32 nextSpellId = spellInfo->Id; nextSpellId != 0; nextSpellId = GetPrevSpellInChain(nextSpellId))
    {
        SpellEntry const *nextSpellInfo = sSpellStore.LookupEntry(nextSpellId);
        if (!nextSpellInfo)
            break;

        // if found appropriate level
        if (playerLevel + 10 >= nextSpellInfo->spellLevel)
            return nextSpellInfo;

        // one rank less then
    }

    // not found
    return NULL;
}

void SpellMgr::LoadSpellRequired()
{
    mSpellsReqSpell.clear();                                   // need for reload case
    mSpellReq.clear();                                         // need for reload case

    QueryResult_AutoPtr result = WorldDatabase.Query("SELECT spell_id, req_spell from spell_required");

    if (result == NULL)
    {
        sLog->outString();
        sLog->outString(">> Loaded 0 spell required records");
        sLog->outErrorDb("spell_required table is empty!");
        return;
    }
    uint32 rows = 0;

    do
    {
        Field *fields = result->Fetch();

        uint32 spell_id = fields[0].GetUInt32();
        uint32 spell_req = fields[1].GetUInt32();

        mSpellsReqSpell.insert (std::pair<uint32, uint32>(spell_req, spell_id));
        mSpellReq[spell_id] = spell_req;
        ++rows;
    } while (result->NextRow());

    sLog->outString();
    sLog->outString(">> Loaded %u spell required records", rows);
}

void SpellMgr::LoadSpellRanks()
{
    mSpellChains.clear();                                   // need for reload case

    QueryResult_AutoPtr result = WorldDatabase.Query("SELECT first_spell_id, spell_id, rank FROM spell_ranks ORDER BY first_spell_id, rank");

    if (!result)
    {
        sLog->outString();
        sLog->outString(">> Loaded 0 spell rank records");
        sLog->outErrorDb("`spell_ranks` table is empty!");
        return;
    }

    uint32 rows = 0;
    bool finished = false;
    int32 countDeletedSpells = 0;

    do
    {
        // spellid, rank
        std::list < std::pair < int32, int32 > > rankChain;
        int32 currentSpell = -1;
        int32 lastSpell = -1;

        // fill one chain
        while (currentSpell == lastSpell && !finished)
        {
            Field *fields = result->Fetch();

            currentSpell = fields[0].GetUInt32();
            if (lastSpell == -1)
                lastSpell = currentSpell;
            uint32 spell_id = fields[1].GetUInt32();
            uint32 rank = fields[2].GetUInt32();

            // don't drop the row if we're moving to the next rank
            if (currentSpell == lastSpell)
            {
                rankChain.push_back(std::make_pair(spell_id, rank));
                if (!result->NextRow())
                    finished = true;
            }
            else
                break;
        }
        // check if chain is made with valid first spell
        SpellEntry const * first = sSpellStore.LookupEntry(lastSpell);
        if (!first)
        {
            sLog->outErrorDb("Spell rank identifier(first_spell_id) %u listed in `spell_ranks` does not exist!", lastSpell);
            continue;
        }

        // check if chain is long enough
        if (rankChain.size() < 2)
        {
            sLog->outErrorDb("There is only 1 spell rank for identifier(first_spell_id) %u in `spell_ranks`, entry is not needed!", lastSpell);
            continue;
        }

        int32 curRank = 0;
        bool valid = true;

        // check spells in chain
        for (std::list<std::pair<int32, int32> >::iterator itr = rankChain.begin() ; itr!= rankChain.end(); ++itr)
        {
            SpellEntry const * spell = sSpellStore.LookupEntry(itr->first);
            if (!spell)
            {
                sLog->outErrorDb("Spell %u (rank %u) listed in `spell_ranks` for chain %u does not exist!", itr->first, itr->second, lastSpell);
                valid = false;
                break;
            }
            ++curRank;
            if (itr->second != curRank)
            {
                sLog->outErrorDb("Spell %u (rank %u) listed in `spell_ranks` for chain %u does not have proper rank value(should be %u)!", itr->first, itr->second, lastSpell, curRank);
                valid = false;
                break;
            }
        }

        if (!valid)
            continue;

        int32 prevRank = 0;

        // insert the chain
        std::list<std::pair<int32, int32> >::iterator itr = rankChain.begin();
        do
        {
            int32 addedSpell = itr->first;
            mSpellChains[addedSpell].first = lastSpell;
            mSpellChains[addedSpell].last = rankChain.back().first;
            mSpellChains[addedSpell].rank = itr->second;
            mSpellChains[addedSpell].prev = prevRank;
            prevRank = addedSpell;
            ++itr;
            if (itr == rankChain.end())
            {
                mSpellChains[addedSpell].next = 0;
                break;
            }
            else
                mSpellChains[addedSpell].next = itr->first;
        }
        while (true);

        ++rows;
    } while (!finished);

    sLog->outString();
    sLog->outString(">> Loaded %u spell rank records", rows);
}

void SpellMgr::LoadSpellLearnSkills()
{
    mSpellLearnSkills.clear();                              // need for reload case

    // search auto-learned skills and add its to map also for use in unlearn spells/talents
    uint32 dbc_count = 0;
    for (uint32 spell = 0; spell < sSpellStore.GetNumRows(); ++spell)
    {
        SpellEntry const* entry = sSpellStore.LookupEntry(spell);

        if (!entry)
            continue;

        for (int i = 0; i < 3; ++i)
        {
            if (entry->Effect[i] == SPELL_EFFECT_SKILL)
            {
                SpellLearnSkillNode dbc_node;
                dbc_node.skill    = entry->EffectMiscValue[i];
                if (dbc_node.skill != SKILL_RIDING)
                    dbc_node.value    = 1;
                else
                    dbc_node.value    = (entry->EffectBasePoints[i] + 1) * 75;
                dbc_node.maxvalue = (entry->EffectBasePoints[i] + 1) * 75;

                SpellLearnSkillNode const* db_node = GetSpellLearnSkill(spell);

                mSpellLearnSkills[spell] = dbc_node;
                ++dbc_count;
                break;
            }
        }
    }

    sLog->outString();
    sLog->outString(">> Loaded %u Spell Learn Skills from DBC", dbc_count);
}

void SpellMgr::LoadSpellLearnSpells()
{
    mSpellLearnSpells.clear();                              // need for reload case

    QueryResult_AutoPtr result = WorldDatabase.Query("SELECT entry, SpellID FROM spell_learn_spell");
    if (!result)
    {
        sLog->outString();
        sLog->outString(">> Loaded 0 spell learn spells");
        sLog->outErrorDb("spell_learn_spell table is empty!");
        return;
    }

    uint32 count = 0;

    do
    {
        Field *fields = result->Fetch();

        uint32 spell_id    = fields[0].GetUInt32();

        SpellLearnSpellNode node;
        node.spell      = fields[1].GetUInt32();
        node.autoLearned= false;

        if (!sSpellStore.LookupEntry(spell_id))
        {
            sLog->outErrorDb("Spell %u listed in spell_learn_spell does not exist", spell_id);
            continue;
        }

        if (!sSpellStore.LookupEntry(node.spell))
        {
            sLog->outErrorDb("Spell %u listed in spell_learn_spell does not exist", node.spell);
            continue;
        }

        mSpellLearnSpells.insert(SpellLearnSpellMap::value_type(spell_id, node));

        ++count;
    } while (result->NextRow());

    // search auto-learned spells and add its to map also for use in unlearn spells/talents
    uint32 dbc_count = 0;
    for (uint32 spell = 0; spell < sSpellStore.GetNumRows(); ++spell)
    {
        SpellEntry const* entry = sSpellStore.LookupEntry(spell);

        if (!entry)
            continue;

        for (int i = 0; i < 3; ++i)
        {
            if (entry->Effect[i] == SPELL_EFFECT_LEARN_SPELL)
            {
                SpellLearnSpellNode dbc_node;
                dbc_node.spell       = entry->EffectTriggerSpell[i];
                dbc_node.autoLearned = true;

                SpellLearnSpellMap::const_iterator db_node_begin = GetBeginSpellLearnSpell(spell);
                SpellLearnSpellMap::const_iterator db_node_end   = GetEndSpellLearnSpell(spell);

                bool found = false;
                for (SpellLearnSpellMap::const_iterator itr = db_node_begin; itr != db_node_end; ++itr)
                {
                    if (itr->second.spell == dbc_node.spell)
                    {
                        sLog->outErrorDb("Spell %u auto-learn spell %u in spell.dbc then the record in spell_learn_spell is redundant, please fix DB.",
                            spell, dbc_node.spell);
                        found = true;
                        break;
                    }
                }

                if (!found)                                  // add new spell-spell pair if not found
                {
                    mSpellLearnSpells.insert(SpellLearnSpellMap::value_type(spell, dbc_node));
                    ++dbc_count;
                }
            }
        }
    }

    sLog->outString();
    sLog->outString(">> Loaded %u spell learn spells + %u found in DBC", count, dbc_count);
}

void SpellMgr::LoadSpellScriptTarget()
{
    mSpellScriptTarget.clear();                             // need for reload case

    uint32 count = 0;

    QueryResult_AutoPtr result = WorldDatabase.Query("SELECT entry, type, targetEntry FROM spell_script_target");

    if (!result)
    {
        sLog->outString();
        sLog->outString(">> Loaded 0 spell script target");
        sLog->outErrorDb("spell_script_target table is empty!");
        return;
    }

    do
    {
        Field *fields = result->Fetch();

        uint32 spellId     = fields[0].GetUInt32();
        uint32 type        = fields[1].GetUInt32();
        uint32 targetEntry = fields[2].GetUInt32();

        SpellEntry const* spellProto = sSpellStore.LookupEntry(spellId);

        if (!spellProto)
        {
            sLog->outErrorDb("Table spell_script_target: spellId %u listed for TargetEntry %u does not exist.", spellId, targetEntry);
            continue;
        }

        /*bool targetfound = false;
        for (int i = 0; i <3; ++i)
        {
            if (spellProto->EffectImplicitTargetA[i] == TARGET_UNIT_NEARBY_ENTRY ||
                spellProto->EffectImplicitTargetB[i] == TARGET_UNIT_NEARBY_ENTRY ||
                spellProto->EffectImplicitTargetA[i] == TARGET_DST_NEARBY_ENTRY ||
                spellProto->EffectImplicitTargetB[i] == TARGET_DST_NEARBY_ENTRY)
            {
                targetfound = true;
                break;
            }
        }
        if (!targetfound)
        {
            sLog->outErrorDb("Table spell_script_target: spellId %u listed for TargetEntry %u does not have any implicit target TARGET_UNIT_NEARBY_ENTRY(38) or TARGET_DST_NEARBY_ENTRY (46).", spellId, targetEntry);
            continue;
        }*/

        if (type >= MAX_SPELL_TARGET_TYPE)
        {
            sLog->outErrorDb("Table spell_script_target: target type %u for TargetEntry %u is incorrect.", type, targetEntry);
            continue;
        }

        switch (type)
        {
            case SPELL_TARGET_TYPE_GAMEOBJECT:
            {
                if (targetEntry == 0)
                    break;

                if (!sGOStorage.LookupEntry<GameObjectInfo>(targetEntry))
                {
                    sLog->outErrorDb("Table spell_script_target: gameobject template entry %u does not exist.", targetEntry);
                    continue;
                }
                break;
            }
            default:
            {
                //players
                /*if (targetEntry == 0)
                {
                    sLog->outErrorDb("Table spell_script_target: target entry == 0 for not GO target type (%u).", type);
                    continue;
                }*/
                if (targetEntry && !sCreatureStorage.LookupEntry<CreatureTemplate>(targetEntry))
                {
                    sLog->outErrorDb("Table spell_script_target: creature template entry %u does not exist.", targetEntry);
                    continue;
                }
                const CreatureTemplate* cInfo = sCreatureStorage.LookupEntry<CreatureTemplate>(targetEntry);

                if (spellId == 30427 && !cInfo->SkinLootId)
                {
                    sLog->outErrorDb("Table spell_script_target has creature %u as a target of spellid 30427, but this creature has no skinlootid. Gas extraction will not work!", cInfo->Entry);
                    continue;
                }
                break;
            }
        }

        mSpellScriptTarget.insert(SpellScriptTarget::value_type(spellId, SpellTargetEntry(SpellScriptTargetType(type), targetEntry)));

        ++count;
    } while (result->NextRow());

    // Check all spells
    /* Disabled (lot errors at this moment)
    for (uint32 i = 1; i < sSpellStore.nCount; ++i)
    {
        SpellEntry const * spellInfo = sSpellStore.LookupEntry(i);
        if (!spellInfo)
            continue;

        bool found = false;
        for (int j=0; j<3; ++j)
        {
            if (spellInfo->EffectImplicitTargetA[j] == TARGET_UNIT_NEARBY_ENTRY || spellInfo->EffectImplicitTargetA[j] != TARGET_UNIT_CASTER && spellInfo->EffectImplicitTargetB[j] == TARGET_UNIT_NEARBY_ENTRY)
            {
                SpellScriptTarget::const_iterator lower = sSpellMgr->GetBeginSpellScriptTarget(spellInfo->Id);
                SpellScriptTarget::const_iterator upper = sSpellMgr->GetEndSpellScriptTarget(spellInfo->Id);
                if (lower == upper)
                {
                    sLog->outErrorDb("Spell (ID: %u) has effect EffectImplicitTargetA/EffectImplicitTargetB = %u (TARGET_UNIT_NEARBY_ENTRY), but does not have record in spell_script_target", spellInfo->Id, TARGET_UNIT_NEARBY_ENTRY);
                    break;                                  // effects of spell
                }
            }
        }
    }
    */

    sLog->outString();
    sLog->outString(">> Loaded %u Spell Script Targets", count);
}

void SpellMgr::LoadSpellPetAuras()
{
    mSpellPetAuraMap.clear();                                  // need for reload case

    uint32 count = 0;

    //                                                       0      1    2
    QueryResult_AutoPtr result = WorldDatabase.Query("SELECT spell, pet, aura FROM spell_pet_auras");
    if (!result)
    {
        sLog->outString();
        sLog->outString(">> Loaded %u spell pet auras", count);
        return;
    }

    do
    {
        Field *fields = result->Fetch();

        uint16 spell = fields[0].GetUInt16();
        uint16 pet = fields[1].GetUInt16();
        uint16 aura = fields[2].GetUInt16();

        SpellPetAuraMap::iterator itr = mSpellPetAuraMap.find(spell);
        if (itr != mSpellPetAuraMap.end())
            itr->second.AddAura(pet, aura);
        else
        {
            SpellEntry const* spellInfo = sSpellStore.LookupEntry(spell);
            if (!spellInfo)
            {
                sLog->outErrorDb("Spell %u listed in spell_pet_auras does not exist", spell);
                continue;
            }
            int i = 0;
            for (; i < 3; ++i)
                if ((spellInfo->Effect[i] == SPELL_EFFECT_APPLY_AURA &&
                    spellInfo->EffectApplyAuraName[i] == SPELL_AURA_DUMMY) ||
                    spellInfo->Effect[i] == SPELL_EFFECT_DUMMY)
                    break;

            if (i == 3)
            {
                sLog->outError("Spell %u listed in spell_pet_auras does not have dummy aura or dummy effect", spell);
                continue;
            }

            SpellEntry const* spellInfo2 = sSpellStore.LookupEntry(aura);
            if (!spellInfo2)
            {
                sLog->outErrorDb("Aura %u listed in spell_pet_auras does not exist", aura);
                continue;
            }

            PetAura pa(pet, aura, spellInfo->EffectImplicitTargetA[i] == TARGET_UNIT_PET, spellInfo->EffectBasePoints[i] + spellInfo->EffectBaseDice[i]);
            mSpellPetAuraMap[spell] = pa;
        }

        ++count;
    } while (result->NextRow());

    sLog->outString();
    sLog->outString(">> Loaded %u spell pet auras", count);
}

// set data in core for now
void SpellMgr::LoadSpellCustomAttr()
{
    mSpellCustomAttr.resize(GetSpellStore()->GetNumRows());

    SpellEntry *spellInfo;
    for (uint32 i = 0; i < GetSpellStore()->GetNumRows(); ++i)
    {
        mSpellCustomAttr[i] = 0;
        spellInfo = (SpellEntry*)GetSpellStore()->LookupEntry(i);
        if (!spellInfo)
            continue;

        bool auraSpell = true;
        for (uint8 j = 0; j < 3; ++j)
        {
            if (spellInfo->Effect[j])
                if (spellInfo->Effect[j] != SPELL_EFFECT_APPLY_AURA
                || SpellTargetType[spellInfo->EffectImplicitTargetA[j]] != TARGET_TYPE_UNIT_TARGET)
                //ignore target party for now
                {
                    auraSpell = false;
                    break;
                }
        }
        if (auraSpell)
            mSpellCustomAttr[i] |= SPELL_ATTR_CU_AURA_SPELL;

        for (uint32 j = 0; j < 3; ++j)
        {
            switch (spellInfo->EffectApplyAuraName[j])
            {
                case SPELL_AURA_PERIODIC_DAMAGE:
                case SPELL_AURA_PERIODIC_DAMAGE_PERCENT:
                case SPELL_AURA_PERIODIC_LEECH:
                    mSpellCustomAttr[i] |= SPELL_ATTR_CU_AURA_DOT;
                    break;
                case SPELL_AURA_PERIODIC_HEAL:
                case SPELL_AURA_OBS_MOD_HEALTH:
                    mSpellCustomAttr[i] |= SPELL_ATTR_CU_AURA_HOT;
                    break;
                case SPELL_AURA_MOD_ROOT:
                case SPELL_AURA_MOD_DECREASE_SPEED:
                    mSpellCustomAttr[i] |= SPELL_ATTR_CU_MOVEMENT_IMPAIR;
                    break;
                default:
                    break;
            }

            switch (spellInfo->Effect[j])
            {
                case SPELL_EFFECT_SCHOOL_DAMAGE:
                case SPELL_EFFECT_WEAPON_DAMAGE:
                case SPELL_EFFECT_WEAPON_DAMAGE_NOSCHOOL:
                case SPELL_EFFECT_NORMALIZED_WEAPON_DMG:
                case SPELL_EFFECT_WEAPON_PERCENT_DAMAGE:
                case SPELL_EFFECT_HEAL:
                    mSpellCustomAttr[i] |= SPELL_ATTR_CU_DIRECT_DAMAGE;
                    break;
                case SPELL_EFFECT_CHARGE:
                    if (!spellInfo->speed && !spellInfo->SpellFamilyName)
                        spellInfo->speed = SPEED_CHARGE;
                    mSpellCustomAttr[i] |= SPELL_ATTR_CU_CHARGE;
                    break;
                case SPELL_EFFECT_TRIGGER_SPELL:
                    if (IsPositionTarget(spellInfo->EffectImplicitTargetA[j]) ||
                        spellInfo->Targets & (TARGET_FLAG_SOURCE_LOCATION|TARGET_FLAG_DEST_LOCATION))
                        spellInfo->Effect[j] = SPELL_EFFECT_TRIGGER_MISSILE;
                    break;
            }
        }

        for (uint8 j = 0; j < 3; ++j)
        {
            switch (spellInfo->EffectApplyAuraName[j])
            {
                case SPELL_AURA_MOD_POSSESS:
                case SPELL_AURA_MOD_CONFUSE:
                case SPELL_AURA_MOD_CHARM:
                case SPELL_AURA_MOD_FEAR:
                case SPELL_AURA_MOD_STUN:
                    mSpellCustomAttr[i] |= SPELL_ATTR_CU_AURA_CC;
                    mSpellCustomAttr[i] &= ~SPELL_ATTR_CU_MOVEMENT_IMPAIR;
                    break;
            }
        }

        if (spellInfo->SpellVisual == 3879)
            mSpellCustomAttr[i] |= SPELL_ATTR_CU_CONE_BACK;

        if ((spellInfo->SpellFamilyName == SPELLFAMILY_DRUID && spellInfo->SpellFamilyFlags & 0x1000LL && spellInfo->SpellIconID == 494) || spellInfo->Id == 33745 /* Lacerate */)
            mSpellCustomAttr[i] |= SPELL_ATTR_CU_IGNORE_ARMOR;

        // Modify SchoolMask to allow them critically heal
        // Healthstones
        if (spellInfo->SpellFamilyName == SPELLFAMILY_WARLOCK && spellInfo->SpellFamilyFlags & 0x10000LL)
            spellInfo->SchoolMask = SPELL_SCHOOL_MASK_SHADOW;

        if (spellInfo->AttributesEx2 & SPELL_ATTR_EX2_HEALTH_FUNNEL && spellInfo->SpellIconID == 153)    // Fix Health Funnel
            spellInfo->SpellVisual = 9219;

        switch (spellInfo->SpellFamilyName)
        {
            case SPELLFAMILY_GENERIC:
            {
                 if (spellInfo->Id == 52009)
                     spellInfo->EffectMiscValue[0] = 20865;
                 // Goblin Rocket Launcher
                 else if (spellInfo->SpellIconID == 184 && spellInfo->Attributes == 4259840)
                     mSpellCustomAttr[i] |= SPELL_ATTR_CU_NO_SPELL_DMG_COEFF;
                 // Dragonbreath Chili
                 else if (spellInfo->Id == 15852)
                     spellInfo->Dispel = DISPEL_NONE;
                 // Crab Disguise
                 else if (spellInfo->Id == 46337)
                     spellInfo->AuraInterruptFlags |= AURA_INTERRUPT_FLAG_CAST;
                 else if (spellInfo->SpellIconID == 2367) // remove flag from steam tonk & crashin trashin racers
                     spellInfo->AttributesEx4 &= ~SPELL_ATTR_EX4_FORCE_TRIGGERED;
                 break;
            }
            case SPELLFAMILY_SHAMAN:
            {
                // Flametongue Weapon
                if (spellInfo->SpellFamilyFlags & 2097152 && spellInfo->SpellVisual == 0)
                    mSpellCustomAttr[i] |= SPELL_ATTR_CU_NO_SPELL_DMG_COEFF;
                // Flametongue Totem
                if (spellInfo->Id == 16368)
                    mSpellCustomAttr[i] |= SPELL_ATTR_CU_NO_SPELL_DMG_COEFF;
                break;
            }
            case SPELLFAMILY_PALADIN:
            {
                // Judgement & seal of Light
                if (spellInfo->SpellFamilyFlags & 0x100040000LL)
                    mSpellCustomAttr[i] |= SPELL_ATTR_CU_NO_SPELL_DMG_COEFF;
                // Seal of Righteousness trigger - already computed for parent spell
                else if (spellInfo->SpellIconID == 25 && spellInfo->AttributesEx4 & 0x00800000LL)
                    mSpellCustomAttr[i] |= SPELL_ATTR_CU_FIXED_DAMAGE;
                // Blessing of Sanctuary
                else if (spellInfo->SpellFamilyFlags & 0x10000000LL && spellInfo->SpellIconID == 29)
                    mSpellCustomAttr[i] |= SPELL_ATTR_CU_NO_SPELL_DMG_COEFF;
                // Eye for an Eye
                else if (spellInfo->Id == 25997)
                    mSpellCustomAttr[i] |= SPELL_ATTR_CU_NO_SPELL_DMG_COEFF;
                // Seal of Blood (Backlash damage)
                else if (spellInfo->Id == 32221 || spellInfo->Id == 32220)
                    mSpellCustomAttr[i] |= SPELL_ATTR_CU_NO_SPELL_DMG_COEFF;
                break;
            }
            case SPELLFAMILY_PRIEST:
            {
                // Mana Burn
                if (spellInfo->SpellFamilyFlags & 0x10LL && spellInfo->SpellIconID == 212)
                    mSpellCustomAttr[i] |= SPELL_ATTR_CU_NO_SPELL_DMG_COEFF;
                // Health Link T5 Hunter / Warlock bonus
                else if (spellInfo->Id == 37382)
                    mSpellCustomAttr[i] |= SPELL_ATTR_CU_NO_SPELL_DMG_COEFF;
                // Reflective Shield
                else if (!spellInfo->SpellFamilyFlags && spellInfo->SpellIconID == 566)
                    mSpellCustomAttr[i] |= SPELL_ATTR_CU_NO_SPELL_DMG_COEFF;
                // Retributive Smite
                else if (!spellInfo->SpellFamilyFlags && spellInfo->SpellIconID == 237)
                    mSpellCustomAttr[i] |= SPELL_ATTR_CU_NO_SPELL_DMG_COEFF;
                // Surge of Light
                else if (spellInfo->Id == 33151)
                    spellInfo->EffectApplyAuraName[2] = SPELLMOD_FLAT;
                break;
            }
            case SPELLFAMILY_MAGE:
            {
                // Molten Armor
                if (spellInfo->SpellFamilyFlags & 0x800000000LL)
                    mSpellCustomAttr[i] |= SPELL_ATTR_CU_NO_SPELL_DMG_COEFF;
                // Ignite
                else if (spellInfo->Id == 12654)
                    mSpellCustomAttr[i] |= SPELL_ATTR_CU_FIXED_DAMAGE;
                break;
            }
            case SPELLFAMILY_WARLOCK:
            {
                // Drain Mana
                if (spellInfo->SpellFamilyFlags & 0x10LL && spellInfo->SpellIconID == 548)
                    mSpellCustomAttr[i] |= SPELL_ATTR_CU_NO_SPELL_DMG_COEFF;
                // Healthstone
                else if (spellInfo->SpellFamilyFlags & 0x10000LL)
                    mSpellCustomAttr[i] |= SPELL_ATTR_CU_NO_SPELL_DMG_COEFF;
                break;
            }
            case SPELLFAMILY_WARRIOR:
            case SPELLFAMILY_HUNTER:
            case SPELLFAMILY_ROGUE:
                mSpellCustomAttr[i] |= SPELL_ATTR_CU_NO_SPELL_DMG_COEFF;
                break;
        }

        switch (i)
        {
        case 41635: // Prayer of Mending
            spellInfo->MaxAffectedTargets = 1;
            break;
        case 26029: // dark glare
        case 37433: // spout
        case 43140: case 43215: // flame breath
            mSpellCustomAttr[i] |= SPELL_ATTR_CU_CONE_LINE;
            break;
        case 24340: case 26558: case 28884:     // Meteor
        case 36837: case 38903: case 41276:     // Meteor
        case 26789:                             // Shard of the Fallen Star
        case 31436:                             // Malevolent Cleave
        case 35181:                             // Dive Bomb
        case 40810: case 43267: case 43268:     // Saber Lash
        case 42384:                             // Brutal Swipe
        case 45150:                             // Meteor Slash
            mSpellCustomAttr[i] |= SPELL_ATTR_CU_SHARE_DAMAGE;
            switch (i) // Saber Lash Targets
            {
            case 40810:             spellInfo->MaxAffectedTargets = 3; break;
            case 43267: case 43268: spellInfo->MaxAffectedTargets = 2; break;
            }
            break;
        case 44978: case 45001: case 45002:     // Wild Magic
        case 45004: case 45006: case 45010:     // Wild Magic
        case 31347: // Doom
        case 44869: // Spectral Blast
        case 45027: // Revitalize
        case 45976: // Muru Portal Channel
        case 39365: // Thundering Storm
        case 41071: // Raise Dead
        case 41172: // Rapid Shot
        case 40834: // Agonizing Flames
            spellInfo->MaxAffectedTargets = 1;
            break;
        case 41357: // L1 Acane Charge
        case 41376: // Spite
        case 29576: // Multi-Shot
        case 37790: // Spread Shot
        case 46771: // Flame Sear
        case 45248: // Shadow Blades
        case 41303: // Soul Drain
            spellInfo->MaxAffectedTargets = 3;
            break;
        case 38310: // Multi-Shot
            spellInfo->MaxAffectedTargets = 4;
            break;
        case 42005: // Bloodboil
        case 38296: // Spitfire Totem
        case 37676: // Insidious Whisper
        case 46008: // Negative Energy
        case 45641: // Fire Bloom
            spellInfo->MaxAffectedTargets = 5;
            break;
        case 40827: // Sinful Beam
        case 40859: // Sinister Beam
        case 40860: // Vile Beam
        case 40861: // Wicked Beam
            spellInfo->MaxAffectedTargets = 10;
            break;
        case 8122: case 8124: case 10888: case 10890: // Psychic Scream
        case 12494: // Frostbite
            spellInfo->Attributes |= SPELL_ATTR_BREAKABLE_BY_DAMAGE;
            break;
        case 38794: case 33711: //Murmur's Touch
            spellInfo->MaxAffectedTargets = 1;
            spellInfo->EffectTriggerSpell[0] = 33760;
            break;
        case 32727: // Arena Preparation - remove invisibility aura
        case 44949: // Whirlwind's offhand attack - TODO: remove this (50% weapon damage effect)
            spellInfo->Effect[1] = NULL;
            break;
        case 12723: // Sweeping Strikes proc
            spellInfo->Attributes |= SPELL_ATTR_IMPOSSIBLE_DODGE_PARRY_BLOCK;
            break;
        case 24905: // Moonkin form -> elune's touch
            spellInfo->EffectImplicitTargetA[2] = TARGET_UNIT_CASTER;
            break;
        case 20252: //Intercept
        case 20616:
        case 20617:
        case 25272:
        case 25275:
        case 100:   //Charge
        case 6178:
        case 11578:
            spellInfo->AttributesEx |= SPELL_ATTR_EX_CANT_BE_REFLECTED; 
            break;
        case 7922:                        // Charge stun
        case 25273:                       // Intercept stun  
        case 20615:
        case 25274:                       
            spellInfo->Attributes |= SPELL_ATTR_IMPOSSIBLE_DODGE_PARRY_BLOCK;
            spellInfo->AttributesEx |= SPELL_ATTR_EX_CANT_BE_REFLECTED;
            break;
        case 1833:                        // Cheap Shot
            spellInfo->EffectMechanic[0] = MECHANIC_STUN;
            break;
        case 41013:     // Parasitic Shadowfiend Passive
            spellInfo->EffectApplyAuraName[0] = 4; // proc debuff, and summon infinite fiends
            break;
        case 27892:     // To Anchor 1
        case 27928:     // To Anchor 1
        case 27935:     // To Anchor 1
        case 27915:     // Anchor to Skulls
        case 27931:     // Anchor to Skulls
        case 27937:     // Anchor to Skulls
            spellInfo->rangeIndex = 13;
            break;
        case 34580: // Impale
            mSpellCustomAttr[i] |= SPELL_ATTR_CU_IGNORE_ARMOR;
            break;
        case 33619: // Reflective Shield
        case 5171: // Slice and Dice (Rank 1)
        case 6774: // Slice and Dice (Rank 2)
        case 13810: // Frost Trap
        case 34919: // Vampiric Touch (party mana gain)
        case 15290: // Vampiric Embrace (party hp gain)
        case 1543: // Flare
        case 14157: // Ruthlessness
            spellInfo->AttributesEx3 |= SPELL_ATTR_EX3_NO_INITIAL_AGGRO;
            break;
        case 29200: // Purify Helboar Meat
            spellInfo->EffectImplicitTargetA[0] = TARGET_UNIT_CASTER;
            break;
        case 35460: // Fury of the Dreghood Elders
            spellInfo->EffectImplicitTargetA[1] = TARGET_TYPE_UNIT_TARGET;
            break;
        case 25228: // Soul Link (fix pet + dmg)
            spellInfo->Effect[0] = SPELL_EFFECT_APPLY_AREA_AURA_PET;
            break;
        case 30242: // Demonic Tactics rank 1
        case 30245: // Demonic Tactics rank 2
        case 30246: // Demonic Tactics rank 3
        case 30247: // Demonic Tactics rank 4
        case 30248: // Demonic Tactics rank 5
            spellInfo->EffectApplyAuraName[1] = SPELL_AURA_MOD_CRIT_PERCENT;
            spellInfo->EffectApplyAuraName[2] = SPELL_AURA_MOD_SPELL_CRIT_CHANCE;
            break;
        case 31233: // Find Weakness rank 1
        case 31239: // Find Weakness rank 2
        case 31240: // Find Weakness rank 3
        case 31241: // Find Weakness rank 4
        case 31242: // Find Weakness rank 5
            spellInfo->procFlags = 87376;
            break;
        case 35685: // Electro Shock Therapy - Limit to netherstorm
            spellInfo->AreaId = 3523;
            break;
        case 30452: // Rocket Boots Engaged (trigger?)
        case 51582: // Rocket Boots Engaged
        case 13120: // Net-o-Matic
        case 835:   // Tidal Charm
        case 15712: // Linken's Boomerang
            spellInfo->AttributesEx4 |= SPELL_ATTR_EX4_NOT_USABLE_IN_ARENA;
            break;
        case 32375: // Mass Dispel
        case 32592: // Mass Dispel.
        case 39897: // Mass Dispel
        case 3600:  // Earthbind Totem
        case 1725:  // Distract
        case 36554: // Shadowstep (Parent)
        case 33206: // Pain Supression
        case 44416: // Pain Supression
        case 35339: // Lee's TEST Mind Vision (arena PoV spell)
        case 29858: // Soulshatter
            mSpellCustomAttr[i] = SPELL_ATTR_CU_DONT_BREAK_STEALTH;
            break;
        case 14183: // Premeditation
            spellInfo->DmgClass = SPELL_DAMAGE_CLASS_MAGIC;
            spellInfo->AttributesEx4 |= SPELL_ATTR_EX4_IGNORE_RESISTANCES;
            spellInfo->AttributesEx |= SPELL_ATTR_EX_CANT_BE_REFLECTED;
            mSpellCustomAttr[i] = SPELL_ATTR_CU_DONT_BREAK_STEALTH;
            break;
        case 43730: // Stormchops effect
            spellInfo->EffectTriggerSpell[0] = 43733;
            spellInfo->EffectTriggerSpell[1] = 43731;
            spellInfo->EffectImplicitTargetA[1] = 1;
            spellInfo->EffectImplicitTargetB[1] = 0;
            mSpellCustomAttr[i] |= SPELL_ATTR_CU_WELL_FED;
            break;
        case 30174: // Riding Turtle 100% speed
            spellInfo->Effect[1] = SPELL_EFFECT_APPLY_AURA;
            spellInfo->EffectImplicitTargetA[1] = TARGET_UNIT_CASTER;
            spellInfo->EffectApplyAuraName[1] = SPELL_AURA_MOD_INCREASE_MOUNTED_SPEED;
            spellInfo->EffectBasePoints[1] = 99;
            break;
        case 10787: // Panther
        case 10788: // Leopard
        case 10789: // Spotted Frostsaber
        case 10790: // Bengal Tiger
        case 42929: // [DNT] Test Mount
        case 46197: // X-51 Nether-Rocket
            spellInfo->EffectBasePoints[1] = 99;
            break;
        case 32240: // Snowy Gryphon
            spellInfo->EffectBasePoints[1] = 279;
            break;
        case 18191: // Food that should act like Well Fed
        case 46687: // NOTE: Somebody needs to comment this
        case 5257:
        case 5021:
        case 5020:
        case 22789:
        case 37058:
        case 25804:
        case 25722:
        case 25037:
        case 20875:
        case 18193:
        case 18125:
        case 18192:
        case 18141:
        case 18194:
        case 18222:
        case 22730:
        case 23697:
        case 25661:
            mSpellCustomAttr[i] |= SPELL_ATTR_CU_WELL_FED;
            break;
        case 66: // Invisibility Fade
        case 20580: // Shadowmeld (does not get removed when casting mass dispell)
            spellInfo->AuraInterruptFlags |= AURA_INTERRUPT_FLAG_CAST;
            break;
        case 26373: // Lunar Invitation
            spellInfo->EffectImplicitTargetA[1] = TARGET_UNIT_CASTER;
            spellInfo->EffectImplicitTargetB[1] = 0;
            break;
        case 27066: // Trueshot Aura 4
            spellInfo->Dispel = 0;
            break;
        case 24531: // Refocus (Renataki's Charm of Beasts)
            spellInfo->EffectImplicitTargetA[0] = TARGET_UNIT_CASTER;
            break;
        case 39992: // Needle Spine
            spellInfo->MaxAffectedTargets = 3;
            spellInfo->AttributesEx3 |= SPELL_ATTR_EX3_PLAYERS_ONLY;
            break;
        case 1953: // Blink
            spellInfo->DurationIndex = 328; // 250ms
            break;
        case 14748: // Improved Power Word: Shield (Rank 1) Implemented in Aura::HandleSchoolAbsorb
        case 14768: // Improved Power Word: Shield (Rank 2) Implemented in Aura::HandleSchoolAbsorb
        case 14769: // Improved Power Word: Shield (Rank 3) Implemented in Aura::HandleSchoolAbsorb
            spellInfo->EffectItemType[0] = 0;
            break;
        case 6789: // Death Coil
        case 17925:
        case 17926:
        case 27223:
        case 23694: // Improved Hamstring
        case 19229: // Improved Wing Clip
        case 44041: // Chastise
        case 44043:
        case 44044:
        case 44045:
        case 44046:
        case 44047:
        case 45334: // Feral Charge Effect
            mSpellCustomAttr[i] |= SPELL_ATTR_CU_DONT_BREAK_ON_DAMAGE;
            break;
        case 16614: // Storm Gauntlets
        case 7714:  // Fiery Plate Gauntlets
        case 40471: // Enduring Light - T6 proc
        case 40472: // Enduring Judgement - T6 proc
        case 32221: // Seal of Blood
        case 32220: // Judgement of Blood
        case 28715: // Flame Cap
        case 37284: // Scalding Water
        case 6297:  // Fiery Blaze
        case 46579: // Deathfrost
        case 28734: // Mana Tap
        case 32409: // SW: Death
        case 45297: // Chain Lightning
        case 23102: // Frostbolt
        case 9487:  // Fireball
        case 45429: // Shattered Sun Pendant of Acumen: Scryers ex proc
        case 27655: // Heart of Wyrmthalak: Flame Lash proc
        case 45055: // Shadow Bolt (Timbal's Focusing Crystal)
        case 37661: // The Lightning Capacitor, lightning bolt spell
        case 28733: // Arcane Torrent
        case 43731: // Lightning Zap on critters (Stormchops)
        case 43733: // Lightning Zap on others (Stormchops)
            mSpellCustomAttr[i] |= SPELL_ATTR_CU_NO_SPELL_DMG_COEFF;
            break;
        case 408: // Kidney Shot
        case 8643:
            spellInfo->Effect[1] = 0;
            spellInfo->EffectApplyAuraName[1] = 0;
            spellInfo->Effect[2] = 0;
            spellInfo->EffectApplyAuraName[2] = 0;
            break;
        case 32381: // Empowered Corruption (Rank I) 12%
        case 32382: // Empowered Corruption (Rank II) 24%
        case 32383: // Empowered Corruption (Rank III) 36%
            spellInfo->EffectBasePoints[0] = ((spellInfo->EffectBasePoints[0] + 1) * 6) - 1;
            break;
        case 19975: // Entangling Roots (Nature's Grasp trigger) Rank 1
        case 19974: // Entangling Roots (Nature's Grasp trigger) Rank 2
        case 19973: // Entangling Roots (Nature's Grasp trigger) Rank 3
        case 19972: // Entangling Roots (Nature's Grasp trigger) Rank 4
        case 19971: // Entangling Roots (Nature's Grasp trigger) Rank 5
        case 19970: // Entangling Roots (Nature's Grasp trigger) Rank 6
        case 27010: // Entangling Roots (Nature's Grasp trigger) Rank 7
            spellInfo->CastingTimeIndex = 0;
            break;
        case 30171: // Fiendish Portal - Should be fixed in Spell::EffectSummonType to use data from spell_target_position
            spellInfo->Effect[0] = 0;
            break;
        case 30834: // Infernal Relay
            spellInfo->AttributesEx2 |= SPELL_ATTR_EX2_IGNORE_LOS;
            break;
        case 29955: // Arcane Missiles (Shade of Aran - Karazhan)
            spellInfo->EffectImplicitTargetA[0] = TARGET_UNIT_CASTER;
            spellInfo->EffectImplicitTargetA[2] = TARGET_UNIT_TARGET_ENEMY;
            break;
        case 1852: // Silenced (GM spell)
            spellInfo->AttributesEx3 |= SPELL_ATTR_EX3_DEATH_PERSISTENT;
            break;
        default:
            break;
        }
    }
    CreatureAI::FillAISpellInfo();
}

void SpellMgr::LoadSpellLinked()
{
    mSpellLinkedMap.clear();    // need for reload case
    uint32 count = 0;

    //                                                       0              1             2
    QueryResult_AutoPtr result = WorldDatabase.Query("SELECT spell_trigger, spell_effect, type FROM spell_linked_spell");
    if (!result)
    {
        sLog->outString();
        sLog->outString(">> Loaded %u linked spells", count);
        return;
    }

    do
    {
        Field *fields = result->Fetch();

        int32 trigger = fields[0].GetInt32();
        int32 effect = fields[1].GetInt32();
        int32 type = fields[2].GetInt32();

        SpellEntry const* spellInfo = sSpellStore.LookupEntry(abs(trigger));
        if (!spellInfo)
        {
            sLog->outErrorDb("Spell Trigger %i listed in spell_linked_spell does not exist", trigger);
            continue;
        }
        spellInfo = sSpellStore.LookupEntry(abs(effect));
        if (!spellInfo)
        {
            sLog->outErrorDb("Spell Effect %i listed in spell_linked_spell does not exist", effect);
            continue;
        }

        if (trigger > 0)
        {
            switch (type)
            {
                case 0: mSpellCustomAttr[trigger] |= SPELL_ATTR_CU_LINK_CAST; break;
                case 1: mSpellCustomAttr[trigger] |= SPELL_ATTR_CU_LINK_HIT;  break;
                case 2: mSpellCustomAttr[trigger] |= SPELL_ATTR_CU_LINK_AURA; break;
            }
        }
        else
        {
            mSpellCustomAttr[-trigger] |= SPELL_ATTR_CU_LINK_REMOVE;
        }

        if (type) //we will find a better way when more types are needed
        {
            if (trigger > 0)
                trigger += SPELL_LINKED_MAX_SPELLS * type;
            else
                trigger -= SPELL_LINKED_MAX_SPELLS * type;
        }
        mSpellLinkedMap[trigger].push_back(effect);

        ++count;
    } while (result->NextRow());

    sLog->outString();
    sLog->outString(">> Loaded %u linked spells", count);
}

// Some checks for spells, to prevent adding depricated/broken spells for trainers, spell book, etc
bool SpellMgr::IsSpellValid(SpellEntry const* spellInfo, Player* pl, bool msg)
{
    // not exist
    if (!spellInfo)
        return false;

    bool need_check_reagents = false;

    // check effects
    for (int i = 0; i < 3; ++i)
    {
        switch (spellInfo->Effect[i])
        {
            case 0:
                continue;

                // craft spell for crafting non-existed item (break client recipes list show)
            case SPELL_EFFECT_CREATE_ITEM:
            {
                if (!ObjectMgr::GetItemPrototype(spellInfo->EffectItemType[i]))
                {
                    if (msg)
                    {
                        if (pl)
                            ChatHandler(pl).PSendSysMessage("Craft spell %u create not-exist in DB item (Entry: %u) and then...", spellInfo->Id, spellInfo->EffectItemType[i]);
                        else
                            sLog->outErrorDb("Craft spell %u create not-exist in DB item (Entry: %u) and then...", spellInfo->Id, spellInfo->EffectItemType[i]);
                    }
                    return false;
                }

                need_check_reagents = true;
                break;
            }
            case SPELL_EFFECT_LEARN_SPELL:
            {
                SpellEntry const* spellInfo2 = sSpellStore.LookupEntry(spellInfo->EffectTriggerSpell[i]);
                if (!IsSpellValid(spellInfo2, pl, msg))
                {
                    if (msg)
                    {
                        if (pl)
                            ChatHandler(pl).PSendSysMessage("Spell %u learn to broken spell %u, and then...", spellInfo->Id, spellInfo->EffectTriggerSpell[i]);
                        else
                            sLog->outErrorDb("Spell %u learn to invalid spell %u, and then...", spellInfo->Id, spellInfo->EffectTriggerSpell[i]);
                    }
                    return false;
                }
                break;
            }
        }
    }

    if (need_check_reagents)
    {
        for (int j = 0; j < 8; ++j)
        {
            if (spellInfo->Reagent[j] > 0 && !ObjectMgr::GetItemPrototype(spellInfo->Reagent[j]))
            {
                if (msg)
                {
                    if (pl)
                        ChatHandler(pl).PSendSysMessage("Craft spell %u has invalid reagent in DB item (Entry: %u) and then...", spellInfo->Id, spellInfo->Reagent[j]);
                    else
                        sLog->outErrorDb("Craft spell %u has invalid reagent in DB item (Entry: %u) and then...", spellInfo->Id, spellInfo->Reagent[j]);
                }
                return false;
            }
        }
    }

    return true;
}

bool IsSpellAllowedInLocation(SpellEntry const *spellInfo, uint32 map_id, uint32 zone_id, uint32 area_id)
{
    // normal case
    if (spellInfo->AreaId && spellInfo->AreaId != zone_id && spellInfo->AreaId != area_id)
        return false;

    // continent limitation (virtual continent)
    if ((spellInfo->AttributesEx4 & SPELL_ATTR_EX4_CAST_ONLY_IN_OUTLAND))
    {
        uint32 v_map = GetVirtualMapForMapAndZone(map_id, zone_id);
        MapEntry const* mapEntry = sMapStore.LookupEntry(v_map);
        if (!mapEntry || mapEntry->addon < 1 || !mapEntry->IsContinent())
            return SPELL_FAILED_REQUIRES_AREA;
    }

    // raid instance limitation
    if (spellInfo->AttributesEx6 & SPELL_ATTR_EX6_NOT_IN_RAID_INSTANCE)
    {
        MapEntry const* mapEntry = sMapStore.LookupEntry(map_id);
        if (!mapEntry || mapEntry->IsRaid())
            return SPELL_FAILED_REQUIRES_AREA;
    }

    // elixirs (all area dependent elixirs have family SPELLFAMILY_POTION, use this for speedup)
    if (spellInfo->SpellFamilyName == SPELLFAMILY_POTION)
    {
        if (uint32 mask = sSpellMgr->GetSpellElixirMask(spellInfo->Id))
        {
            if (mask & ELIXIR_BATTLE_MASK)
            {
                if (spellInfo->Id == 45373)                    // Bloodberry Elixir
                    return zone_id == 4075;
            }
            if (mask & ELIXIR_UNSTABLE_MASK)
            {
                // in the Blade's Edge Mountains Plateaus and Gruul's Lair.
                return zone_id == 3522 || map_id == 565;
            }
            if (mask & ELIXIR_SHATTRATH_MASK)
            {
                // in Tempest Keep, Serpentshrine Cavern, Caverns of Time: Mount Hyjal, Black Temple, Sunwell Plateau
                if (zone_id == 3607 || map_id == 534 || map_id == 564 || zone_id == 4075)
                    return true;

                MapEntry const* mapEntry = sMapStore.LookupEntry(map_id);
                if (!mapEntry)
                    return false;

                return mapEntry->multimap_id == 206;
            }

            // elixirs not have another limitations
            return true;
        }
    }

    // special cases zone check (maps checked by multimap common id)
    switch (spellInfo->Id)
    {
        case 23333:                                         // Warsong Flag
        case 23335:                                         // Silverwing Flag
        case 46392:                                         // Focused Assault
        case 46393:                                         // Brutal Assault
        {
            MapEntry const* mapEntry = sMapStore.LookupEntry(map_id);
            if (!mapEntry)
                return false;

            if (!mapEntry->IsBattleGround())
                return false;

            if (zone_id == 3277)
                return true;

            return false;
        }
        case 34976:                                         // Netherstorm Flag
        {
            MapEntry const* mapEntry = sMapStore.LookupEntry(map_id);
            if (!mapEntry)
                return false;

            if (!mapEntry->IsBattleGround())
                return false;

            if (zone_id == 3820)
                return true;

            return false;
        }
        case 29866:                                         // Cast Fishing Net
            return area_id == 3857;
        case 32307:                                         // Warmaul Ogre Banner
            return area_id == 3637;
        case 32724:                                         // Gold Team (Alliance)
        case 32725:                                         // Green Team (Alliance)
        case 32727:                                         // Arena Preparation
        case 35774:                                         // Gold Team (Horde)
        case 35775:                                         // Green Team (Horde)
        {
            MapEntry const* mapEntry = sMapStore.LookupEntry(map_id);
            if (!mapEntry)
                return false;

            //the follow code doesn't work.
            //if (!mapEntry->IsBattleArena())
            //    return false;

            //this is the working code, HACK
            if (zone_id == 3702 || zone_id == 3968 || zone_id == 3698)
                return true;

            return false;
        }
        case 41618:                                         // Bottled Nethergon Energy
        case 41620:                                         // Bottled Nethergon Vapor
        {
            MapEntry const* mapEntry = sMapStore.LookupEntry(map_id);
            if (!mapEntry)
                return false;

            return mapEntry->multimap_id == 206;
        }
        case 41617:                                         // Cenarion Mana Salve
        case 41619:                                         // Cenarion Healing Salve
        {
            MapEntry const* mapEntry = sMapStore.LookupEntry(map_id);
            if (!mapEntry)
                return false;

            return mapEntry->multimap_id == 207;
        }
        case 40216:                                         // Dragonmaw Illusion
        case 42016:                                         // Dragonmaw Illusion
            return area_id == 3759 || area_id == 3966 || area_id == 3939;
        case 2584:                                          // Waiting to Resurrect
        case 22011:                                         // Spirit Heal Channel
        case 22012:                                         // Spirit Heal
        case 24171:                                         // Resurrection Impact Visual
        case 42792:                                         // Recently Dropped Flag
        case 43681:                                         // Inactive
        case 44535:                                         // Spirit Heal (mana)
        case 44521:                                         // Preparation
        {
            MapEntry const* mapEntry = sMapStore.LookupEntry(map_id);
            if (!mapEntry)
                return false;

            if (!mapEntry->IsBattleGround())
                return false;
        }
    }

    return true;
}

void SpellMgr::LoadSkillLineAbilityMap()
{
    mSkillLineAbilityMap.clear();

    uint32 count = 0;

    for (uint32 i = 0; i < sSkillLineAbilityStore.GetNumRows(); i++)
    {
        SkillLineAbilityEntry const *SkillInfo = sSkillLineAbilityStore.LookupEntry(i);
        if (!SkillInfo)
            continue;

        mSkillLineAbilityMap.insert(SkillLineAbilityMap::value_type(SkillInfo->spellId, SkillInfo));
        ++count;
    }

    sLog->outString();
    sLog->outString(">> Loaded %u SkillLineAbility MultiMap", count);
}

DiminishingGroup GetDiminishingReturnsGroupForSpell(SpellEntry const* spellproto, bool triggered)
{
    // Explicit Diminishing Groups
    switch (spellproto->SpellFamilyName)
    {
        case SPELLFAMILY_MAGE:
        {
            // Polymorph
            if ((spellproto->SpellFamilyFlags & 0x00001000000LL) && spellproto->EffectApplyAuraName[0] == SPELL_AURA_MOD_CONFUSE)
                return DIMINISHING_POLYMORPH;
            break;
        }
        case SPELLFAMILY_ROGUE:
        {
            // Kidney Shot
            if (spellproto->SpellFamilyFlags & 0x00000200000LL)
                return DIMINISHING_KIDNEYSHOT;
            // Sap
            else if (spellproto->SpellFamilyFlags & 0x00000000080LL)
                return DIMINISHING_POLYMORPH;
            // Gouge
            else if (spellproto->SpellFamilyFlags & 0x00000000008LL)
                return DIMINISHING_POLYMORPH;
            // Blind
            else if (spellproto->SpellFamilyFlags & 0x00001000000LL)
                return DIMINISHING_BLIND_CYCLONE;
            break;
        }
        case SPELLFAMILY_HUNTER:
        {
            // Freezing trap
            if (spellproto->SpellFamilyFlags & 0x00000000008LL)
                return DIMINISHING_FREEZE;
            break;
        }
        case SPELLFAMILY_WARLOCK:
        {
            // Death Coil
            if (spellproto->SpellFamilyFlags & 0x00000080000LL)
                return DIMINISHING_DEATHCOIL;
            // Seduction
            else if (spellproto->SpellFamilyFlags & 0x00040000000LL && spellproto->SpellIconID == 48)
                return DIMINISHING_FEAR;
            // Unstable affliction dispel silence
            else if (spellproto->Id == 31117)
                return DIMINISHING_UNSTABLE_AFFLICTION;
            break;
        }
        case SPELLFAMILY_DRUID:
        {
            // Cyclone
            if (spellproto->SpellFamilyFlags & 0x02000000000LL)
                return DIMINISHING_BLIND_CYCLONE;
            // Nature's Grasp trigger
            if (spellproto->SpellFamilyFlags & 0x00000000200LL && spellproto->Attributes == 0x49010000)
                return DIMINISHING_CONTROL_ROOT;
            // Feral Charge (Root Effect)
            if (spellproto->Id == 45334)
                return DIMINISHING_NONE;
            break;
        }
        case SPELLFAMILY_WARRIOR:
        {
            // Hamstring - limit duration to 10s in PvP
            if (spellproto->SpellFamilyFlags & 0x00000000002LL)
                return DIMINISHING_LIMITONLY;
            break;
        }
        case SPELLFAMILY_PALADIN:
        {
            // Turn Evil
            if (spellproto->Id == 10326)
                return DIMINISHING_FEAR;
            break;
            //spellproto->SpellFamilyFlags & 0x00000004000LL &&
        }
        default:
        {
            if (spellproto->Id == 12494) // frostbite
                return DIMINISHING_TRIGGER_ROOT;

            // Intimidation
            if (spellproto->Id == 24394)
                return DIMINISHING_CONTROL_STUN;
            break;
        }
    }

    // Get by mechanic
    for (uint8 i = 0;i < 3;++i)
    {
        if (spellproto->Mechanic == MECHANIC_STUN    || spellproto->EffectMechanic[i] == MECHANIC_STUN)
            return triggered ? DIMINISHING_TRIGGER_STUN : DIMINISHING_CONTROL_STUN;
        else if (spellproto->Mechanic == MECHANIC_SLEEP   || spellproto->EffectMechanic[i] == MECHANIC_SLEEP)
            return DIMINISHING_SLEEP;
        else if (spellproto->Mechanic == MECHANIC_ROOT    || spellproto->EffectMechanic[i] == MECHANIC_ROOT)
            return triggered ? DIMINISHING_TRIGGER_ROOT : DIMINISHING_CONTROL_ROOT;
        else if (spellproto->Mechanic == MECHANIC_FEAR    || spellproto->EffectMechanic[i] == MECHANIC_FEAR)
            return DIMINISHING_FEAR;
        else if (spellproto->Mechanic == MECHANIC_CHARM   || spellproto->EffectMechanic[i] == MECHANIC_CHARM)
            return DIMINISHING_CHARM;
        else if (spellproto->Mechanic == MECHANIC_DISARM  || spellproto->EffectMechanic[i] == MECHANIC_DISARM)
            return DIMINISHING_DISARM;
        else if (spellproto->Mechanic == MECHANIC_FREEZE  || spellproto->EffectMechanic[i] == MECHANIC_FREEZE)
            return DIMINISHING_FREEZE;
        else if (spellproto->Mechanic == MECHANIC_KNOCKOUT|| spellproto->EffectMechanic[i] == MECHANIC_KNOCKOUT ||
                 spellproto->Mechanic == MECHANIC_SAPPED  || spellproto->EffectMechanic[i] == MECHANIC_SAPPED)
            return DIMINISHING_KNOCKOUT;
        else if (spellproto->Mechanic == MECHANIC_BANISH  || spellproto->EffectMechanic[i] == MECHANIC_BANISH)
            return DIMINISHING_BANISH;
    }

    return DIMINISHING_NONE;
}

bool IsDiminishingReturnsGroupDurationLimited(DiminishingGroup group)
{
    switch (group)
    {
        case DIMINISHING_CONTROL_STUN:
        case DIMINISHING_TRIGGER_STUN:
        case DIMINISHING_KIDNEYSHOT:
        case DIMINISHING_SLEEP:
        case DIMINISHING_CONTROL_ROOT:
        case DIMINISHING_TRIGGER_ROOT:
        case DIMINISHING_FEAR:
        case DIMINISHING_WARLOCK_FEAR:
        case DIMINISHING_CHARM:
        case DIMINISHING_POLYMORPH:
        case DIMINISHING_FREEZE:
        case DIMINISHING_KNOCKOUT:
        case DIMINISHING_BLIND_CYCLONE:
        case DIMINISHING_BANISH:
        case DIMINISHING_LIMITONLY:
            return true;
    }
    return false;
}

DiminishingReturnsType GetDiminishingReturnsGroupType(DiminishingGroup group)
{
    switch (group)
    {
        case DIMINISHING_BLIND_CYCLONE:
        case DIMINISHING_CONTROL_STUN:
        case DIMINISHING_TRIGGER_STUN:
        case DIMINISHING_KIDNEYSHOT:
            return DRTYPE_ALL;
        case DIMINISHING_SLEEP:
        case DIMINISHING_CONTROL_ROOT:
        case DIMINISHING_TRIGGER_ROOT:
        case DIMINISHING_FEAR:
        case DIMINISHING_CHARM:
        case DIMINISHING_POLYMORPH:
        case DIMINISHING_DISARM:
        case DIMINISHING_DEATHCOIL:
        case DIMINISHING_FREEZE:
        case DIMINISHING_BANISH:
        case DIMINISHING_WARLOCK_FEAR:
        case DIMINISHING_KNOCKOUT:
        case DIMINISHING_UNSTABLE_AFFLICTION:
            return DRTYPE_PLAYER;
    }

    return DRTYPE_NONE;
}

