-- Trash Mob Add
SET @CGUID := 213212;
DELETE FROM `creature` WHERE `guid` BETWEEN @CGUID+0 AND @CGUID+12;
INSERT INTO `creature` (`guid`, `id`, `map`, `spawnMask`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `curhealth`, `MovementType`) VALUES
(@CGUID+8, 20880, 552, 3, 251.157898, 22.352457, 48.400421, 6.221873, 7200, 0, 39123, 0),
(@CGUID+9, 20879, 552, 3, 262.316040, 21.858221, 48.400421, 3.147021, 7200, 0, 39123, 0),
(@CGUID+10, 20879, 552, 3, 285.5186, 146.1547, 22.31179, 5.794493, 7200, 0, 39123, 0),
(@CGUID+11, 20880, 552, 3, 301.7973, 127.4436, 22.31079, 1.308997, 7200, 0, 39123, 0),
(@CGUID+12, 20880, 552, 3, 305.7355, 148.0587, 24.8633, 3.979351, 7200, 0, 39123, 0);

-- Pathing for Soul Devourer Entry: 20866
SET @NPC := 79477;
SET @PATH := @NPC * 10;
UPDATE `creature` SET `spawndist`=0,`MovementType`=2,`position_x`=207.7079,`position_y`=-129.0737,`position_z`=-10.10952 WHERE `guid`=@NPC;
DELETE FROM `creature_addon` WHERE `guid`=@NPC;
INSERT INTO `creature_addon` (`guid`,`path_id`,`bytes2`,`mount`,`auras`) VALUES (@NPC,@PATH,1,0, '');
DELETE FROM `waypoint_data` WHERE `id`=@PATH;
INSERT INTO `waypoint_data` (`id`,`point`,`position_x`,`position_y`,`position_z`,`delay`,`move_flag`,`action`,`action_chance`,`wpguid`) VALUES
(@PATH,1,207.7079,-129.0737,-10.10952,0,0,0,100,0),
(@PATH,2,245.4314,-128.669,-10.11772,0,0,0,100,0),
(@PATH,3,245.1088,-143.0256,-10.11027,0,0,0,100,0),
(@PATH,4,256.1706,-143.8266,-10.11091,0,0,0,100,0),
(@PATH,5,276.7943,-145.2905,-10.11652,0,0,0,100,0),
(@PATH,6,256.1706,-143.8266,-10.11091,0,0,0,100,0),
(@PATH,7,245.1088,-143.0256,-10.11027,0,0,0,100,0),
(@PATH,8,245.4314,-128.669,-10.11772,0,0,0,100,0),
(@PATH,9,207.7079,-129.0737,-10.10952,0,0,0,100,0),
(@PATH,10,221.3861,-128.9554,-10.11454,0,0,0,100,0);
-- 0xF130518200003BFB .go 207.7079 -129.0737 -10.10952

-- Pathing for Soul Devourer Entry: 20866
SET @NPC := 86053;
SET @PATH := @NPC * 10;
UPDATE `creature` SET `id`=20866,`curhealth`=1,`spawndist`=0,`MovementType`=2,`position_x`=221.8707,`position_y`=-152.5772,`position_z`=-10.11229 WHERE `guid`=@NPC;
DELETE FROM `creature_addon` WHERE `guid`=@NPC;
INSERT INTO `creature_addon` (`guid`,`path_id`,`bytes2`,`mount`,`auras`) VALUES (@NPC,@PATH,1,0, '');
DELETE FROM `waypoint_data` WHERE `id`=@PATH;
INSERT INTO `waypoint_data` (`id`,`point`,`position_x`,`position_y`,`position_z`,`delay`,`move_flag`,`action`,`action_chance`,`wpguid`) VALUES
(@PATH,1,221.8707,-152.5772,-10.11229,0,0,0,100,0),
(@PATH,2,237.1845,-152.618,-10.10515,0,0,0,100,0),
(@PATH,3,253.953,-152.8117,-10.1066,0,0,0,100,0),
(@PATH,4,253.2783,-175.503,-10.10356,0,0,0,100,0),
(@PATH,5,253.953,-152.8117,-10.1066,0,0,0,100,0),
(@PATH,6,237.1845,-152.618,-10.10515,0,0,0,100,0),
(@PATH,7,221.8707,-152.5772,-10.11229,0,0,0,100,0),
(@PATH,8,208.5352,-152.2789,-10.11248,0,0,0,100,0);
-- 0xF130518200003BFD .go 221.8707 -152.5772 -10.11229

-- Pathing for Gargantuan Abyssal Entry: 20898
SET @NPC := 79433;
SET @PATH := @NPC * 10;
UPDATE `creature` SET `spawndist`=0,`MovementType`=2,`position_x`=444.3863,`position_y`=-151.7787,`position_z`=43.03745 WHERE `guid`=@NPC;
DELETE FROM `creature_addon` WHERE `guid`=@NPC;
INSERT INTO `creature_addon` (`guid`,`path_id`,`bytes2`,`mount`,`auras`) VALUES (@NPC,@PATH,1,0, '');
DELETE FROM `waypoint_data` WHERE `id`=@PATH;
INSERT INTO `waypoint_data` (`id`,`point`,`position_x`,`position_y`,`position_z`,`delay`,`move_flag`,`action`,`action_chance`,`wpguid`) VALUES
(@PATH,1,444.3863,-151.7787,43.03745,0,0,0,100,0),
(@PATH,2,456.1953,-162.0229,43.09797,0,0,0,100,0),
(@PATH,3,444.3863,-151.7787,43.03745,0,0,0,100,0),
(@PATH,4,437.7717,-136.3138,43.10011,0,0,0,100,0),
(@PATH,5,441.6954,-123.0422,43.10011,0,0,0,100,0),
(@PATH,6,445.0636,-105.6565,43.10011,0,0,0,100,0),
(@PATH,7,446.4647,-89.31697,43.10009,0,0,0,100,0),
(@PATH,8,446.4129,-65.38671,48.39542,0,0,0,100,0),
(@PATH,9,446.4648,-89.31628,43.10009,0,0,0,100,0),
(@PATH,10,445.0636,-105.6565,43.10011,0,0,0,100,0),
(@PATH,11,441.6954,-123.0422,43.10011,0,0,0,100,0),
(@PATH,12,437.7717,-136.3138,43.10011,0,0,0,100,0);

-- Sentinel
DELETE FROM `creature` WHERE `guid` IN (86059, 86061, 86062, 86063, 86064);

SET @CGUID := 86054; -- 86059 to 86064 free
DELETE FROM `creature` WHERE `guid` BETWEEN @CGUID+0 AND @CGUID+4;
INSERT INTO `creature` (`guid`, `id`, `map`, `spawnMask`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `curhealth`, `MovementType`) VALUES
(@CGUID+0, 20869, 552, 3, 264.2865, -61.32112, 22.45335, 5.288348, 7200, 0, 115269, 0),
(@CGUID+1, 20869, 552, 3, 336.5143, 27.42666, 48.42604, 3.839724, 7200, 0, 115269, 0),
(@CGUID+2, 20869, 552, 3, 253.942, 131.8811, 22.39496, 0.7679449, 7200, 0, 115269, 0),
(@CGUID+3, 20869, 552, 3, 255.4978, 158.9143, 22.36194, 5.410521, 7200, 0, 115269, 0),
(@CGUID+4, 20869, 552, 3, 395.413, 18.19484, 48.29602, 2.495821, 7200, 0, 115269, 0);

-- Sentinel Spell Scripts
DELETE FROM `spell_script_target` WHERE `entry`=38829;
INSERT INTO `spell_script_target` (`entry`, `type`, `targetEntry`) VALUES (38829,1,0);

DELETE FROM `spell_script_target` WHERE `entry`=36717;
INSERT INTO `spell_script_target` (`entry`, `type`, `targetEntry`) VALUES (36717,1,0);