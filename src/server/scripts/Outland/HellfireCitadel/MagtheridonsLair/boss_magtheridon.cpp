 /*
  * Copyright (C) 2008-2017 Smolderforge <https://ww.smolderforge.com/>
  * Copyright (C) 2010-2012 Project SkyFire <http://www.projectskyfire.org/>
  * Copyright (C) 2010-2012 Oregon <http://www.oregoncore.com/>
  * Copyright (C) 2006-2008 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
  * Copyright (C) 2008-2012 TrinityCore <http://www.trinitycore.org/>
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

/* ScriptData
SDName: Boss_Magtheridon
SD%Complete: 99
SDComment: Cubes should cancel when player does (only visual)
SDCategory: Hellfire Citadel, Magtheridon's lair
EndScriptData */

#include "ScriptPCH.h"
#include "magtheridons_lair.h"

enum MagtheridonTexts
{
    RANDOM_YELL_1      = -1544000,
    RANDOM_YELL_2      = -1544001,
    RANDOM_YELL_3      = -1544002,
    RANDOM_YELL_4      = -1544003,
    RANDOM_YELL_5      = -1544004,
    RANDOM_YELL_6      = -1544005,

    SAY_FREED              = -1544006,
    SAY_AGGRO              = -1544007,
    SAY_BANISH             = -1544008,
    SAY_CHAMBER_DESTROY    = -1544009,
    SAY_PLAYER_KILLED      = -1544010,
    SAY_DEATH              = -1544011
};

enum MagtheridonEmotes
{
    EMOTE_BERSERK       = -1544012,
    EMOTE_BLASTNOVA     = -1544013,
    EMOTE_BEGIN         = -1544014
};

enum MagtheridonMobEntries
{
    MOB_MAGTHERIDON         = 17257,
    MOB_MAGTHERIDON_ROOM    = 17516,
    MOB_HELLFIRE_CHANNELLER = 17256,
    MOB_HELLFIRE_TRIGGER    = 17376,
    MOB_ABYSSAL             = 17454,
    MOB_MAGTHERIDON_TRIGGER = 19703,
    MOB_BLAZE_FIRE          = 100003
};

enum MagtheridonSpells
{
    SPELL_BLASTNOVA             = 30616,
    SPELL_CLEAVE                = 30619,
    SPELL_QUAKE_KNOCKBACK       = 30571,
    SPELL_QUAKE_TRIGGER         = 30576,    // proper trigger for Quake
    SPELL_BLAZE_TARGET          = 30541,
    SPELL_BLAZE_TRAP            = 30542,
    SPELL_DEBRIS_KNOCKDOWN      = 36449,
    SPELL_DEBRIS                = 30632,
    SPELL_DEBRIS_DAMAGE         = 30631,
    SPELL_CAMERA_SHAKE          = 36455,
    SPELL_BERSERK               = 27680,

    SPELL_SHADOW_CAGE           = 30168,
    SPELL_SHADOW_GRASP          = 30410,
    SPELL_SHADOW_GRASP_VISUAL   = 30166,
    SPELL_MIND_EXHAUSTION       = 44032,

    SPELL_SHADOW_CAGE_C         = 30205,
    SPELL_SHADOW_GRASP_C        = 30207,
    SPELL_SOUL_TRANSFER         = 30531,

    SPELL_SHADOW_BOLT_VOLLEY    = 30510,
    SPELL_DARK_MENDING          = 30528,
    SPELL_FEAR                  = 30530,    //39176
    SPELL_BURNING_ABYSSAL       = 30511,

    SPELL_FIRE_BLAST            = 37110
};

// count of clickers needed to interrupt blast nova
#define CLICKERS_COUNT              5

typedef std::map<uint64, uint64> CubeMap;

struct mob_abyssalAI : public ScriptedAI
{
    mob_abyssalAI(Creature *c) : ScriptedAI(c)
    {
        Despawn_Timer = 60000;
    }

    uint32 FireBlast_Timer;
    uint32 Despawn_Timer;

    void Reset()
    {
        FireBlast_Timer = 6000;
    }

    void EnterCombat(Unit*)
    {
        DoZoneInCombat();
    }

    void JustReachedHome()
    {
        Despawn_Timer = 2000;
    }

    void UpdateAI(const uint32 diff)
    {
        if (Despawn_Timer <= diff)
            me->ForcedDespawn();
        else
            Despawn_Timer -= diff;

        if (!UpdateVictim())
            return;

        if (FireBlast_Timer <= diff)
        {
            DoCast(me->getVictim(), SPELL_FIRE_BLAST);
            FireBlast_Timer = urand(5000, 15000);
        }
        else
            FireBlast_Timer -= diff;

        DoMeleeAttackIfReady();
    }
};

struct mob_magtheridon_triggerAI : public Scripted_NoMovementAI
{
    mob_magtheridon_triggerAI(Creature *c) : Scripted_NoMovementAI(c)
    {
        me->SetDisplayId(11686); // blank - must be visible so players see effect
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        despawnTimer = 0;
    }

    uint32 debrisTimer;
    uint32 despawnTimer;

    void DoDebris()
    {
        DoCast(me, SPELL_DEBRIS, true);
        debrisTimer = 5000;
        despawnTimer = 6000;
    }

    void SpellHit(Unit*, const SpellEntry *spell)
    {
        if (spell->Id == SPELL_BLAZE_TARGET)
        {
            me->CastSpell(me, SPELL_BLAZE_TRAP, true);
            despawnTimer = 130000; // 2 min spell duration, give some leeway
        }
    }

    void UpdateAI(const uint32 diff)
    {
        if (debrisTimer)
        {
            if (debrisTimer <= diff)
            {
                DoCast(me, SPELL_DEBRIS_DAMAGE, true);
                debrisTimer = 0;
            }
            else
                debrisTimer -= diff;
        }

        if (despawnTimer)
        {
            if (despawnTimer <= diff)
                me->ForcedDespawn();
            else
                despawnTimer -= diff;
        }
    }
};

struct boss_magtheridonAI : public ScriptedAI
{
    boss_magtheridonAI(Creature *c) : ScriptedAI(c)
    {
        instance = me->GetInstanceScript();
        me->SetFloatValue(UNIT_FIELD_BOUNDINGRADIUS, 10);
        me->SetFloatValue(UNIT_FIELD_COMBATREACH, 10);

        // target 7, random target with certain entry spell, need core fix
        // blaze will NOT PROC ON TRIGGERS IF THIS IS REMOVED
        SpellEntry *TempSpell;
        TempSpell = GET_SPELL(SPELL_BLAZE_TARGET);
        if (TempSpell && TempSpell->EffectImplicitTargetA[0] != 6)
        {
            TempSpell->EffectImplicitTargetA[0] = 6;
            TempSpell->EffectImplicitTargetB[0] = 0;
        }
    }

    CubeMap Cube;

    ScriptedInstance* instance;

    uint32 AttackBegin_Timer;
    uint32 Berserk_Timer;
    uint32 Quake_Timer;
    uint32 QuakeKnockback_Timer;
    uint32 Cleave_Timer;
    uint32 BlastNova_Timer;
    uint32 Blaze_Timer;
    uint32 Debris_Timer;
    uint32 RandChat_Timer;

    uint8 knockCount;
    bool isQuaking;

    bool Phase3;
    bool NeedCheckCube;

    void Reset()
    {
        if (instance)
        {
            instance->SetData(DATA_CHANNELER_EVENT, NOT_STARTED);
            instance->SetData(DATA_MAGTHERIDON_EVENT, NOT_STARTED);
            instance->SetData(DATA_COLLAPSE, false);
        }

        AttackBegin_Timer = 0;
        Berserk_Timer = 1320000;
        Quake_Timer = 40000;
        QuakeKnockback_Timer = 0;
        Debris_Timer = 10000;
        Blaze_Timer = urand(10000, 30000);
        BlastNova_Timer = urand(50000, 60000);
        Cleave_Timer = 15000;
        RandChat_Timer = 90000;

        knockCount = 0;
        isQuaking = false;

        Phase3 = false;

        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_OOC_NOT_ATTACKABLE);
        me->CastSpell(me, SPELL_SHADOW_CAGE_C, true);

        if (me->GetDistance2d(me->GetHomePosition().GetPositionX(), me->GetHomePosition().GetPositionY()) < 2) // don't root instantly on reset, use justreachedhome
            me->addUnitState(UNIT_STAT_STUNNED);

        me->NearTeleportTo(me->GetHomePosition().GetPositionX(), me->GetHomePosition().GetPositionY(), me->GetHomePosition().GetPositionZ(), me->GetHomePosition().GetOrientation());
    }

    void JustReachedHome()
    {
        me->addUnitState(UNIT_STAT_STUNNED);
    }

    void DoFreed()
    {
        me->TextEmote("breaks free!", 0);
        me->RemoveAurasDueToSpell(SPELL_SHADOW_CAGE_C);
        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        DoScriptText(SAY_FREED, me);
        me->HandleEmoteCommand(15); // roar
        AttackBegin_Timer = 6500;
    }

    void SetClicker(uint64 cubeGUID, uint64 clickerGUID)
    {
        // to avoid multiclicks from 1 cube
        if (uint64 guid = Cube[cubeGUID])
            DebuffClicker(Unit::GetUnit(*me, guid));

        Cube[cubeGUID] = clickerGUID;
        NeedCheckCube = true;
    }

    //function to interrupt channeling and debuff clicker with mind exh(used if second person clicks with same cube or after dispeling/ending shadow grasp DoT)
    void DebuffClicker(Unit *clicker)
    {
        if (!clicker || !clicker->isAlive())
            return;

        clicker->RemoveAurasDueToSpell(SPELL_SHADOW_GRASP); // cannot interrupt triggered spells
        clicker->InterruptNonMeleeSpells(false);
    }

    void NeedCheckCubeStatus()
    {
        uint32 ClickerNum = 0;
        // now checking if every clicker has debuff from manticron
        // if not - apply mind exhaustion and delete from clickers list
        for (CubeMap::iterator i = Cube.begin(); i != Cube.end(); ++i)
        {
            Unit *clicker = Unit::GetUnit(*me, (*i).second);
            if (!clicker || !clicker->HasAura(SPELL_SHADOW_GRASP, 1))
            {
                DebuffClicker(clicker);
                (*i).second = 0;
            }
            else
                ClickerNum++;
        }

        // if 5 clickers from other cubes apply shadow cage
        if (ClickerNum >= CLICKERS_COUNT && !me->HasAura(SPELL_SHADOW_CAGE, 0))
        {
            DoScriptText(SAY_BANISH, me);
            me->CastSpell(me, SPELL_SHADOW_CAGE, true);
        }
        else if (ClickerNum < CLICKERS_COUNT && me->HasAura(SPELL_SHADOW_CAGE, 0))
            me->RemoveAurasDueToSpell(SPELL_SHADOW_CAGE);

        if (!ClickerNum)
            NeedCheckCube = false;
    }

    void KilledUnit(Unit* victim)
    {
        DoScriptText(SAY_PLAYER_KILLED, me);
    }

    void JustDied(Unit* Killer)
    {
        if (instance)
            instance->SetData(DATA_MAGTHERIDON_EVENT, DONE);

        DoScriptText(SAY_DEATH, me);
    }

    void MoveInLineOfSight(Unit*) {}

    void AttackStart(Unit *who)
    {
        if (!me->hasUnitState(UNIT_STAT_STUNNED))
            ScriptedAI::AttackStart(who);
    }

    void EnterCombat(Unit *who)
    {
        if (!who && instance && instance->GetData(DATA_MAGTHERIDON_EVENT) == NOT_STARTED)
        {
            instance->SetData(DATA_MAGTHERIDON_EVENT, SPECIAL);
            DoFreed();
        }
    }

    void UpdateAI(const uint32 diff)
    {
        if (!me->isInCombat())
        {
            if (RandChat_Timer <= diff)
            {
                DoScriptText(RAND(RANDOM_YELL_1, RANDOM_YELL_2, RANDOM_YELL_3, RANDOM_YELL_4, RANDOM_YELL_5, RANDOM_YELL_6), me);
                RandChat_Timer = 90000;
            }
            else
                RandChat_Timer -= diff;
        }

        // real aggro
        if (AttackBegin_Timer)
        {
            if (AttackBegin_Timer <= diff)
            {
                if (instance)
                    instance->SetData(DATA_MAGTHERIDON_EVENT, IN_PROGRESS); // activate cubes

                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_OOC_NOT_ATTACKABLE);
                me->clearUnitState(UNIT_STAT_STUNNED);

                DoZoneInCombat();

                DoScriptText(SAY_AGGRO, me);
                AttackBegin_Timer = 0; // don't repeat
            }
            else
                AttackBegin_Timer -= diff;
        }

        if (!UpdateVictim())
            return;

        if (NeedCheckCube)
            NeedCheckCubeStatus();

        if (me->HasAura(SPELL_SHADOW_CAGE, 0))
            return;

        if (Berserk_Timer <= diff)
        {
            me->CastSpell(me, SPELL_BERSERK, true);
            DoScriptText(EMOTE_BERSERK, me);
            Berserk_Timer = 60000;
        }
        else
            Berserk_Timer -= diff;

        if (Cleave_Timer <= diff)
        {
            DoCast(me->getVictim(),SPELL_CLEAVE);
            Cleave_Timer = 10000;
        }
        else
            Cleave_Timer -= diff;

        if (BlastNova_Timer <= diff)
        {
            // to avoid earthquake interruption
            if (!me->hasUnitState(UNIT_STAT_STUNNED))
            {
                DoScriptText(EMOTE_BLASTNOVA, me);
                DoCast(me, SPELL_BLASTNOVA);
                BlastNova_Timer = 60000;
            }
        }
        else
            BlastNova_Timer -= diff;

        if (Quake_Timer <= diff)
        {
            // to avoid blastnova interruption
            if (!me->IsNonMeleeSpellCasted(false))
            {
                isQuaking = true;
                DoCast(SPELL_QUAKE_TRIGGER);
                QuakeKnockback_Timer = 0;
                Quake_Timer = 50000;
            }
        }
        else
            Quake_Timer -= diff;

        // weird work-around for quake trigger NOT working on players
        if (QuakeKnockback_Timer <= diff && isQuaking)
        {
            Map* pMap = me->GetMap();
            Map::PlayerList const& players = pMap->GetPlayers();
            if (!players.isEmpty())
            {
                for (Map::PlayerList::const_iterator itr = players.begin(); itr != players.end(); ++itr)
                {
                    if (Player* player = itr->getSource())
                        player->CastSpell(player, SPELL_QUAKE_KNOCKBACK, true);
                }
            }

            if (knockCount < 6)
            {
                ++knockCount;
                QuakeKnockback_Timer = 1000;
            }
            else
            {
                knockCount = 0;
                isQuaking = false;
            }
        }
        else
            QuakeKnockback_Timer -= diff;

        if (Blaze_Timer <= diff)
        {
            // don't cast during earthquake or blastnova
            if (!me->hasUnitState(UNIT_STAT_STUNNED) && !me->IsNonMeleeSpellCasted(false))
            {
                // Blaze should be cast to target 7 (TARGET_UNIT_AREA_ENTRY_SRC) but there is no core support for this
                // instead, randomize area between X and Y confines slightly before cube pads to prevent fire on cube
                float x = irand(-49, 7);
                float y = irand(-30, 24);

                if (Creature* trigger = me->SummonCreature(MOB_MAGTHERIDON_TRIGGER, x, y, me->GetPositionZ(), 0, TEMPSUMMON_MANUAL_DESPAWN, 0))
                    me->CastSpell(trigger, SPELL_BLAZE_TARGET, true);

                Blaze_Timer = urand(20000, 40000);
            }
        }
        else
            Blaze_Timer -= diff;

        if (!Phase3 && HealthBelowPct(30)
            && !me->IsNonMeleeSpellCasted(false) // blast nova
            && !me->hasUnitState(UNIT_STAT_STUNNED)) // shadow cage and earthquake
        {
            Phase3 = true;
            DoScriptText(SAY_CHAMBER_DESTROY, me);
            DoCast(me, SPELL_CAMERA_SHAKE, true);
            DoCast(me, SPELL_DEBRIS_KNOCKDOWN, true);

            if (instance)
                instance->SetData(DATA_COLLAPSE, true);
        }

        if (Phase3)
        {
            if (Debris_Timer <= diff)
            {
                if (Unit *pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0))
                    if (Creature* trigger = me->SummonCreature(MOB_MAGTHERIDON_TRIGGER, pTarget->GetPositionX(), pTarget->GetPositionY(), pTarget->GetPositionZ(), 0, TEMPSUMMON_MANUAL_DESPAWN, 0))
                        ((mob_magtheridon_triggerAI*)trigger->AI())->DoDebris();

                Debris_Timer = 10000;
            }
            else
                Debris_Timer -= diff;
        }

        if (me->GetPositionX() < -72.0f) // past doorway
            EnterEvadeMode();

        DoMeleeAttackIfReady();
    }
};

struct mob_hellfire_channelerAI : public ScriptedAI
{
    mob_hellfire_channelerAI(Creature *c) : ScriptedAI(c)
    {
        instance = me->GetInstanceScript();
    }

    ScriptedInstance* instance;

    uint32 ShadowBoltVolley_Timer;
    uint32 DarkMending_Timer;
    uint32 Fear_Timer;
    uint32 Infernal_Timer;

    uint32 Check_Timer;

    Creature* Magtheridon;

    void Reset()
    {
        me->setFaction(14); // same as mag -- prevent attacking each other

        ShadowBoltVolley_Timer = urand(8000, 10000);
        DarkMending_Timer = 10000;
        Fear_Timer = urand(15000, 20000);
        Infernal_Timer = urand(10000, 50000);

        Check_Timer = 5000;

        Magtheridon = NULL;

        if (instance)
        {
            instance->SetData(DATA_CHANNELER_EVENT, NOT_STARTED);

            Magtheridon = me->GetCreature(*me, instance->GetData64(DATA_MAGTHERIDON));

            if (me->GetDistance2d(me->GetHomePosition().GetPositionX(), me->GetHomePosition().GetPositionY()) < 2)
                DoCast(Magtheridon ? Magtheridon : me, SPELL_SHADOW_GRASP_C);
        }

        me->RemoveAurasDueToSpell(SPELL_SOUL_TRANSFER);
    }

    void JustReachedHome()
    {
        DoCast(Magtheridon ? Magtheridon : me, SPELL_SHADOW_GRASP_C);
    }

    void EnterCombat(Unit *who)
    {
        if (instance)
            instance->SetData(DATA_CHANNELER_EVENT, IN_PROGRESS);

        me->InterruptNonMeleeSpells(false);
        me->CallForHelp(VISIBLE_RANGE);
        DoZoneInCombat();
    }

    void JustSummoned(Creature *summon)
    {
        summon->AI()->AttackStart(me->getVictim());
    }

    void MoveInLineOfSight(Unit*) {}

    void DamageTaken(Unit*, uint32 &damage)
    {
        if (damage >= me->GetHealth())
            DoCast(me, SPELL_SOUL_TRANSFER, true);
    }

    void JustDied(Unit* killer)
    {
        if (instance)
            instance->SetData(DATA_CHANNELER_EVENT, SPECIAL);

        me->RemoveFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        if (ShadowBoltVolley_Timer <= diff)
        {
            DoCast(me, SPELL_SHADOW_BOLT_VOLLEY);
            ShadowBoltVolley_Timer = urand(10000, 20000);
        }
        else
            ShadowBoltVolley_Timer -= diff;

        if (DarkMending_Timer <= diff)
        {
            if (HealthBelowPct(50))
                DoCast(me, SPELL_DARK_MENDING);

            DarkMending_Timer = urand(10000, 20000);
        }
        else
            DarkMending_Timer -= diff;

        if (Fear_Timer <= diff)
        {
            if (Unit *pTarget = SelectUnit(SELECT_TARGET_RANDOM, 1))
                DoCast(pTarget, SPELL_FEAR);

            Fear_Timer = urand(25000, 40000);
        }
        else
            Fear_Timer -= diff;

        if (Infernal_Timer)
        {
            if (Infernal_Timer <= diff)
            {
                if (Unit *pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0))
                    DoCast(pTarget, SPELL_BURNING_ABYSSAL, true);

                Infernal_Timer = 0;
            }
            else
                Infernal_Timer -= diff;
        }

        DoMeleeAttackIfReady();
    }
};

// Manticron Cube
bool GOHello_go_Manticron_Cube(Player* player, GameObject* _GO)
{
    ScriptedInstance* instance =_GO->GetInstanceScript();

    if (!instance)
        return true;

    if (instance->GetData(DATA_MAGTHERIDON_EVENT) != IN_PROGRESS)
        return true;

    Creature *Magtheridon = Unit::GetCreature(*_GO, instance->GetData64(DATA_MAGTHERIDON));

    if (!Magtheridon || !Magtheridon->isAlive())
        return true;

    // if exhausted or already channeling return
    if (player->HasAura(SPELL_MIND_EXHAUSTION, 0) || player->HasAura(SPELL_SHADOW_GRASP, 1))
        return true;

    _GO->SetOwnerGUID(player->GetGUID());

    player->AttackStop(); // prevents bows/guns from firing + causing us to cancel immediately
    player->InterruptNonMeleeSpells(false);
    player->CastSpell(player, SPELL_SHADOW_GRASP, false);
    if (Creature *trigger = player->SummonCreature(MOB_HELLFIRE_TRIGGER, _GO->GetPositionX(), _GO->GetPositionY(), _GO->GetPositionZ(), _GO->GetOrientation(), TEMPSUMMON_TIMED_DESPAWN, 12000))
        trigger->CastSpell(Magtheridon, SPELL_SHADOW_GRASP_VISUAL, true);

    ((boss_magtheridonAI*)Magtheridon->AI())->SetClicker(_GO->GetGUID(), player->GetGUID());
    return true;
}



CreatureAI* GetAI_boss_magtheridon(Creature* creature)
{
    return new boss_magtheridonAI(creature);
}

CreatureAI* GetAI_mob_hellfire_channeler(Creature* creature)
{
    return new mob_hellfire_channelerAI(creature);
}

CreatureAI* GetAI_mob_abyssalAI(Creature* creature)
{
    return new mob_abyssalAI(creature);
}

CreatureAI* GetAI_mob_magtheridon_triggerAI(Creature *_Creature)
{
    return new mob_magtheridon_triggerAI(_Creature);
}

void AddSC_boss_magtheridon()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "boss_magtheridon";
    newscript->GetAI = &GetAI_boss_magtheridon;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_hellfire_channeler";
    newscript->GetAI = &GetAI_mob_hellfire_channeler;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "go_manticron_cube";
    newscript->pGOHello = &GOHello_go_Manticron_Cube;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_abyssal";
    newscript->GetAI = &GetAI_mob_abyssalAI;
    newscript->RegisterSelf();

    newscript = new Script();
    newscript->Name = "mob_magtheridon_trigger";
    newscript->GetAI = &GetAI_mob_magtheridon_triggerAI;
    newscript->RegisterSelf();
}

