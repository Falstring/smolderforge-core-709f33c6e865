#Updates Bloodthirst and Mortal Strike Discount SpellFamilyMask
select * from spell_affect where entry = 37535;
UPDATE spell_affect SET SpellFamilyMask = 4398080065536 WHERE entry=37535;