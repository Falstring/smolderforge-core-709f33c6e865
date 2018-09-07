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
SDName: boss_Kaelthas
SD%Complete: 60
SDComment: Mind Control, Reset Event if Weapons despawn/reset
SDCategory: Tempest Keep, The Eye
EndScriptData */

#include "ScriptPCH.h"
#include "the_eye.h"
#include "WorldPacket.h"

 //kael'thas Speech
#define SAY_INTRO                           -1550016
#define SAY_INTRO_CAPERNIAN                 -1550017
#define SAY_INTRO_TELONICUS                 -1550018
#define SAY_INTRO_THALADRED                 -1550019
#define SAY_INTRO_SANGUINAR                 -1550020
#define SAY_PHASE2_WEAPON                   -1550021
#define SAY_PHASE3_ADVANCE                  -1550022
#define SAY_PHASE4_INTRO2                   -1550023
#define SAY_PHASE5_NUTS                     -1550024
#define SAY_SLAY1                           -1550025
#define SAY_SLAY2                           -1550026
#define SAY_SLAY3                           -1550027
#define SAY_MINDCONTROL1                    -1550028
#define SAY_MINDCONTROL2                    -1550029
#define SAY_GRAVITYLAPSE1                   -1550030
#define SAY_GRAVITYLAPSE2                   -1550031
#define SAY_SUMMON_PHOENIX1                 -1550032
#define SAY_SUMMON_PHOENIX2                 -1550033
#define SAY_DEATH                           -1550034

//Thaladred the Darkener speech
#define SAY_THALADRED_AGGRO                 -1550035
#define SAY_THALADRED_DEATH                 -1550036
#define EMOTE_THALADRED_GAZE                -1550037

//Lord Sanguinar speech
#define SAY_SANGUINAR_AGGRO                 -1550038
#define SAY_SANGUINAR_DEATH                 -1550039

//Grand Astromancer Capernian speech
#define SAY_CAPERNIAN_AGGRO                 -1550040
#define SAY_CAPERNIAN_DEATH                 -1550041

//Master Engineer Telonicus speech
#define SAY_TELONICUS_AGGRO                 -1550042
#define SAY_TELONICUS_DEATH                 -1550043

//Phase 2 spells (Not used)
#define SPELL_SUMMON_WEAPONS              36976
#define SPELL_SUMMON_WEAPONA              36958
#define SPELL_SUMMON_WEAPONB              36959
#define SPELL_SUMMON_WEAPONC              36960
#define SPELL_SUMMON_WEAPOND              36961
#define SPELL_SUMMON_WEAPONE              36962
#define SPELL_SUMMON_WEAPONF              36963
#define SPELL_SUMMON_WEAPONG              36964
#define SPELL_WEAPON_SPAWN                41236

//Phase 3 Advisor max health
#define SPELL_RES_VISUAL                  24341
enum AdvisorMaxHealth
{
    HP_THALADRED        = 273168,
    HP_SANGUINAR        = 273168,
    HP_CAPERNIAN        = 191196,
    HP_TELONICUS        = 273168
};

//Phase 4 spells
#define SPELL_FIREBALL                    36805
#define SPELL_PYROBLAST                   36819
#define SPELL_FLAME_STRIKE                36735
#define SPELL_FLAME_STRIKE_VIS            36730
#define SPELL_FLAME_STRIKE_DMG            36731
#define SPELL_ARCANE_DISRUPTION           36834
#define SPELL_SHOCK_BARRIER               36815
#define SPELL_SUMMON_PHOENIX              36723
#define SPELL_MIND_CONTROL                32830

//Phase 5 animation
#define GRAVITY_X                          794.4f//793.6f
#define GRAVITY_Y                          -0.42f//-0.045f
#define GRAVITY_Z                          48.66f

//Phase 5 blue beam
#define SPELL_PURPLE_BEAM_RIGHT           36090
#define SPELL_PURPLE_BEAM_LEFT            36089

//Phase 5 spells
#define SPELL_NETHERBEAM_GLOW1            36364
#define SPELL_NETHERBEAM_GLOW2            36370
#define SPELL_NETHERBEAM_GLOW3            36371
#define SPELL_EXPLODE                     36092
#define SPELL_GAINING_POWER               36091
#define SPELL_FULLPOWER                   36187
#define SPELL_KNOCKBACK                   11027
#define SPELL_GRAVITY_LAPSE               34480
#define SPELL_GRAVITY_LAPSE_AURA          39432
#define SPELL_NETHER_BEAM                 35873
#define SPELL_EXPLODE_SHAKE1              36373
#define SPELL_EXPLODE_SHAKE2              36375
#define SPELL_EXPLODE_SHAKE3              36376
#define SPELL_EXPLODE_SHAKE4              36354
#define SPELL_GAIN_POWER1                 36196
#define SPELL_GAIN_POWER2                 36197
#define SPELL_GAIN_POWER3                 36198

//Thaladred the Darkener spells
#define EQUIP_DEVASTATION                 41560
#define DEVASTATION_EQUIP_INFO            33489154
#define SPELL_PSYCHIC_BLOW                10689
#define SPELL_SILENCE                     30225
#define SPELL_REND                        36965

//Lord Sanguinar spells
#define EQUIP_RED_MACE                    40676
#define RED_MACE_EQUIP_INFO               352453634
#define EQUIP_PHASESHIFT_BULWARK          40867
#define PHASESHIFT_BULWARK_EQUIP_INFO     33490436
#define PHASESHIFT_BULWARK_EQUIP_INFO2    1038
#define SPELL_BELLOWING_ROAR              40636

//Grand Astromancer Capernian spells
#define EQUIP_SOD                         41895
#define SOD_EQUIP_INFO                    33490946
#define CAPERNIAN_DISTANCE                20                //she casts away from the target
#define SPELL_CAPERNIAN_FIREBALL          36971
#define SPELL_CONFLAGRATION               37018
#define SPELL_ARCANE_EXPLOSION            36970

//Master Engineer Telonicus spells
#define EQUIP_INFINITY_BLADE              41872
#define INFINITY_BLADE_EQUIP_INFO         33492738
#define SPELL_BOMB                        37036
#define SPELL_REMOTE_TOY                  37027

//Nether Vapor spell
#define SPELL_NETHER_VAPOR                35859

//Phoenix spell
#define SPELL_BURN                        36720
#define SPELL_EMBER_BLAST                 34341
#define SPELL_REBIRTH                     41587

//Creature IDs
#define PHOENIX                           21362
#define PHOENIX_EGG                       21364
#define KAEL_TRIGGER                      85000
#define KAEL_TRIGGER2                     85001

//Phoenix egg and phoenix model
#define PHOENIX_MODEL           19682
#define PHOENIX_EGG_MODEL       20245

//weapon id + position
float KaelthasWeapons[7][5] =
{
    {21270, 794.38f, 15, 48.72f, 2.9f},                       //[Cosmic Infuser]
    {21269, 785.47f, 12.12f, 48.72f, 3.14f},                   //[Devastation]
    {21271, 781.25f, 4.39f, 48.72f, 3.14f},                    //[Infinity Blade]
    {21273, 777.38f, -0.81f, 48.72f, 3.06f},                   //[Phaseshift Bulwark]
    {21274, 781.48f, -6.08f, 48.72f, 3.9f},                    //[Staff of Disintegration]
    {21272, 785.42f, -13.59f, 48.72f, 3.4f},                   //[Warp Slicer]
    {21268, 793.06f, -16.61f, 48.72f, 3.10f}                    //[Netherstrand Longbow]
};

#define TIME_PHASE_2_3      120000
#define TIME_PHASE_3_4      120000

#define KAEL_VISIBLE_RANGE  50.0f
#define ROOM_BASE_Z 49.0f

//Base AI for Advisors
struct advisorbase_ai : public ScriptedAI
{
    ScriptedInstance* instance;
    bool FakeDeath;
    uint32 DelayRes_Timer;
    uint64 DelayRes_Target;

    advisorbase_ai(Creature *c) : ScriptedAI(c)
    {
        instance = c->GetInstanceScript();
    }

    void MoveInLineOfSight(Unit *who)
    {
        if (!who || FakeDeath || me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
            return;

        ScriptedAI::MoveInLineOfSight(who);
    }

    void AttackStart(Unit* who)
    {
        if (!who || FakeDeath || me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
            return;

        ScriptedAI::AttackStart(who);
    }

    void Reset()
    {
        me->SetNoCallAssistance(true);
        FakeDeath = false;
        DelayRes_Timer = 0;
        DelayRes_Target = 0;

        me->SetUInt32Value(UNIT_FIELD_BYTES_1, 0);
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);

        //reset encounter
        if (instance && (instance->GetData(DATA_KAELTHASEVENT) == 1 || instance->GetData(DATA_KAELTHASEVENT) == 3))
        {
            Creature *Kaelthas = NULL;
            Kaelthas = (Creature*)(Unit::GetUnit((*me), instance->GetData64(DATA_KAELTHAS)));

            if (Kaelthas)
                Kaelthas->AI()->EnterEvadeMode();
        }

        UpdateMaxHealth(false);
    }

    void Revive(Unit *pTarget)
    {
        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        me->SetHealth(me->GetMaxHealth());
        me->SetUInt32Value(UNIT_FIELD_BYTES_1, 0);
        DoCast(me, SPELL_RES_VISUAL, true);
        DelayRes_Timer = 2000;
    }

    void UpdateMaxHealth(bool twice)
    {
        if (me->GetGUID() == instance->GetData64(DATA_THALADREDTHEDARKENER))
            me->SetMaxHealth(twice ? HP_THALADRED*2 : HP_THALADRED);
        if (me->GetGUID() == instance->GetData64(DATA_LORDSANGUINAR))
            me->SetMaxHealth(twice ? HP_SANGUINAR*2 : HP_SANGUINAR);
        if (me->GetGUID() == instance->GetData64(DATA_GRANDASTROMANCERCAPERNIAN))
            me->SetMaxHealth(twice ? HP_CAPERNIAN*2 : HP_CAPERNIAN);
        if (me->GetGUID() == instance->GetData64(DATA_MASTERENGINEERTELONICUS))
            me->SetMaxHealth(twice ? HP_TELONICUS*2 : HP_TELONICUS);
    }

    void DamageTaken(Unit* pKiller, uint32 &damage)
    {
        if (damage < me->GetHealth())
            return;

        //Prevent glitch if in fake death
        if (FakeDeath)
        {
            damage = 0;
            return;
        }
        //Don't really die in phase 1 & 3, only die after that
        if (instance && instance->GetData(DATA_KAELTHASEVENT) != 0)
        {
            //prevent death
            damage = 0;
            FakeDeath = true;

            me->InterruptNonMeleeSpells(false);
            me->SetHealth(0);
            me->ClearComboPointHolders();
            me->RemoveAllAurasOnDeath();
            me->ModifyAuraState(AURA_STATE_HEALTHLESS_20_PERCENT, false);
            me->ModifyAuraState(AURA_STATE_HEALTHLESS_35_PERCENT, false);
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            me->ClearAllReactives();
            me->SetUInt64Value(UNIT_FIELD_TARGET, 0);
            me->GetMotionMaster()->Clear();
            me->GetMotionMaster()->MoveIdle();
            me->SetStandState(UNIT_STAND_STATE_DEAD);

            UpdateMaxHealth(true);

            if (instance->GetData(DATA_KAELTHASEVENT) == 3)
                JustDied(pKiller);
        }
    }

    void UpdateAI(const uint32 diff)
    {
        if (DelayRes_Timer)
        {
            if (DelayRes_Timer <= diff)
            {
                DelayRes_Timer = 0;
                FakeDeath = false;

                Unit *pTarget = Unit::GetUnit((*me), DelayRes_Target);
                if (!pTarget)
                    pTarget = me->getVictim();
                DoResetThreat();
                if (!pTarget)
                    return;
                AttackStart(pTarget);
                me->GetMotionMaster()->Clear();
                me->GetMotionMaster()->MoveChase(pTarget);
                me->AddThreat(pTarget, 0.0f);
            } else DelayRes_Timer -= diff;
        }
    }
};

//Kael'thas AI
struct boss_kaelthasAI : public ScriptedAI
{
    boss_kaelthasAI(Creature *c) : ScriptedAI(c), summons(me)
    {
        instance = c->GetInstanceScript();
        AdvisorGuid[0] = 0;
        AdvisorGuid[1] = 0;
        AdvisorGuid[2] = 0;
        AdvisorGuid[3] = 0;
    }

    ScriptedInstance* instance;

    std::list<uint64> Phoenix;

    uint32 Fireball_Timer;
    uint32 ArcaneDisruption_Timer;
    uint32 Phoenix_Timer;
    uint32 ShockBarrier_Timer;
    uint32 GravityLapse_Timer;
    uint32 GravityLapse_Phase;
    uint32 NetherBeam_Timer;
    uint32 NetherVapor_Timer;
    uint32 FlameStrike_Timer;
    uint32 MindControl_Timer;
    uint32 Check_Timer;
    uint32 Phase;
    uint32 PhaseSubphase;                                   //generic
    uint32 Phase_Timer;                                     //generic timer
    uint32 PyrosCasted;

    uint32 Step;
    Creature* beam[6];

    bool InGravityLapse;
    bool IsCastingFireball;
    bool ChainPyros;

    uint64 AdvisorGuid[4];
    uint64 WeaponGuid[7];
    SummonList summons;

    void DeleteLegs()
    {
        InstanceMap::PlayerList const &playerliste = ((InstanceMap*)me->GetMap())->GetPlayers();
        InstanceMap::PlayerList::const_iterator it;

        Map::PlayerList const &PlayerList = ((InstanceMap*)me->GetMap())->GetPlayers();
        for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
        {
            Player* i_pl = i->getSource();
            i_pl->DestroyItemCount(30312, 1, true);
            i_pl->DestroyItemCount(30311, 1, true);
            i_pl->DestroyItemCount(30317, 1, true);
            i_pl->DestroyItemCount(30316, 1, true);
            i_pl->DestroyItemCount(30313, 1, true);
            i_pl->DestroyItemCount(30314, 1, true);
            i_pl->DestroyItemCount(30318, 1, true);
            i_pl->DestroyItemCount(30319, 1, true);
            i_pl->DestroyItemCount(30320, 1, true);
        }
    }

    void Reset()
    {
        if (Step >= 9)
            if (instance)
                instance->SetData(DATA_EXPLODE, DONE);

        me->SetNoCallAssistance(true);
        Fireball_Timer = 5000+rand()%10000;
        ArcaneDisruption_Timer = 45000;
        MindControl_Timer = 40000;
        Phoenix_Timer = 50000;
        ShockBarrier_Timer = 60000;
        FlameStrike_Timer = 30000;
        GravityLapse_Timer = 20000;
        GravityLapse_Phase = 0;
        NetherBeam_Timer = 8000;
        NetherVapor_Timer = 10000;
        Check_Timer = 4000;
        PyrosCasted = 0;
        Phase = 0;
        InGravityLapse = false;
        IsCastingFireball = false;
        ChainPyros = false;

        Step = 0;

        me->SetObjectScale(1.0f);

        if (!me->isInCombat())
            PrepareAdvisors();

        DeleteLegs();
        summons.DespawnAll();

        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        me->RemoveUnitMovementFlag(MOVEFLAG_WALK_MODE);
        SetCombatMovement(false);

        me->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_STATE_STAND);
        me->setEmoteState(EMOTE_STATE_STAND);

        if (instance)
            instance->SetData(DATA_KAELTHASEVENT, NOT_STARTED);
    }

    void PrepareAdvisors()
    {
        Creature* creature;
        for (uint8 i = 0; i < 4; ++i)
        {
            creature = (Creature*)(Unit::GetUnit((*me), AdvisorGuid[i]));
            if (creature)
            {
                creature->Respawn();
                creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                creature->setFaction(me->getFaction());
                creature->AI()->EnterEvadeMode();
            }
        }
    }

    void StartEvent()
    {
        if (!instance)
            return;

        AdvisorGuid[0] = instance->GetData64(DATA_THALADREDTHEDARKENER);
        AdvisorGuid[1] = instance->GetData64(DATA_LORDSANGUINAR);
        AdvisorGuid[2] = instance->GetData64(DATA_GRANDASTROMANCERCAPERNIAN);
        AdvisorGuid[3] = instance->GetData64(DATA_MASTERENGINEERTELONICUS);

        if (!AdvisorGuid[0] || !AdvisorGuid[1] || !AdvisorGuid[2] || !AdvisorGuid[3])
        {
            sLog->outError("TSCR: Kael'Thas One or more advisors missing, Skipping Phases 1-3");

            DoScriptText(SAY_PHASE4_INTRO2, me);
            Phase = 4;

            instance->SetData(DATA_KAELTHASEVENT, 4);

            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);

            if (Unit *pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0))
                AttackStart(pTarget);
        }
        else
        {
            //PrepareAdvisors();

            DoScriptText(SAY_INTRO, me);

            instance->SetData(DATA_KAELTHASEVENT, IN_PROGRESS);
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);

            PhaseSubphase = 0;
            Phase_Timer = 23000;
            Phase = 1;
        }
    }

    void KilledUnit()
    {
        switch (rand()%3)
        {
        case 0: DoScriptText(SAY_SLAY1, me); break;
        case 1: DoScriptText(SAY_SLAY2, me); break;
        case 2: DoScriptText(SAY_SLAY3, me); break;
        }
    }

    void JustSummoned(Creature* summoned)
    {
        if (summoned->GetEntry() == PHOENIX)
        {
            summoned->setFaction(me->getFaction());
            Unit *pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0);
            if (pTarget)
                summoned->AI()->AttackStart(pTarget);
            summons.Summon(summoned);
        }
        else if (summoned->GetEntry() == KAEL_TRIGGER)
        {
            if (Step < 11) // Beams during ascention
                summoned->CastSpell(((TempSummon*)summoned)->GetSummoner(), SPELL_PURPLE_BEAM_RIGHT, false);
            else // explosion effects on Kael during gaining power
                summoned->CastSpell(((TempSummon*)summoned)->GetSummoner(), RAND(SPELL_GAIN_POWER1, SPELL_GAIN_POWER2, SPELL_GAIN_POWER3), false);
        }
        else if (summoned->GetEntry() == KAEL_TRIGGER2)
        {
            summoned->CastSpell(((TempSummon*)summoned)->GetSummoner(), SPELL_PURPLE_BEAM_LEFT, false);
        }
    }

    void SummonedCreatureDespawn(Creature *summon) {summons.Despawn(summon);}

    void JustDied(Unit* Killer)
    {
        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);

        DoScriptText(SAY_DEATH, me);

        DeleteLegs();
        summons.DespawnAll();

        if (instance)
            instance->SetData(DATA_KAELTHASEVENT, DONE);

        Creature* creature;
        for (uint8 i = 0; i < 4; ++i)
        {
            creature = (Unit::GetCreature((*me), AdvisorGuid[i]));
            if (creature)
            {
                creature->setDeathState(JUST_DIED);
            }
        }
    }

    uint32 Intro_Next(uint32 Step) //animation sequence when starting phase 5
    {    /*TODO list:
        - find a proper way to implement netherbeams (36089, 36090)
        - find right spells and animations for an arcane storm (some spells known: 36196, 36197, 36198)
        - setup proper timers
        - others
        */

        switch (Step)
        {
            case 0:
                instance->SetData(DATA_KAELTHASEVENT, 4);
                me->AttackStop();
                me->SetReactState(REACT_PASSIVE);
                me->GetMotionMaster()->Clear();
                me->GetMotionMaster()->MoveIdle();
                DoTeleportTo(795.0f, -0.46f, 48.7f);
                me->Relocate(795.0f, -0.46f, 48.7f);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                ChainPyros = false;
                return 1000;
            case 1:
                DoScriptText(SAY_PHASE5_NUTS, me);
                DoCast(me, SPELL_GAINING_POWER, true);
                return 1000;
            case 2:
                //NetherBeam spells needed here
                return 0;
            case 3:

                return 1000;
            case 4:
                beam[0] = me->SummonCreature(KAEL_TRIGGER2, 798.91f, -38.11f, 85.18f, 1.53f, TEMPSUMMON_TIMED_DESPAWN, 19000);
                beam[1] = me->SummonCreature(KAEL_TRIGGER, 798.56f, 36.16f, 85.18f, 4.68f, TEMPSUMMON_TIMED_DESPAWN, 19000);
                me->SetUnitMovementFlags(MOVEFLAG_LEVITATING + MOVEFLAG_ONTRANSPORT + MOVEFLAG_SWIMMING);
                me->RemoveUnitMovementFlag(MOVEFLAG_WALK_MODE);
                me->SendMonsterMoveWithSpeed(GRAVITY_X-0.05f, GRAVITY_Y-0.05f, GRAVITY_Z+25.0f, 12000);
                DoCast(me, SPELL_EXPLODE_SHAKE1, true);
                me->SetObjectScale(1.2f);
                return 4000;
            case 5:
                beam[2] = me->SummonCreature(KAEL_TRIGGER, 847.78f, 14.56f, 59.76f, 3.5f, TEMPSUMMON_TIMED_DESPAWN, 15000);
                beam[3] = me->SummonCreature(KAEL_TRIGGER2, 847.78f, -16.396f, 59.76f, 3.5f, TEMPSUMMON_TIMED_DESPAWN, 15000);
                DoCast(me, SPELL_NETHERBEAM_GLOW1, true);
                me->SetObjectScale(1.4f);
                return 4000;
            case 6:
                beam[4] = me->SummonCreature(KAEL_TRIGGER, 844.368f, 6.19f, 59.76f, 3.1f, TEMPSUMMON_TIMED_DESPAWN, 11000);
                beam[5] = me->SummonCreature(KAEL_TRIGGER2, 844.368f, -7.72f, 59.76f, 3.1f, TEMPSUMMON_TIMED_DESPAWN, 11000);
                DoCast(me, SPELL_EXPLODE_SHAKE2, true);
                DoCast(me, SPELL_NETHERBEAM_GLOW2, true);
                me->SetObjectScale(1.6f);
                return 4000;
            case 7:
                DoCast(me, SPELL_NETHERBEAM_GLOW3, true);
                me->SetObjectScale(1.8f);
                return 4000;
            case 8:
                DoCast(me, SPELL_EXPLODE_SHAKE3, true);
                me->SetObjectScale(2.0f);
                return 2000;
            case 9:
                me->RemoveAllAuras();
                me->InterruptNonMeleeSpells(false);
                me->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_STATE_DROWNED);
                me->setEmoteState(EMOTE_STATE_DROWNED);
                return 10;
            case 10:
                DoCast(me, SPELL_EXPLODE, true);
                if (instance)
                    instance->SetData(DATA_EXPLODE, DONE);
                return 2000;
            case 11:
                DoCast(me, SPELL_EXPLODE_SHAKE4, true);
                me->SummonCreature(KAEL_TRIGGER, 844.368f, -7.72f, 59.76f, 3.1f, TEMPSUMMON_TIMED_DESPAWN, 2000);
                DoPlaySoundToSet(me, 6477);
                return 500;
            case 12:
                DoCast(me, SPELL_NETHERBEAM_GLOW1, true);
                me->SummonCreature(KAEL_TRIGGER, 844.368f, -7.72f, 59.76f, 3.1f, TEMPSUMMON_TIMED_DESPAWN, 2000);
                DoPlaySoundToSet(me, 6477);
                return 500;
            case 13:
                me->SummonCreature(KAEL_TRIGGER, 844.368f, -7.72f, 59.76f, 3.1f, TEMPSUMMON_TIMED_DESPAWN, 2000);
                DoPlaySoundToSet(me, 6477);
                return 250;
            case 14:
                me->SummonCreature(KAEL_TRIGGER, 844.368f, -7.72f, 59.76f, 3.1f, TEMPSUMMON_TIMED_DESPAWN, 2000);
                DoPlaySoundToSet(me, 6477);
                return 250;
            case 15:
                me->SummonCreature(KAEL_TRIGGER, 844.368f, -7.72f, 59.76f, 3.1f, TEMPSUMMON_TIMED_DESPAWN, 2000);
                DoPlaySoundToSet(me, 6477);
                return 500;
            case 16:
                me->SummonCreature(KAEL_TRIGGER, 844.368f, -7.72f, 59.76f, 3.1f, TEMPSUMMON_TIMED_DESPAWN, 2000);
                DoPlaySoundToSet(me, 6477);
                return 1000;
            case 17:
                me->SummonCreature(KAEL_TRIGGER, 844.368f, -7.72f, 59.76f, 3.1f, TEMPSUMMON_TIMED_DESPAWN, 2000);
                DoPlaySoundToSet(me, 6477);
                return 250;
            case 18:
                me->SummonCreature(KAEL_TRIGGER, 844.368f, -7.72f, 59.76f, 3.1f, TEMPSUMMON_TIMED_DESPAWN, 2000);
                DoPlaySoundToSet(me, 6477);
                return 500;
            case 19:
                me->SummonCreature(KAEL_TRIGGER, 844.368f, -7.72f, 59.76f, 3.1f, TEMPSUMMON_TIMED_DESPAWN, 2000);
                DoPlaySoundToSet(me, 6477);
                return 250;
            case 20:
                me->SummonCreature(KAEL_TRIGGER, 844.368f, -7.72f, 59.76f, 3.1f, TEMPSUMMON_TIMED_DESPAWN, 2000);
                DoPlaySoundToSet(me, 6477);
                return 1000;
            case 21:
                me->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_STATE_STAND);
                me->setEmoteState(EMOTE_STATE_STAND);
                DoCast(me, SPELL_FULLPOWER);
                me->SummonCreature(KAEL_TRIGGER, 844.368f, -7.72f, 59.76f, 3.1f, TEMPSUMMON_TIMED_DESPAWN, 2000);
                DoPlaySoundToSet(me, 6477);
                return 1000;
            case 22:
                me->SummonCreature(KAEL_TRIGGER, 844.368f, -7.72f, 59.76f, 3.1f, TEMPSUMMON_TIMED_DESPAWN, 2000);
                DoPlaySoundToSet(me, 6477);
                return 2000;
            case 23:
                me->SendMonsterMoveWithSpeed(GRAVITY_X, GRAVITY_Y, GRAVITY_Z, 13000);
                me->Relocate(GRAVITY_X, GRAVITY_Y, GRAVITY_Z);
                me->SummonCreature(KAEL_TRIGGER, 844.368f, -7.72f, 59.76f, 3.1f, TEMPSUMMON_TIMED_DESPAWN, 2000);
                DoPlaySoundToSet(me, 6477);
                me->RemoveAurasDueToSpell(SPELL_NETHERBEAM_GLOW1);
                return 13000;
            case 24:
                me->RemoveAllAuras();
                me->Relocate(GRAVITY_X, GRAVITY_Y, GRAVITY_Z);
                me->InterruptNonMeleeSpells(false);
                me->RemoveAurasDueToSpell(SPELL_FULLPOWER);
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                me->RemoveUnitMovementFlag(MOVEFLAG_LEVITATING);
                me->RemoveUnitMovementFlag(MOVEFLAG_SWIMMING);
                me->RemoveUnitMovementFlag(MOVEFLAG_ONTRANSPORT);
                me->AddUnitMovementFlag(MOVEFLAG_WALK_MODE);
                me->SetReactState(REACT_AGGRESSIVE);
                DoStartMovement(me->getVictim());
                AttackStart(me->getVictim());
                Fireball_Timer = 0;
                Phase = 6;
                return 500;
            default: return 0;
        }
        return 0;
    }

    void EnterCombat(Unit *who)
    {
        if (instance && !instance->GetData(DATA_KAELTHASEVENT) && !Phase)
            StartEvent();
    }

    void MoveInLineOfSight(Unit *who)
    {
        if (!me->getVictim() && who->isTargetableForAttack() && who->isInAccessiblePlaceFor (me) && me->IsHostileTo(who))
        {
            if (!me->canFly() && me->GetDistanceZ(who) > CREATURE_Z_ATTACK_RANGE)
                return;

            float attackRadius = me->GetAttackDistance(who);
            if (Phase >= 4 && me->IsWithinDistInMap(who, attackRadius) && me->IsWithinLOSInMap(who))
            {
                AttackStart(who);
            }
            else if (who->isAlive())
            {
                if (instance && !instance->GetData(DATA_KAELTHASEVENT) && !Phase && me->IsWithinDistInMap(who, 60.0f))
                    StartEvent();

                //add to the threat list, so we can use SelectTarget
                me->AddThreat(who, 0.0f);
            }
        }
    }

    void UpdateAI(const uint32 diff)
    {
        if (instance && Phase)
        {
            if (instance->GetData(DATA_KAELTHASEVENT) == IN_PROGRESS && me->getThreatManager().getThreatList().empty())
            {
                EnterEvadeMode();
                return;
            }
        }
        //Phase 1
        switch (Phase)
        {
            case 1:
            {
                Unit *pTarget;
                Creature* Advisor;

                //Subphase switch
                switch (PhaseSubphase)
                {
                    //Subphase 1 - Start
                    case 0:
                        if (Phase_Timer <= diff)
                        {
                            DoScriptText(SAY_INTRO_THALADRED, me);

                            //start advisor within 7 seconds
                            Phase_Timer = 7000;

                            ++PhaseSubphase;
                        } else Phase_Timer -= diff;
                        break;

                        //Subphase 1 - Unlock advisor
                    case 1:
                        if (Phase_Timer <= diff)
                        {
                            Advisor = (Creature*)(Unit::GetUnit((*me), AdvisorGuid[0]));

                            if (Advisor)
                            {
                                Advisor->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                                Advisor->setFaction(me->getFaction());

                                pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0);
                                if (pTarget)
                                    Advisor->AI()->AttackStart(pTarget);
                            }

                            ++PhaseSubphase;
                        } else Phase_Timer -= diff;
                        break;

                        //Subphase 2 - Start
                    case 2:
                        Advisor = (Creature*)(Unit::GetUnit((*me), AdvisorGuid[0]));
                        if (Advisor && (Advisor->GetUInt32Value(UNIT_FIELD_BYTES_1) == UNIT_STAND_STATE_DEAD))
                        {
                            DoScriptText(SAY_INTRO_SANGUINAR, me);

                            //start advisor within 12.5 seconds
                            Phase_Timer = 12500;

                            ++PhaseSubphase;
                        }
                        break;

                        //Subphase 2 - Unlock advisor
                    case 3:
                        if (Phase_Timer <= diff)
                        {
                            Advisor = (Creature*)(Unit::GetUnit((*me), AdvisorGuid[1]));

                            if (Advisor)
                            {
                                Advisor->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                                Advisor->setFaction(me->getFaction());

                                pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0);
                                if (pTarget)
                                    Advisor->AI()->AttackStart(pTarget);
                            }

                            ++PhaseSubphase;
                        } else Phase_Timer -= diff;
                        break;

                        //Subphase 3 - Start
                    case 4:
                        Advisor = (Creature*)(Unit::GetUnit((*me), AdvisorGuid[1]));
                        if (Advisor && (Advisor->GetUInt32Value(UNIT_FIELD_BYTES_1) == UNIT_STAND_STATE_DEAD))
                        {
                            DoScriptText(SAY_INTRO_CAPERNIAN, me);

                            //start advisor within 7 seconds
                            Phase_Timer = 7000;

                            ++PhaseSubphase;
                        }
                        break;

                        //Subphase 3 - Unlock advisor
                    case 5:
                        if (Phase_Timer <= diff)
                        {
                            Advisor = (Creature*)(Unit::GetUnit((*me), AdvisorGuid[2]));

                            if (Advisor)
                            {
                                Advisor->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                                Advisor->setFaction(me->getFaction());

                                pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0);
                                if (pTarget)
                                    Advisor->AI()->AttackStart(pTarget);
                            }

                            ++PhaseSubphase;
                        } else Phase_Timer -= diff;
                        break;

                        //Subphase 4 - Start
                    case 6:
                        Advisor = (Creature*)(Unit::GetUnit((*me), AdvisorGuid[2]));
                        if (Advisor && (Advisor->GetUInt32Value(UNIT_FIELD_BYTES_1) == UNIT_STAND_STATE_DEAD))
                        {
                            DoScriptText(SAY_INTRO_TELONICUS, me);

                            //start advisor within 8.4 seconds
                            Phase_Timer = 8400;

                            ++PhaseSubphase;
                        }
                        break;

                        //Subphase 4 - Unlock advisor
                    case 7:
                        if (Phase_Timer <= diff)
                        {
                            Advisor = (Creature*)(Unit::GetUnit((*me), AdvisorGuid[3]));

                            if (Advisor)
                            {
                                Advisor->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                                Advisor->setFaction(me->getFaction());

                                pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0);
                                if (pTarget)
                                    Advisor->AI()->AttackStart(pTarget);
                            }

                            Phase_Timer = 3000;

                            ++PhaseSubphase;
                        } else Phase_Timer -= diff;
                        break;

                        //End of phase 1
                    case 8:
                        Advisor = (Creature*)(Unit::GetUnit((*me), AdvisorGuid[3]));
                        if (Advisor && (Advisor->GetUInt32Value(UNIT_FIELD_BYTES_1) == UNIT_STAND_STATE_DEAD))
                        {
                            Phase = 2;
                            instance->SetData(DATA_KAELTHASEVENT, 2);

                            DoScriptText(SAY_PHASE2_WEAPON, me);
                            PhaseSubphase = 0;
                            Phase_Timer = 3500;
                            DoCast(me, SPELL_SUMMON_WEAPONS);
                        }
                        break;
                }
            }
            break;

            case 2:
            {
                if (PhaseSubphase == 0)
                {
                    if (Phase_Timer <= diff)
                    {
                        PhaseSubphase = 1;
                    } else Phase_Timer -= diff;
                }

                //Spawn weapons
                if (PhaseSubphase == 1)
                {
                    Creature* Weapon;
                    for (uint32 i = 0; i < 7; ++i)
                    {
                        Unit *pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0);
                        Weapon = me->SummonCreature(((uint32)KaelthasWeapons[i][0]),KaelthasWeapons[i][1],KaelthasWeapons[i][2],KaelthasWeapons[i][3],0, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 10000);

                        if (!Weapon)
                            sLog->outError("SOSCR: Kael'thas weapon %i could not be spawned", i);
                        else
                        {
                            Weapon->setFaction(me->getFaction());
                            Weapon->AI()->AttackStart(pTarget);
                            Weapon->CastSpell(Weapon, SPELL_WEAPON_SPAWN, false);
                            WeaponGuid[i] = Weapon->GetGUID();
                        }
                    }

                    PhaseSubphase = 2;
                    Phase_Timer = TIME_PHASE_2_3;
                }

                if (PhaseSubphase == 2)
                    if (Phase_Timer <= diff)
                {
                    DoScriptText(SAY_PHASE3_ADVANCE, me);
                    instance->SetData(DATA_KAELTHASEVENT, 3);
                    Phase = 3;
                    PhaseSubphase = 0;
                    Phase_Timer = 12000;
                } else Phase_Timer -= diff;
                 //missing Resetcheck
            }
            break;

            case 3:
            {
                if (PhaseSubphase == 0)
                {
                    if (Phase_Timer <= diff)
                    {
                        PhaseSubphase = 1;
                    } else Phase_Timer -= diff;
                }

                if (PhaseSubphase == 1)
                {
                    //Respawn advisors
                    Unit *pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0);

                    Creature* Advisor;
                    for (uint32 i = 0; i < 4; ++i)
                    {
                        Advisor = (Creature*)(Unit::GetUnit((*me), AdvisorGuid[i]));
                        if (!Advisor)
                            sLog->outError("TSCR: Kael'Thas Advisor %u does not exist. Possibly despawned? Incorrectly Killed?", i);
                        else if (pTarget)
                        {
                            ((advisorbase_ai*)Advisor->AI())->Revive(pTarget);
                        }
                    }

                    PhaseSubphase = 2;
                    Phase_Timer = TIME_PHASE_3_4;
                }

                if (PhaseSubphase == 2)
                {
                    if (Phase_Timer <= diff)
                    {
                        DoScriptText(SAY_PHASE4_INTRO2, me);
                        Phase = 4;

                        instance->SetData(DATA_KAELTHASEVENT, 4);

                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);

                        if (Unit *pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0))
                        {
                            DoResetThreat(); // only healers will be at top threat, so reset(not delete) all players's threat when Kael comes to fight
                            AttackStart(pTarget);
                        }
                        Phase_Timer = 30000;
                    } else Phase_Timer -= diff;
                }
            }
            break;

            case 4:
            case 5:
            case 6:
            {
                //Return since we have no target
                if (!UpdateVictim())
                    return;

                if (!InGravityLapse && !ChainPyros && Phase != 5)
                {
                    //Fireball_Timer
                    if (Fireball_Timer <= diff)
                    {
                        if (!IsCastingFireball)
                        {
                            if (!me->IsNonMeleeSpellCasted(false))
                            {
                                //interruptable
                                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_INTERRUPT_CAST, false);
                                me->CastSpell(me->getVictim(), SPELL_FIREBALL, false);
                                IsCastingFireball = true;
                                Fireball_Timer = 2500;
                            }
                        }
                        else
                        {
                            //apply resistance
                            me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_INTERRUPT_CAST, true);
                            IsCastingFireball = false;
                            Fireball_Timer = 5000+rand()%10000;
                        }
                    } else Fireball_Timer -= diff;

                    //ArcaneDisruption_Timer
                    if (ArcaneDisruption_Timer <= diff)
                    {
                        DoCast(me->getVictim(), SPELL_ARCANE_DISRUPTION, true);

                        ArcaneDisruption_Timer = 60000;
                    } else ArcaneDisruption_Timer -= diff;

                    //FlameStrike_Timer
                    if (FlameStrike_Timer <= diff)
                    {
                        if (Unit* pUnit = SelectTarget(SELECT_TARGET_RANDOM, 0, 70, true))
                            DoCast(pUnit, SPELL_FLAME_STRIKE);

                        FlameStrike_Timer = 30000;
                    } else FlameStrike_Timer -= diff;

                    if (MindControl_Timer <= diff)
                    {
                        if (me->getThreatManager().getThreatList().size() >= 2)
                        for (uint32 i = 0; i < 3; i++)
                        {
                            Unit *pTarget =SelectTarget(SELECT_TARGET_RANDOM, 1, 70, true);
                            if (!pTarget)
                                pTarget = me->getVictim();
                            sLog->outDebug("TSCR: Kael'Thas mind control not supported.");
                            if (pTarget)
                                DoCast(pTarget, SPELL_MIND_CONTROL);
                        }

                        MindControl_Timer = 60000;
                    } else MindControl_Timer -= diff;

                    //Phoenix_Timer
                    if (Phoenix_Timer <= diff)
                    {
                        DoCast(me, SPELL_SUMMON_PHOENIX);

                        switch (rand() % 2)
                        {
                            case 0: DoScriptText(SAY_SUMMON_PHOENIX1, me); break;
                            case 1: DoScriptText(SAY_SUMMON_PHOENIX2, me); break;
                        }

                        Phoenix_Timer = 60000;
                    }
                    else Phoenix_Timer -= diff;
                }

                //Phase 4 specific spells
                if (Phase == 4)
                {
                    if (me->GetHealth()*100 / me->GetMaxHealth() < 50)
                    {
                        Phase = 5;
                        Phase_Timer = 0;
                    }

                    //ShockBarrier_Timer
                    if (ShockBarrier_Timer <= diff)
                    {
                        DoCast(me, SPELL_SHOCK_BARRIER);
                        ChainPyros = true;
                        PyrosCasted = 0;
                        Check_Timer = 0;

                        ShockBarrier_Timer = 60000;
                    } else ShockBarrier_Timer -= diff;

                    //Chain Pyros (3 of them max)
                    if (ChainPyros)
                    {
                        if (PyrosCasted < 3 && Check_Timer <= diff)
                        {
                            DoCast(me->getVictim(), SPELL_PYROBLAST);
                            ++PyrosCasted;

                            Check_Timer = 4200;
                        } else Check_Timer -= diff;

                        if (PyrosCasted >= 3)
                        {
                            ChainPyros = false;
                            Fireball_Timer = 2500;
                            ArcaneDisruption_Timer = 60000;
                        }
                    }
                } else Check_Timer -= 4100;

                if (Phase == 5)
                {
                    if (Phase_Timer <= diff)
                    {
                        Phase_Timer = Intro_Next(Step++);

                    } else Phase_Timer -= diff;
                }

                if (Phase == 6)
                {
                    //GravityLapse_Timer
                    if (GravityLapse_Timer <= diff)
                    {
                        std::list<HostileReference*>::iterator i = me->getThreatManager().getThreatList().begin();
                        switch (GravityLapse_Phase)
                        {
                            case 0:
                                me->GetMotionMaster()->Clear();
                                me->GetMotionMaster()->MoveIdle();
                                DoTeleportTo(GRAVITY_X, GRAVITY_Y, GRAVITY_Z);
                                // 1) Kael'thas will portal the whole raid right into his body
                                for (i = me->getThreatManager().getThreatList().begin(); i != me->getThreatManager().getThreatList().end();)
                                {
                                    Unit* pUnit = Unit::GetUnit((*me), (*i)->getUnitGuid());
                                    ++i;
                                    if (pUnit && (pUnit->GetTypeId() == TYPEID_PLAYER))
                                    {
                                        //Use work around packet to prevent player from being dropped from combat
                                        DoTeleportPlayer(pUnit, GRAVITY_X, GRAVITY_Y, GRAVITY_Z, pUnit->GetOrientation());
                                    }
                                }
                                GravityLapse_Timer = 500;
                                ++GravityLapse_Phase;
                                InGravityLapse = true;
                                ShockBarrier_Timer = 1000;
                                NetherBeam_Timer = 5000;
                                break;

                            case 1:
                                switch (rand()%2)
                                {
                                    case 0: DoScriptText(SAY_GRAVITYLAPSE1, me); break;
                                    case 1: DoScriptText(SAY_GRAVITYLAPSE2, me); break;
                                }

                                // 2) At that point he will put a Gravity Lapse debuff on everyone
                                for (i = me->getThreatManager().getThreatList().begin(); i != me->getThreatManager().getThreatList().end();)
                                {
                                    Unit* pUnit = Unit::GetUnit((*me), (*i)->getUnitGuid());
                                    ++i;
                                    if (pUnit && pUnit->GetTypeId() == TYPEID_PLAYER)
                                    {
                                        me->CastSpell(pUnit, SPELL_KNOCKBACK, true);
                                        //Gravity lapse - needs an exception in Spell system to work

                                        pUnit->CastSpell(pUnit, SPELL_GRAVITY_LAPSE, true, 0, 0, me->GetGUID());
                                        pUnit->CastSpell(pUnit, SPELL_GRAVITY_LAPSE_AURA, true, 0, 0, me->GetGUID());

                                        //Using packet workaround
                                        WorldPacket data(12);
                                        data.SetOpcode(SMSG_MOVE_SET_CAN_FLY);
                                        data << pUnit->GetPackGUID();
                                        data << uint32(0);
                                        pUnit->SendMessageToSet(&data, true);
                                    }
                                }
                                GravityLapse_Timer = 10000;
                                ++GravityLapse_Phase;
                                break;

                            case 2:
                                //Cast nether vapor aura on self
                                me->InterruptNonMeleeSpells(false);
                                DoCast(me, SPELL_NETHER_VAPOR);

                                GravityLapse_Timer = 20000;
                                ++GravityLapse_Phase;
                                break;

                            case 3:
                                //Remove flight
                                for (i = me->getThreatManager().getThreatList().begin(); i != me->getThreatManager().getThreatList().end();)
                                {
                                    Unit* pUnit = Unit::GetUnit((*me), (*i)->getUnitGuid());
                                    ++i;
                                    if (pUnit && pUnit->GetTypeId() == TYPEID_PLAYER)
                                    {
                                        //Using packet workaround
                                        WorldPacket data(12);
                                        data.SetOpcode(SMSG_MOVE_UNSET_CAN_FLY);
                                        data << pUnit->GetPackGUID();
                                        data << uint32(0);
                                        pUnit->SendMessageToSet(&data, true);
                                    }
                                }
                                me->RemoveAurasDueToSpell(SPELL_NETHER_VAPOR);
                                InGravityLapse = false;
                                GravityLapse_Timer = 60000;
                                GravityLapse_Phase = 0;
                                DoStartMovement(me->getVictim());
                                AttackStart(me->getVictim());
                                DoResetThreat();
                                break;
                        }
                    } else GravityLapse_Timer -= diff;

                    if (InGravityLapse)
                    {
                        //ShockBarrier_Timer
                        if (ShockBarrier_Timer <= diff)
                        {
                            DoCast(me, SPELL_SHOCK_BARRIER);
                            ShockBarrier_Timer = 20000;
                        } else ShockBarrier_Timer -= diff;

                        //NetherBeam_Timer
                        if (NetherBeam_Timer <= diff)
                        {
                            if (Unit* pUnit = SelectUnit(SELECT_TARGET_RANDOM, 0))
                                DoCast(pUnit, SPELL_NETHER_BEAM);

                            NetherBeam_Timer = 4000;
                        } else NetherBeam_Timer -= diff;
                    }
                }

                if (!InGravityLapse && !(Phase == 5))
                    DoMeleeAttackIfReady();
            }
        }
    }
};

//Thaladred the Darkener AI
struct boss_thaladred_the_darkenerAI : public advisorbase_ai
{
    boss_thaladred_the_darkenerAI(Creature *c) : advisorbase_ai(c) {}

    uint32 Gaze_Timer;
    uint32 Silence_Timer;
    uint32 PsychicBlow_Timer;

    void Reset()
    {
        me->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_DISPLAY, EQUIP_DEVASTATION);
        me->SetUInt32Value(UNIT_VIRTUAL_ITEM_INFO, DEVASTATION_EQUIP_INFO);
        me->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_DISPLAY+1, 0);
        me->SetUInt32Value(UNIT_VIRTUAL_ITEM_INFO+2, 0);

        Gaze_Timer = 100;
        Silence_Timer = 20000;
        PsychicBlow_Timer = 10000;

        advisorbase_ai::Reset();
    }

    void JustDied(Unit* pKiller)
    {
        DoScriptText(SAY_THALADRED_DEATH, me);
    }

    void EnterCombat(Unit *who)
    {
        if (me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
            return;

        if (!who || FakeDeath)
            return;

        DoScriptText(SAY_THALADRED_AGGRO, me);
        me->AddThreat(who, 5000000.0f);
    }

    void UpdateAI(const uint32 diff)
    {
        advisorbase_ai::UpdateAI(diff);

        //Faking death, don't do anything
        if (FakeDeath)
            return;

        //Return since we have no target
        if (!UpdateVictim())
            return;

        //Gaze_Timer
        if (Gaze_Timer <= diff)
        {
            if (Unit *pTarget = SelectTarget(SELECT_TARGET_RANDOM, 0, 100, true))
            {
                DoResetThreat();
                if (pTarget)
                {
                    me->AddThreat(pTarget, 5000000.0f);
                    DoScriptText(EMOTE_THALADRED_GAZE, me, pTarget);
                }
                Gaze_Timer = 9000;
            }
        } else Gaze_Timer -= diff;

        //Silence_Timer
        if (Silence_Timer <= diff)
        {
            DoCast(me->getVictim(), SPELL_SILENCE);
            Silence_Timer = 20000;
        } else Silence_Timer -= diff;

        //PsychicBlow_Timer
        if (PsychicBlow_Timer <= diff)
        {
            DoCast(me->getVictim(), SPELL_PSYCHIC_BLOW);
            PsychicBlow_Timer = 20000+rand()%5000;
        } else PsychicBlow_Timer -= diff;

        if (me->isAttackReady() && me->IsWithinMeleeRange(me->getVictim()))
            DoCast(me->getVictim(), SPELL_REND);

        DoMeleeAttackIfReady();
    }
};

//Lord Sanguinar AI
struct boss_lord_sanguinarAI : public advisorbase_ai
{
    boss_lord_sanguinarAI(Creature *c) : advisorbase_ai(c){}

    uint32 Fear_Timer;

    void Reset()
    {
        me->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_DISPLAY, EQUIP_RED_MACE);
        me->SetUInt32Value(UNIT_VIRTUAL_ITEM_INFO, RED_MACE_EQUIP_INFO);
        me->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_DISPLAY+1, EQUIP_PHASESHIFT_BULWARK);
        me->SetUInt32Value(UNIT_VIRTUAL_ITEM_INFO+2, PHASESHIFT_BULWARK_EQUIP_INFO);
        me->SetUInt32Value(UNIT_VIRTUAL_ITEM_INFO+3, PHASESHIFT_BULWARK_EQUIP_INFO2);

        Fear_Timer = 20000;
        advisorbase_ai::Reset();
    }

    void JustDied(Unit* Killer)
    {
        DoScriptText(SAY_SANGUINAR_DEATH, me);
    }

    void EnterCombat(Unit *who)
    {
        if (me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
            return;

        if (!who || FakeDeath)
            return;

        DoScriptText(SAY_SANGUINAR_AGGRO, me);
    }

    void UpdateAI(const uint32 diff)
    {
        advisorbase_ai::UpdateAI(diff);

        //Faking death, don't do anything
        if (FakeDeath)
            return;

        //Return since we have no target
        if (!UpdateVictim())
            return;

        //Fear_Timer
        if (Fear_Timer <= diff)
        {
            DoCast(me->getVictim(), SPELL_BELLOWING_ROAR);
            Fear_Timer = 25000+rand()%10000;                //approximately every 30 seconds
        } else Fear_Timer -= diff;

        DoMeleeAttackIfReady();
    }
};

//Grand Astromancer Capernian AI
struct boss_grand_astromancer_capernianAI : public advisorbase_ai
{
    boss_grand_astromancer_capernianAI(Creature *c) : advisorbase_ai(c){}

    uint32 Fireball_Timer;
    uint32 Conflagration_Timer;
    uint32 ArcaneExplosion_Timer;

    void Reset()
    {
        me->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_DISPLAY, EQUIP_SOD);
        me->SetUInt32Value(UNIT_VIRTUAL_ITEM_INFO, SOD_EQUIP_INFO);
        me->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_DISPLAY+1, 0);
        me->SetUInt32Value(UNIT_VIRTUAL_ITEM_INFO+2, 0);

        Fireball_Timer = 0;
        Conflagration_Timer = 20000;
        ArcaneExplosion_Timer = 5000;

        advisorbase_ai::Reset();
    }

    void JustDied(Unit* pKiller)
    {
        DoScriptText(SAY_CAPERNIAN_DEATH, me);
    }

    void AttackStart(Unit* who)
    {
        if (!who || FakeDeath || me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
            return;

        if (me->Attack(who, true))
        {
            me->AddThreat(who, 0.0f);
            me->SetInCombatWith(who);
            who->SetInCombatWith(me);

            me->GetMotionMaster()->MoveChase(who, CAPERNIAN_DISTANCE);
        }
    }

    void EnterCombat(Unit *who)
    {
        if (me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
            return;

        if (!who || FakeDeath)
            return;

        DoScriptText(SAY_CAPERNIAN_AGGRO, me);
    }

    void UpdateAI(const uint32 diff)
    {
        advisorbase_ai::UpdateAI(diff);

        //Faking Death, don't do anything
        if (FakeDeath)
            return;

        //Return since we have no target
        if (!UpdateVictim())
            return;

        //Fireball_Timer
        if (Fireball_Timer <= diff)
        {
            DoCast(me->getVictim(), SPELL_CAPERNIAN_FIREBALL);
            Fireball_Timer = 4000;
        } else Fireball_Timer -= diff;

        //Conflagration_Timer
        if (Conflagration_Timer <= diff)
        {
            Unit *pTarget = NULL;
            pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0);

            if (pTarget && me->IsWithinDistInMap(pTarget, 30))
                DoCast(pTarget, SPELL_CONFLAGRATION);
            else
                DoCast(me->getVictim(), SPELL_CONFLAGRATION);

            Conflagration_Timer = 10000+rand()%5000;
        } else Conflagration_Timer -= diff;

        //ArcaneExplosion_Timer
        if (ArcaneExplosion_Timer <= diff)
        {
            bool InMeleeRange = false;
            Unit *pTarget = NULL;
            std::list<HostileReference*>& m_threatlist = me->getThreatManager().getThreatList();
            for (std::list<HostileReference*>::iterator i = m_threatlist.begin(); i != m_threatlist.end();++i)
            {
                Unit* pUnit = Unit::GetUnit((*me), (*i)->getUnitGuid());
                                                            //if in melee range
                if (pUnit && pUnit->IsWithinDistInMap(me, 5))
                {
                    InMeleeRange = true;
                    pTarget = pUnit;
                    break;
                }
            }

            if (InMeleeRange)
                DoCast(pTarget, SPELL_ARCANE_EXPLOSION);

            ArcaneExplosion_Timer = 4000+rand()%2000;
        } else ArcaneExplosion_Timer -= diff;

        //Do NOT deal any melee damage.
    }
};

//Master Engineer Telonicus AI
struct boss_master_engineer_telonicusAI : public advisorbase_ai
{
    boss_master_engineer_telonicusAI(Creature *c) : advisorbase_ai(c){}

    uint32 Bomb_Timer;
    uint32 RemoteToy_Timer;

    void Reset()
    {
        me->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_DISPLAY, EQUIP_INFINITY_BLADE);
        me->SetUInt32Value(UNIT_VIRTUAL_ITEM_INFO, INFINITY_BLADE_EQUIP_INFO);
        me->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_DISPLAY+1, 0);
        me->SetUInt32Value(UNIT_VIRTUAL_ITEM_INFO+2, 0);

        Bomb_Timer = 10000;
        RemoteToy_Timer = 5000;

        advisorbase_ai::Reset();
    }

    void JustDied(Unit* pKiller)
    {
         DoScriptText(SAY_TELONICUS_DEATH, me);
    }

    void EnterCombat(Unit *who)
    {
        if (me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
            return;

        if (!who || FakeDeath)
            return;

        DoScriptText(SAY_TELONICUS_AGGRO, me);
    }

    void UpdateAI(const uint32 diff)
    {
        advisorbase_ai::UpdateAI(diff);

        //Faking Death, do nothing
        if (FakeDeath)
            return;

        //Return since we have no target
        if (!UpdateVictim())
            return;

        //Bomb_Timer
        if (Bomb_Timer <= diff)
        {
            DoCast(me->getVictim(), SPELL_BOMB);
            Bomb_Timer = 25000;
        } else Bomb_Timer -= diff;

        //RemoteToy_Timer
        if (RemoteToy_Timer <= diff)
        {
            if (Unit *pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0))
                DoCast(pTarget, SPELL_REMOTE_TOY);

            RemoteToy_Timer = 10000+rand()%5000;
        } else RemoteToy_Timer -= diff;

        DoMeleeAttackIfReady();
    }
};

//Flame Strike AI
struct mob_kael_flamestrikeAI : public ScriptedAI
{
    mob_kael_flamestrikeAI(Creature *c) : ScriptedAI(c) {}

    uint32 Timer;
    bool Casting;
    bool KillSelf;

    void Reset()
    {
        Timer = 5000;
        Casting = false;
        KillSelf = false;

        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        me->setFaction(14);
    }

    void EnterCombat(Unit *who)
    {
    }

    void MoveInLineOfSight(Unit *who)
    {
    }

    void UpdateAI(const uint32 diff)
    {
        if (!Casting)
        {
            DoCast(me, SPELL_FLAME_STRIKE_VIS);
            Casting = true;
        }

        //Timer
        if (Timer <= diff)
        {
            if (!KillSelf)
            {
                me->InterruptNonMeleeSpells(false);
                DoCast(me, SPELL_FLAME_STRIKE_DMG);
            } else me->DealDamage(me, me->GetMaxHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);

            KillSelf = true;
            Timer = 1000;
        } else Timer -= diff;
    }
};

//Phoenix AI
struct mob_phoenix_tkAI : public ScriptedAI
{
    mob_phoenix_tkAI(Creature *c) : ScriptedAI(c)
    {
       instance = c->GetInstanceScript();
    }

    ScriptedInstance* instance;
    uint32 Cycle_Timer;
    bool egg;

    void JustDied(Unit *victim)
    {
        if (egg)
        {
            float x, y, z;
            me->GetPosition(x, y, z);
            z = me->GetMap()->GetHeight(x, y, z);
            if (z == INVALID_HEIGHT)
                z = ROOM_BASE_Z;
            me->SummonCreature(PHOENIX_EGG, x, y, z, me->GetOrientation(),TEMPSUMMON_TIMED_DESPAWN, 16000);
            me->RemoveCorpse();
        }
    }

    void Reset()
    {
        me->AddUnitMovementFlag(MOVEFLAG_ONTRANSPORT | MOVEFLAG_LEVITATING);//birds can fly! :)
        egg = true;
        Cycle_Timer = 2000;
        me->CastSpell(me, SPELL_BURN, true);
    }

    void EnterCombat(Unit *who) { }

    void DamageTaken(Unit* pKiller, uint32 &damage)
    {
    }

    void UpdateAI(const uint32 diff)
    {
        if (Cycle_Timer <= diff)
        {
            if (instance)//check for boss reset
            {
                Creature* Kael = Unit::GetCreature((*me), instance->GetData64(DATA_KAELTHAS));
                if (Kael && Kael->getThreatManager().getThreatList().empty())
                {
                    egg = false;
                    me->DealDamage(me, me->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
                    Cycle_Timer = 2000;
                    return;
                }
            }
            //spell Burn should possible do this, but it doesn't, so do this for now.
            uint32 dmg = urand(4500, 5500);
            if (me->GetHealth() > dmg)
                me->SetHealth(uint32(me->GetHealth()-dmg));
            else//kill itt
                me->DealDamage(me, me->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
            Cycle_Timer = 2000;
        } else Cycle_Timer -= diff;

        if (!UpdateVictim())
            return;
        DoMeleeAttackIfReady();
    }
};

//Phoenix Egg AI
struct mob_phoenix_egg_tkAI : public ScriptedAI
{
    mob_phoenix_egg_tkAI(Creature *c) : ScriptedAI(c) {}

    uint32 Rebirth_Timer;
    bool summoned;

    void Reset(){
            Rebirth_Timer = 15000;
            summoned = false;
    }

    //ignore any
    void MoveInLineOfSight(Unit* who) { return; }

    void AttackStart(Unit* who)
    {
        if (me->Attack(who, false))
        {
            me->SetInCombatWith(who);
            who->SetInCombatWith(me);

            DoStartNoMovement(who);
        }
    }

    void EnterCombat(Unit *who) { }

    void JustSummoned(Creature* summoned)
    {
        summoned->AddThreat(me->getVictim(), 0.0f);
        summoned->CastSpell(summoned, SPELL_REBIRTH, false);
    }

    void UpdateAI(const uint32 diff)
    {
        if (Rebirth_Timer <= diff)
        {
            if (!summoned)
            {
                Creature* Phoenix = me->SummonCreature(PHOENIX, me->GetPositionX(),me->GetPositionY(),me->GetPositionZ(),me->GetOrientation(),TEMPSUMMON_CORPSE_DESPAWN, 5000);
                summoned = true;
            }
            me->DealDamage(me, me->GetMaxHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
        } else Rebirth_Timer -= diff;
    }
};

CreatureAI* GetAI_boss_kaelthas(Creature* creature)
{
    return new boss_kaelthasAI (creature);
}

CreatureAI* GetAI_boss_thaladred_the_darkener(Creature* creature)
{
    return new boss_thaladred_the_darkenerAI (creature);
}

CreatureAI* GetAI_boss_lord_sanguinar(Creature* creature)
{
    return new boss_lord_sanguinarAI (creature);
}

CreatureAI* GetAI_boss_grand_astromancer_capernian(Creature* creature)
{
    return new boss_grand_astromancer_capernianAI (creature);
}

CreatureAI* GetAI_boss_master_engineer_telonicus(Creature* creature)
{
    return new boss_master_engineer_telonicusAI (creature);
}

CreatureAI* GetAI_mob_kael_flamestrike(Creature* creature)
{
    return new mob_kael_flamestrikeAI (creature);
}

CreatureAI* GetAI_mob_phoenix_tk(Creature* creature)
{
    return new mob_phoenix_tkAI (creature);
}

CreatureAI* GetAI_mob_phoenix_egg_tk(Creature* creature)
{
    return new mob_phoenix_egg_tkAI (creature);
}

void AddSC_boss_kaelthas()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "boss_kaelthas";
    newscript->GetAI = &GetAI_boss_kaelthas;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "boss_thaladred_the_darkener";
    newscript->GetAI = &GetAI_boss_thaladred_the_darkener;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "boss_lord_sanguinar";
    newscript->GetAI = &GetAI_boss_lord_sanguinar;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "boss_grand_astromancer_capernian";
    newscript->GetAI = &GetAI_boss_grand_astromancer_capernian;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "boss_master_engineer_telonicus";
    newscript->GetAI = &GetAI_boss_master_engineer_telonicus;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_kael_flamestrike";
    newscript->GetAI = &GetAI_mob_kael_flamestrike;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_phoenix_tk";
    newscript->GetAI = &GetAI_mob_phoenix_tk;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_phoenix_egg_tk";
    newscript->GetAI = &GetAI_mob_phoenix_egg_tk;
    newscript->RegisterSelf();
}
