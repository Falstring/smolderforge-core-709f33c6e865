-- Delete Blessing of Sanctuary rank 5, and both Greater ranks
DELETE FROM `spell_learn_spell` WHERE entry IN (20914,27168,25899,27169);
INSERT INTO `spell_learn_spell` VALUES
(27168,25899), -- Learn Greater rank 1 on rank 5 of normal blessing
(25899,27169); -- Learn Greater rank 2 on rank 1 of Greater blessing

-- Delete Prayer of Spirit ranks and Divine Spirit rank 5 entries
DELETE FROM `spell_learn_spell` WHERE entry IN (14752,27681,32999);
INSERT INTO `spell_learn_spell` VALUES
(25312,27681), -- Rank 5 learns Prayer of Spirit rank 1
(27681,32999); -- Prayer of Spirit rank 1 learns rank 2
