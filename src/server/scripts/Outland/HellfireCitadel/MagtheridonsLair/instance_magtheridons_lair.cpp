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
SDName: Instance_Magtheridons_Lair
SD%Complete: 100
SDComment:
SDCategory: Hellfire Citadel, Magtheridon's lair
EndScriptData */

#include "ScriptPCH.h"
#include "magtheridons_lair.h"

#define SPELL_SOUL_TRANSFER         30531 // core bug, does not support target 7
#define SPELL_BLAZE_TARGET          30541 // core bug, does not support target 7

#define CHAMBER_CENTER_X            -15.14f
#define CHAMBER_CENTER_Y              1.8f
#define CHAMBER_CENTER_Z             -0.4f

#define ENCOUNTERS 2

struct instance_magtheridons_lair : public ScriptedInstance
{
    instance_magtheridons_lair(Map *Map) : ScriptedInstance(Map)
    {
        Initialize();
    }

    uint32 Encounters[ENCOUNTERS];

    uint64 MagtheridonGUID;
    std::set<uint64> ChannelerGUID;
    uint64 DoorGUID;
    std::set<uint64> CubeGUID;
    std::set<uint64> ColumnGUID;

    uint32 CageTimer;
    uint32 CageWarningTimer;
    uint32 RespawnTimer;

    uint8 deadChannelers;

    void Initialize()
    {
        for (uint8 i = 0; i < ENCOUNTERS; i++)
            Encounters[i] = NOT_STARTED;

        MagtheridonGUID = 0;
        ChannelerGUID.clear();
        DoorGUID = 0;
        CubeGUID.clear();
        ColumnGUID.clear();

        CageTimer = 0;
        CageWarningTimer = 0;
        RespawnTimer = 0;

        deadChannelers = 0;
    }

    bool IsEncounterInProgress() const
    {
        for (uint8 i = 0; i < ENCOUNTERS; i++)
            if (Encounters[i] == IN_PROGRESS) return true;
        return false;
    }

    void OnCreatureCreate(Creature* creature)
    {
        switch (creature->GetEntry())
        {
        case 17257:
            MagtheridonGUID = creature->GetGUID();
            break;
        case 17256:
            ChannelerGUID.insert(creature->GetGUID());
            break;
        }
    }

    void OnGameObjectCreate(GameObject* pGo, bool /*add*/)
    {
        switch (pGo->GetEntry())
        {
        case 181713:
            pGo->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_NO_INTERACT);
            CubeGUID.insert(pGo->GetGUID());
            break;
        case 183847:
            DoorGUID = pGo->GetGUID();
            break;
        case 184653: // hall
        case 184634: // six columns
        case 184635:
        case 184636:
        case 184637:
        case 184638:
        case 184639:
            ColumnGUID.insert(pGo->GetGUID());
            break;
        }
    }

    uint64 GetData64(uint32 type)
    {
        switch (type)
        {
        case DATA_MAGTHERIDON:
            return MagtheridonGUID;
        }
        return 0;
    }

    void SetData(uint32 type, uint32 data)
    {
        switch (type)
        {
        case DATA_MAGTHERIDON_EVENT:
            Encounters[0] = data;
            if (data == NOT_STARTED)
            {
                RespawnTimer = 5000;

                for (std::set<uint64>::iterator i = CubeGUID.begin(); i != CubeGUID.end(); ++i)
                {
                    if (GameObject *cube = instance->GetGameObject(*i))
                    {
                        cube->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_NO_INTERACT);
                    }
                }
            }
            if (data == IN_PROGRESS)
            {
                for (std::set<uint64>::iterator i = CubeGUID.begin(); i != CubeGUID.end(); ++i)
                {
                    if (GameObject *cube = instance->GetGameObject(*i))
                    {
                        cube->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_NO_INTERACT);
                    }
                }
            }
            if (data == DONE)
            {
                if (GameObject *Door = instance->GetGameObject(DoorGUID))
                    Door->SetGoState(GO_STATE_ACTIVE);
            }
            break;

        case DATA_CHANNELER_EVENT:
            switch (data)
            {
            case NOT_STARTED: // Reset all channelers once one is reset.
            {
                if (Encounters[1] == NOT_STARTED)
                    return;

                Encounters[1] = NOT_STARTED;
                for (std::set<uint64>::iterator i = ChannelerGUID.begin(); i != ChannelerGUID.end(); ++i)
                {
                    if (Creature *Channeler = instance->GetCreature(*i))
                    {
                        if (Channeler->isAlive())
                            Channeler->AI()->EnterEvadeMode();
                        else
                        {
                            Channeler->Respawn();
                            Channeler->AI()->EnterEvadeMode();
                        }
                    }
                }

                CageTimer = 0;
                CageWarningTimer = 0;

                if (GameObject *Door = instance->GetGameObject(DoorGUID))
                    Door->SetGoState(GO_STATE_ACTIVE);
                break;
            }

            case IN_PROGRESS: // Event start.
            {
                if (Encounters[1] == IN_PROGRESS)
                    return;

                Encounters[1] = IN_PROGRESS;

                // Release Magtheridon after two minutes, send warning after one.
                Creature *Magtheridon = instance->GetCreature(MagtheridonGUID);
                if (Magtheridon && Magtheridon->isAlive())
                {
                    Magtheridon->TextEmote("'s bonds begin to weaken!", 0);
                    CageTimer = 120000;
                    CageWarningTimer = 60000;
                }
                if (GameObject *Door = instance->GetGameObject(DoorGUID))
                    Door->SetGoState(GO_STATE_READY);
                break;
            }
            case SPECIAL:
            {
                ++deadChannelers;
                if (deadChannelers >= 5)
                {
                    CageTimer = 0; // cancel cage
                    CageWarningTimer = 0; // cancel warning
                    if (Creature *Magtheridon = instance->GetCreature(MagtheridonGUID))
                        Magtheridon->AI()->EnterCombat(NULL); // will invoke DoFreed()

                    data = DONE;
                }
            }
            break;
            }

            Encounters[1] = data;
            break;

        case DATA_COLLAPSE:
            // true - collapse / false - reset
            for (std::set<uint64>::iterator i = ColumnGUID.begin(); i != ColumnGUID.end(); ++i)
                HandleGameObject(*i, data);
            break;

        default:
            break;
        }
    }

    uint32 GetData(uint32 type)
    {
        switch (type)
        {
            case DATA_MAGTHERIDON_EVENT: return Encounters[0];
            case DATA_CHANNELER_EVENT: return Encounters[1];
        }

        return 0;
    }

    void Update(uint32 diff)
    {
        if (CageTimer)
        {
            Creature *Magtheridon = instance->GetCreature(MagtheridonGUID);
            if (CageTimer <= diff)
            {
                if (Magtheridon && Magtheridon->isAlive())
                {
                    // triggers DoFreed()
                    Magtheridon->AI()->EnterCombat(NULL);
                }
                CageTimer = 0;
                deadChannelers = 0;
            }
            else
                CageTimer -= diff;
        }

        if (CageWarningTimer)
        {
            if (CageWarningTimer <= diff) // 1 min left warning
            {
                if (Creature *Magtheridon = instance->GetCreature(MagtheridonGUID))
                    Magtheridon->TextEmote("is nearly free of his bonds!", 0);

                CageWarningTimer = 0;
            }
            else
                CageWarningTimer -= diff;
        }

        if (RespawnTimer)
        {
            if (RespawnTimer <= diff)
            {
                for (std::set<uint64>::iterator i = ChannelerGUID.begin(); i != ChannelerGUID.end(); ++i)
                {
                    if (Creature *Channeler = instance->GetCreature(*i))
                    {
                        if (Channeler->isAlive())
                            Channeler->AI()->EnterEvadeMode();
                        else
                        {
                            Channeler->Respawn();
                            Channeler->AI()->EnterEvadeMode();
                        }
                    }
                }
                deadChannelers = 0;
                RespawnTimer = 0;
                if (Encounters[0] != DONE)
                    Encounters[0] = NOT_STARTED;
            }
            else
                RespawnTimer -= diff;
        }
    }
};

InstanceScript* GetInstanceData_instance_magtheridons_lair(Map* map)
{
    return new instance_magtheridons_lair(map);
}

void AddSC_instance_magtheridons_lair()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "instance_magtheridons_lair";
    newscript->GetInstanceScript = &GetInstanceData_instance_magtheridons_lair;
    newscript->RegisterSelf();
}

