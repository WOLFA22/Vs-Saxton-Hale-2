#if defined _vsh2_included
	#endinput
#endif
#define _vsh2_included

methodmap VSH2Player	
{
	/** [ C O N S T R U C T O R ]
	 * Constructs an instance of the BaseBoss internal methodmap
	 * @param index			index (or the userid) of a player
	 * @param userid		if using userid instead of player index, set this param to true
	 * @return			a player instance of the VSH2Player methodmap
	*/
	public native VSH2Player(const int index, bool userid=false);

	/* **** **** [ P R O P E R T I E S ] **** **** */
	
	/**
	 * gets the userid of the vsh2 player instance
	 * @return			the bare player userid integer
	*/
	property int userid {
		public native get();
	}
	
	/**
	 * gets the index of the vsh2 player instance
	 * @return			the bare player index integer
	*/
	property int index {
		public native get();
	}
	
	/**
         * sets the property of the internal VSH2 methodmap
	 * @param prop_name		name of property you want to access data from.
         * @return			the data of the property.		
        */
	public native any GetProperty(const char prop_name[64]);
	
	/**
         * sets the property of the internal VSH2 methodmap
	 * NOTE: You can use this to create new properties which GetProperty can access!
	 * @param prop_name		name of the property you want to override data from (works like StringMap).
	 * @param value			data you want the property to hold.
         * @noreturn
        */	
	public native void SetProperty(const char prop_name[64], any value);

	/*		AVAILABLE PROPERTIES
	property int iQueue
	property int iPresetType
	property int iKills
	property int iHits
	property int iLives
	property int iState
	property int iDamage
	property int iAirDamage
	property int iSongPick
	property int iHealTarget	// returns -1 is there's no valid player
	property int iOwnerBoss
	property int iUberTarget	// please use userid on this; convert to client index if you want but userid is safer
	property int bGlow		// use like a bool
	property bool bNearDispenser
	property bool bIsMinion
	property bool bInJump
	property float flGlowtime
	property float flLastHit
	property float flLastShot
	
	// 	ALL PROPERTIES AFTER THIS COMMENT ONLY ACCOUNT FOR BOSSES BUT CAN STILL APPLY ON NON-BOSSES AND MINIONS
	property int iHealth
	property int iMaxHealth
	property int iType
	property int iClimbs
	property int iStabbed
	property int iMarketted
	property int iDifficulty
	property bool bIsBoss
	property bool bSetOnSpawn
	property bool bUsedUltimate	// Not an overwatch reference lol; purpose is for a use-once ability but you can reset it
	property float flSpeed	// if you boss needs a set amount of speed, use this obviously
	property float flCharge
	property float flRAGE
	property float flKillSpree
	property float flWeighDown
	*/
		
	public native void ConvertToMinion(const float spawntime);
	public native int SpawnWeapon(char[] name, const int index, const int level, const int qual, char[] att);
	public native int GetWeaponSlotIndex(const int slot);
	public native void SetWepInvis(const int alpha);
	public native void SetOverlay(const char[] strOverlay);
	public native void TeleToSpawn(int team=0);
	public native void IncreaseHeadCount();
	public native void SpawnSmallHealthPack(int ownerteam=0);
	public native void ForceTeamChange(const int team);
	public native void ClimbWall(const int weapon, const float upwardVel, const float health, const bool attackdelay);
	public native void HelpPanelClass();
	
	// Boss oriented natives
	public native void ConvertToBoss();
	public native void GiveRage(const int damage);
	public native void MakeBossAndSwitch(const int type, const bool callEvent);
};


/**
 * Registers a plugin to be able to use VSH2's forwards
 * NOTE: The purpose of this native is to register boss subplugins, you don't need to register plugins to use forwards, simply hook what forward you need.
 * @param plugin_name			module name you want your calling plugin to be identified under
 * @return				integer of the plugin array index
 */
native int VSH2_RegisterPlugin(const char plugin_name[64]);


enum {		//VSH2HookType	
	OnCallDownloads=0,
	OnBossSelected,
	OnTouchPlayer,
	OnTouchBuilding,
	OnBossThink,
	OnBossModelTimer,
	OnBossDeath,
	OnBossEquipped,
	OnBossInitialized,
	OnMinionInitialized,
	OnBossPlayIntro,
	OnBossTakeDamage,
	OnBossDealDamage,
	OnPlayerKilled,
	OnPlayerAirblasted,
	OnTraceAttack,
	OnBossMedicCall,
	OnBossTaunt,
	OnBossKillBuilding,
	OnBossJarated,
	//OnHookSound,
	OnMessageIntro,
	OnBossPickUpItem,
	OnVariablesReset,
	OnUberDeployed,
	OnUberLoop,
	OnMusic,
	OnRoundEndInfo,
	OnLastPlayer,
	OnBossHealthCheck,
	OnControlPointCapped,
	OnBossMenu,
	OnPrepRedTeam,
	OnRedPlayerThink
};

/*
	IF YOU'RE USING THE HOOKING SYSTEM FOR A BOSS, YOU HAVE TO REGISTER YOUR PLUGIN WITH VSH2 BECAUSE YOU NEED THE MODULE INDEX TRACKED
*/

typeset VSH2HookCB {
	/*
		OnBossSelected
		OnBossThink
		OnBossModelTimer
		OnBossDeath
		OnBossEquipped
		OnBossInitialized
		OnMinionInitialized
		OnBossPlayIntro
		OnBossMedicCall
		OnBossTaunt
		OnVariablesReset
		OnBossHealthCheck
		OnPrepRedTeam
		OnRedPlayerThink
	*/
	function void (const VSH2Player Player);
	
	/*
		OnTouchPlayer
		OnBossJarated
		OnUberDeployed	// Victim is uber target, attacker is medic
		OnUberLoop	// Victim is uber target, attacker is medic
	*/
	function void (const VSH2Player Victim, const VSH2Player Attacker);
	
	//	OnTouchBuilding
	function void (const VSH2Player Attacker, const int BuildingRef);
	
	//	OnBossKillBuilding
	function void (const VSH2Player Attacker, const int building, Event event);
	
	//	OnBossTakeDamage , OnBossDealDamage
	function Action (VSH2Player Victim, int& attacker, int& inflictor, float& damage, int& damagetype, int& weapon, float damageForce[3], float damagePosition[3], int damagecustom);
	
	/*
		OnPlayerKilled
		OnPlayerAirblasted
	*/
	function void (const VSH2Player Player, const VSH2Player Victim, Event event);
	
	//	OnTraceAttack
	function void (const VSH2Player Victim, const VSH2Player attacker, int& inflictor, float& damage, int& damagetype, int& ammotype, int hitbox, int hitgroup);
	
	//	OnMessageIntro
	function void (ArrayList bossArray);
	
	//	OnMusic
	function void (char song[PLATFORM_MAX_PATH], float& time);
	
	//	OnRoundEndInfo
	function void (ArrayList bosses, bool bosswin);
	
	//	OnControlPointCapped
	function void (char cappers[MAXPLAYERS+1], const int team);
	
	//	OnCallDownloads , OnLastPlayer
	function void ();
	
	//	OnBossPickUpItem
	function void (const VSH2Player Player, const char item[64]);
	
	//	OnBossMenu
	function void (Menu& menu);
};

// Use hooktype enum on these.
native void VSH2_Hook(const int callbacktype, VSH2HookCB callback);
native bool VSH2_HookEx(const int callbacktype, VSH2HookCB callback);

native void VSH2_Unhook(const int callbacktype, VSH2HookCB callback);
native bool VSH2_UnhookEx(const int callbacktype, VSH2HookCB callback);


native any VSH2GameMode_GetProperty(const char prop_name[64]);
native void VSH2GameMode_SetProperty(const char prop_name[64], any value);
/*	Available properties
property int iRoundState
property int iSpecial
property int iTotalMaxHealth
property int iTimeLeft
property int iRoundCount
property int iHealthChecks
property int iCaptures
property bool bSteam		// Only useable if SteamTools is enabled and used.
property bool bTF2Attribs	// Only useable if TF2Attributes is enabled and used
property bool bPointReady
property bool bMedieval
property float flHealthTime
property float flMusicTime
property VSH2Player hNextBoss
*/

native VSH2Player VSH2GameMode_FindNextBoss();
native VSH2Player VSH2GameMode_GetRandomBoss(const bool IsAlive);
native VSH2Player VSH2GameMode_GetBossByType(const bool IsAlive, const int BossType);
native int VSH2GameMode_CountMinions(const bool IsAlive);
native int VSH2GameMode_CountBosses(const bool IsAlive);
native int VSH2GameMode_GetTotalBossHealth();
native void VSH2GameMode_SearchForItemPacks();
native void VSH2GameMode_UpdateBossHealth();
native void VSH2GameMode_GetBossType();
native int VSH2GameMode_GetTotalRedPlayers();


public SharedPlugin __pl_vsh2 = {
	name = "VSH2",
	file = "vsh2.smx",
#if defined REQUIRE_PLUGIN
	required = 1,
#else
	required = 0,
#endif
};

#if !defined REQUIRE_PLUGIN
public __pl_vsh2_SetNTVOptional()
{
	MarkNativeAsOptional("VSH2_RegisterPlugin");
	MarkNativeAsOptional("VSH2Player.VSH2Player");
	
	MarkNativeAsOptional("VSH2Player.userid.get");
	MarkNativeAsOptional("VSH2Player.index.get");
	
	MarkNativeAsOptional("VSH2Player.GetProperty");
	MarkNativeAsOptional("VSH2Player.SetProperty");
	
	MarkNativeAsOptional("VSH2Player.ConvertToMinion");
	MarkNativeAsOptional("VSH2Player.SpawnWeapon");
	MarkNativeAsOptional("VSH2Player.GetWeaponSlotIndex");
	MarkNativeAsOptional("VSH2Player.SetWepInvis");
	MarkNativeAsOptional("VSH2Player.SetOverlay");
	MarkNativeAsOptional("VSH2Player.TeleToSpawn");
	MarkNativeAsOptional("VSH2Player.IncreaseHeadCount");
	MarkNativeAsOptional("VSH2Player.SpawnSmallHealthPack");
	MarkNativeAsOptional("VSH2Player.ForceTeamChange");
	MarkNativeAsOptional("VSH2Player.ClimbWall");
	MarkNativeAsOptional("VSH2Player.HelpPanelClass");
	MarkNativeAsOptional("VSH2Player.ConvertToBoss");
	MarkNativeAsOptional("VSH2Player.GiveRage");
	MarkNativeAsOptional("VSH2Player.MakeBossAndSwitch");
	
	MarkNativeAsOptional("VSH2_Hook");
	MarkNativeAsOptional("VSH2_HookEx");
	MarkNativeAsOptional("VSH2_Unhook");
	MarkNativeAsOptional("VSH2_UnhookEx");
	
	MarkNativeAsOptional("VSH2GameMode_GetProperty");
	MarkNativeAsOptional("VSH2GameMode_SetProperty");
	MarkNativeAsOptional("VSH2GameMode_FindNextBoss");
	MarkNativeAsOptional("VSH2GameMode_GetRandomBoss");
	MarkNativeAsOptional("VSH2GameMode_GetBossByType");
	MarkNativeAsOptional("VSH2GameMode_CountMinions");
	MarkNativeAsOptional("VSH2GameMode_CountBosses");
	MarkNativeAsOptional("VSH2GameMode_GetTotalBossHealth");
	MarkNativeAsOptional("VSH2GameMode_SearchForItemPacks");
	MarkNativeAsOptional("VSH2GameMode_UpdateBossHealth");
	MarkNativeAsOptional("VSH2GameMode_GetBossType");
	MarkNativeAsOptional("VSH2GameMode_GetTotalRedPlayers");
}
#endif
