ALTER TABLE `creature_template` ADD COLUMN `spell5` MEDIUMINT(8) UNSIGNED NOT NULL DEFAULT '0' AFTER `spell4`;
ALTER TABLE `creature_template` ADD COLUMN `spell6` MEDIUMINT(8) UNSIGNED NOT NULL DEFAULT '0' AFTER `spell5`;
ALTER TABLE `creature_template` ADD COLUMN `spell7` MEDIUMINT(8) UNSIGNED NOT NULL DEFAULT '0' AFTER `spell6`;
ALTER TABLE `creature_template` ADD COLUMN `spell8` MEDIUMINT(8) UNSIGNED NOT NULL DEFAULT '0' AFTER `spell7`;
