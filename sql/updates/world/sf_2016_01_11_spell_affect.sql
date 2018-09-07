# Remove Shiv and Gouge from Rouge's Shadowstep Damage Proc
# Remove all spells from effect 2, except Rupture and Garrote (prevents double damage)
UPDATE spell_affect SET SpellFamilyMask = 1125383275271 WHERE effectId=1 AND entry=36563;
UPDATE spell_affect SET SpellFamilyMask = 1048832 WHERE effectId=2 AND entry=36563;