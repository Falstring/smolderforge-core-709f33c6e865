 /*
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
SDName: Boss_Astromancer
SD%Complete: 80
SDComment:
SDCategory: Tempest Keep, The Eye
EndScriptData */

#include "ScriptPCH.h"
#include "the_eye.h"

enum Yells
{
    SAY_AGGRO                           = -1550007,
    SAY_SUMMON1                         = -1550008,
    SAY_SUMMON2                         = -1550009,
    SAY_KILL1                           = -1550010,
    SAY_KILL2                           = -1550011,
    SAY_KILL3                           = -1550012,
    SAY_DEATH                           = -1550013,
    SAY_VOIDA                           = -1550014,
    SAY_VOIDB                           = -1550015
};

enum Spells
{
    SPELL_ARCANE_MISSILES               = 33031,
    SPELL_MARK_OF_THE_ASTROMANCER       = 33045,
    SPELL_WRATH_OF_THE_ASTROMANCER      = 42783,
    MARK_OF_SOLARIAN                    = 33023,
    SPELL_BLINDING_LIGHT                = 33009,
    SPELL_FEAR                          = 29321,
    SPELL_VOID_BOLT                     = 39329,

    SPELL_SPOTLIGHT                     = 25824,

    SPELL_GREATER_HEAL                  = 41378,
    SPELL_HOLY_SMITE                    = 31740,
    SPELL_SILENCE                       = 37160
};

enum Creatures
{
    NPC_SOLARIUM_AGENT                  = 18925,
    NPC_SOLARIUM_PRIEST                 = 18806,
    NPC_ASTROMANCER_SOLARIAN_SPOTLIGHT  = 18928
};

enum DisplayIds
{
    DISPLAY_ID_HUMAN                    = 18239,
    DISPLAY_ID_VOIDWALKER               = 18988
};

#define MIN_RANGE_FOR_DOT_JUMP      20.0f

static float portalRadius[3] = { 12.6f, 26.0f, 17.005f }; // Small  Large  Z-Position
static float centerPos[4] = { 432.909f, -373.424f, 17.9608f, 1.06421f };


struct boss_high_astromancer_solarianAI : public ScriptedAI
{
    boss_high_astromancer_solarianAI(Creature *c) : ScriptedAI(c), Summons(me)
    {
        instance = c->GetInstanceScript();

        defaultarmor = me->GetArmor();
        defaultsize = me->GetObjectScale();
    }

    ScriptedInstance *instance;
    SummonList Summons;

    uint8 Phase;

    uint32 ArcaneMissiles_Timer;
    uint32 MarkOfTheAstromancer_Timer;
    uint32 BlindingLight_Timer;
    uint32 Fear_Timer;
    uint32 VoidBolt_Timer;
    uint32 Phase1_Timer;
    uint32 Phase2_Timer;
    uint32 Phase3_Timer;
    uint32 AppearDelay_Timer;
    uint32 MarkOfTheSolarian_Timer;
    uint32 Jump_Timer;
    uint32 defaultarmor;
    uint32 Wrath_Timer;

    float defaultsize;
    float Portals[3][3];

    bool AppearDelay;
    bool BlindingLight;

    void Reset()
    {
        ArcaneMissiles_Timer = 2000;
        MarkOfTheAstromancer_Timer = 15000;
        BlindingLight_Timer = 41000;
        Fear_Timer = 20000;
        VoidBolt_Timer = 10000;
        Phase1_Timer = 50000;
        Phase2_Timer = 10000;
        Phase3_Timer = 15000;
        AppearDelay_Timer = 2000;
        BlindingLight = false;
        AppearDelay = false;
        MarkOfTheSolarian_Timer=45000;
        Jump_Timer=8000;
        Wrath_Timer = 20000+rand()%5000;//twice in phase one
        Phase = 1;

        if (instance)
            instance->SetData(DATA_HIGHASTROMANCERSOLARIANEVENT, NOT_STARTED);

        me->SetArmor(defaultarmor);
        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        me->SetVisibility(VISIBILITY_ON);
        me->SetObjectScale(defaultsize);
        me->SetDisplayId(DISPLAY_ID_HUMAN);

        Summons.DespawnAll();
    }

    void StartEvent()
    {
        DoScriptText(SAY_AGGRO, me);

        if (instance)
            instance->SetData(DATA_HIGHASTROMANCERSOLARIANEVENT, IN_PROGRESS);
    }

    void KilledUnit(Unit *victim)
    {
        DoScriptText(RAND(SAY_KILL1, SAY_KILL2, SAY_KILL2), me);
    }

    void JustDied(Unit *victim)
    {
        me->SetObjectScale(defaultsize);
        me->SetDisplayId(DISPLAY_ID_HUMAN);
        DoScriptText(SAY_DEATH, me);

        if (instance)
            instance->SetData(DATA_HIGHASTROMANCERSOLARIANEVENT, DONE);
    }

    void EnterCombat(Unit *who)
    {
        StartEvent();
    }

    void SummonMinion(uint32 entry, float x, float y, float z)
    {
        Creature* Summoned = me->SummonCreature(entry, x, y, z, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 5000);
        if (Summoned)
        {
            if (Unit *pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0))
                Summoned->AI()->AttackStart(pTarget);

            Summons.Summon(Summoned);
        }
    }

    float Portal_X(float radius)
    {
        if ((rand()%2) == 1)
            radius = -radius;

        return (radius * (float)(rand()%100)/100.0f + centerPos[0]);
    }

    float Portal_Y(float x, float radius)
    {
        float z;

        switch (rand()%2)
        {
            case 0: z = 1; break;
            case 1: z = -1; break;
        }
        return (z*sqrt(radius*radius - (x - centerPos[0])*(x - centerPos[0])) + centerPos[1]);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        if (AppearDelay)
        {
            me->StopMoving();
            me->AttackStop();
            if (AppearDelay_Timer <= diff)
            {
                AppearDelay = false;
                if (Phase == 2)
                {
                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                    me->SetVisibility(VISIBILITY_OFF);
                }
                AppearDelay_Timer = 2000;
            } else AppearDelay_Timer -= diff;
        }

        if (Phase == 1)
        {
            if (BlindingLight_Timer <= diff){
                BlindingLight = true;
                BlindingLight_Timer = 45000;
            } else BlindingLight_Timer -= diff;

            if (Wrath_Timer <= diff)
            {
                me->InterruptNonMeleeSpells(false);
                DoCast(SelectTarget(SELECT_TARGET_RANDOM, 1, 100, true), SPELL_WRATH_OF_THE_ASTROMANCER, true);
                Wrath_Timer = 20000+rand()%5000;
            } else Wrath_Timer -= diff;

            if (ArcaneMissiles_Timer <= diff)
            {
                if (BlindingLight)
                {
                    DoCast(me->getVictim(), SPELL_BLINDING_LIGHT);
                    BlindingLight = false;
                }
                else
                {
                    if (Unit *pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0))
                    {
                        if (!me->HasInArc(2.5f, pTarget))
                            pTarget = me->getVictim();

                        if (pTarget)
                            DoCast(pTarget, SPELL_ARCANE_MISSILES);
                    }
                }
                ArcaneMissiles_Timer = 3000;
            } else ArcaneMissiles_Timer -= diff;

            if (MarkOfTheSolarian_Timer <= diff)
            {
                DoCast(me->getVictim(), MARK_OF_SOLARIAN);
                MarkOfTheSolarian_Timer = 45000;
            } else MarkOfTheSolarian_Timer -= diff;

            if (MarkOfTheAstromancer_Timer <= diff) //A debuff that lasts for 5 seconds, cast several times each phase on a random raid member, but not the main tank
            {
                Unit *pTarget = SelectTarget(SELECT_TARGET_RANDOM, 1, 100, true);
                if (pTarget)
                    DoCast(pTarget, SPELL_MARK_OF_THE_ASTROMANCER);
                else DoCast(me->getVictim(), SPELL_MARK_OF_THE_ASTROMANCER);
                MarkOfTheAstromancer_Timer = 15000;
            } else MarkOfTheAstromancer_Timer -= diff;

            //Phase1_Timer
            if (Phase1_Timer <= diff)
            {
                Phase = 2;
                Phase1_Timer = 50000;
                //After these 50 seconds she portals to the middle of the room and disappears, leaving 3 light portals behind.
                me->GetMotionMaster()->Clear();
                me->GetMap()->CreatureRelocation(me, centerPos[0], centerPos[1], centerPos[2], centerPos[3]);
                for (int i = 0; i <= 2; ++i)
                {
                    if (!i)
                    {
                        Portals[i][0] = Portal_X(portalRadius[0]);
                        Portals[i][1] = Portal_Y(Portals[i][0], portalRadius[0]);
                        Portals[i][2] = centerPos[2];
                    }
                    else
                    {
                        Portals[i][0] = Portal_X(portalRadius[1]);
                        Portals[i][1] = Portal_Y(Portals[i][0], portalRadius[1]);
                        Portals[i][2] = centerPos[2];
                    }
                }
                if ((abs(Portals[2][0] - Portals[1][0]) < 7) && (abs(Portals[2][1] - Portals[1][1]) < 7))
                {
                    int i=1;
                    if (abs(portalRadius[0] + 26.0f - Portals[2][0]) < 7)
                        i = -1;
                    Portals[2][0] = Portals[2][0]+7*i;
                    Portals[2][1] = Portal_Y(Portals[2][0], portalRadius[1]);
                }
                for (int i = 0; i <= 2; i++)
                {
                    Creature* Summoned = me->SummonCreature(NPC_ASTROMANCER_SOLARIAN_SPOTLIGHT, Portals[i][0], Portals[i][1], Portals[i][2], 0.0f, TEMPSUMMON_TIMED_DESPAWN, Phase2_Timer+Phase3_Timer+AppearDelay_Timer+1700);
                    if (Summoned)
                    {
                        Summoned->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                        Summoned->CastSpell(Summoned, SPELL_SPOTLIGHT, false);
                    }
                }
                AppearDelay = true;
            } else Phase1_Timer-=diff;
        }
        else if (Phase == 2)
        {
            //10 seconds after Solarian disappears, 12 mobs spawn out of the three portals.
            me->AttackStop();
            me->StopMoving();
            if (Phase2_Timer <= diff)
            {
                Phase = 3;
                for (int i = 0; i <= 2; i++)
                    for (int j = 1; j <= 4; j++)
                        SummonMinion(NPC_SOLARIUM_AGENT, Portals[i][0], Portals[i][1], Portals[i][2]);

                DoScriptText(SAY_SUMMON1, me);
                Phase2_Timer = 10000;
            } else Phase2_Timer -= diff;
        }
        else if (Phase == 3)
        {
            me->AttackStop();
            me->StopMoving();

            //Check Phase3_Timer
            if (Phase3_Timer <= diff)
            {
                Phase = 1;

                //15 seconds later Solarian reappears out of one of the 3 portals. Simultaneously, 2 healers appear in the two other portals.
                int i = rand()%3;
                me->GetMotionMaster()->Clear();
                me->GetMap()->CreatureRelocation(me, Portals[i][0], Portals[i][1], Portals[i][2], centerPos[4]);

                for (int j=0; j<=2; j++)
                    if (j != i)
                        SummonMinion(NPC_SOLARIUM_PRIEST, Portals[j][0], Portals[j][1], Portals[j][2]);

                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                me->SetVisibility(VISIBILITY_ON);

                DoScriptText(SAY_SUMMON2, me);
                AppearDelay = true;
                Phase3_Timer = 15000;
            } else Phase3_Timer -= diff;
        }
        else if (Phase == 4)
        {
            //Fear_Timer
            if (Fear_Timer <= diff)
            {
                DoCast(me->getVictim(), SPELL_FEAR);
                Fear_Timer = 20000;
            } else Fear_Timer -= diff;

            //VoidBolt_Timer
            if (VoidBolt_Timer <= diff)
            {
                DoCast(me->getVictim(), SPELL_VOID_BOLT);
                VoidBolt_Timer = 10000;
            } else VoidBolt_Timer -= diff;
        }

        //When Solarian reaches 20% she will transform into a huge void walker.
        if (Phase != 4 && ((me->GetHealth()*100 / me->GetMaxHealth())<20))
        {
            Phase = 4;

            //To make sure she wont be invisible or not selecatble
            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            me->SetVisibility(VISIBILITY_ON);
            DoScriptText(SAY_VOIDA, me);
            DoScriptText(SAY_VOIDB, me);
            me->SetArmor(31000);
            me->SetDisplayId(DISPLAY_ID_VOIDWALKER);
            me->SetObjectScale(defaultsize * 2.5f);
        }

        DoMeleeAttackIfReady();
    }
};

struct mob_solarium_priestAI : public ScriptedAI
{
    mob_solarium_priestAI(Creature *c) : ScriptedAI(c)
    {
        instance = c->GetInstanceScript();
    }

    ScriptedInstance *instance;

    uint32 healTimer;
    uint32 holysmiteTimer;
    uint32 aoesilenceTimer;

    void Reset()
    {
        healTimer = 9000;
        holysmiteTimer = 1;
        aoesilenceTimer = 15000;
    }

    void EnterCombat(Unit *who)
    {
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        if (healTimer <= diff)
        {
            Unit *pTarget = NULL;

            switch (rand()%2)
            {
                case 0:
                    if (instance)
                        pTarget = Unit::GetUnit((*me), instance->GetData64(DATA_ASTROMANCER));
                    break;
                case 1:
                    pTarget = me;
                    break;
            }

            if (pTarget)
            {
                DoCast(pTarget, SPELL_GREATER_HEAL);
                healTimer = 9000;
            }
        } else healTimer -= diff;

        if (holysmiteTimer <= diff)
        {
            DoCast(me->getVictim(), SPELL_HOLY_SMITE);
            holysmiteTimer = 4000;
        } else holysmiteTimer -= diff;

        if (aoesilenceTimer <= diff)
        {
            DoCast(me->getVictim(), SPELL_SILENCE);
            aoesilenceTimer = 13000;
        } else aoesilenceTimer -= diff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_solarium_priest(Creature* creature)
{
    return new mob_solarium_priestAI (creature);
}

CreatureAI* GetAI_boss_high_astromancer_solarian(Creature* creature)
{
    return new boss_high_astromancer_solarianAI (creature);
}

void AddSC_boss_high_astromancer_solarian()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "boss_high_astromancer_solarian";
    newscript->GetAI = &GetAI_boss_high_astromancer_solarian;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_solarium_priest";
    newscript->GetAI = &GetAI_mob_solarium_priest;
    newscript->RegisterSelf();
}
