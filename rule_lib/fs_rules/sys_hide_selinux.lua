-- Copyright (C) 2012 Nokia Corporation.
-- Licensed under MIT license.

-- Rule file interface version, mandatory.
--
fs_rule_lib_interface_version = "105"
----------------------------------

-- /sys rules which are needed for hiding selinux functionality:
-- If the host has selinux enabled, the programs running inside
-- SB2's sessions should not be able to detect that. Usually SB2
-- is configured to simulate a host without selinux functions.
-- For example, groupadd (or useradd) may fail if they think
-- that selinux is active but then don't have the necessary
-- privileges (as they don't; processes inside the sessions
-- may be running with minimal privileges within SB)

rules_sys = {

	{dir = "/sys/fs/selinux", map_to = target_root},

	-- The default: Use real /sys
	{prefix = "/sys", use_orig_path = true},
}

return rules_sys
