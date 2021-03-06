public char extensions[][] = { ".mdl", ".dx80.vtx", ".dx90.vtx", ".sw.vtx", ".vvd", ".phy" };
public char extensionsb[2][5] = { ".vtf", ".vmt" };

stock int GetHealingTarget(const int client)
{
	int medigun = GetPlayerWeaponSlot(client, TFWeaponSlot_Secondary);
	if (!IsValidEdict(medigun) || !IsValidEntity(medigun))
		return -1;
	char s[32]; GetEdictClassname(medigun, s, sizeof(s));
	if ( !strcmp(s, "tf_weapon_medigun", false) ) {
		if ( GetEntProp(medigun, Prop_Send, "m_bHealing") )
			return GetEntPropEnt( medigun, Prop_Send, "m_hHealingTarget" );
	}
	return -1;
}
stock int GetActiveWep(const int client)
{
	int weapon = GetEntPropEnt(client, Prop_Send, "m_hActiveWeapon");
	if (IsValidEdict(weapon) and IsValidEntity(weapon))
		return weapon;
	return -1;
}
stock float GetMediCharge(const int medigun)
{
	if (IsValidEdict(medigun) && IsValidEntity(medigun))
		return GetEntPropFloat(medigun, Prop_Send, "m_flChargeLevel");
	return -1.0;
}
stock void SetMediCharge(const int medigun, const float val)
{
	if (IsValidEdict(medigun) && IsValidEntity(medigun))
		SetEntPropFloat(medigun, Prop_Send, "m_flChargeLevel", val);
}
stock bool IsNearSpencer(const int client)
{
	int medics = 0;
	for (int i=MaxClients ; i ; --i) {
		if (IsValidClient(i) && GetHealingTarget(i) == client)
			medics++;
	}
	return (GetEntProp(client, Prop_Send, "m_nNumHealers") > medics);
}
//Prepare content stocks
stock void CheckDownload(const char[] dlpath)
{
	if ( FileExists(dlpath) )
		AddFileToDownloadsTable(dlpath);
}
//Following 'Prepare' stocks taken from VSH1
//'Prepare' stocks will precache AND add to the downloads table (if file exists).
stock void PrepareSound(const char[] szSoundPath) //For single custom sound files
{
	PrecacheSound(szSoundPath, true);
	char s[PLATFORM_MAX_PATH];
	Format(s, sizeof(s), "sound/%s", szSoundPath);
	CheckDownload(s);
}
stock void DownloadSoundList(const char[][] szFileList, int iSize) //For arrays of custom sounds
{
	for (int i = 0; i < iSize; i++)
	{
		PrepareSound(szFileList[i]);
	}
}
stock void PrecacheSoundList(const char[][] szFileList, int iSize) //For arrays of stock TF2 sounds
{
	for (int i = 0; i < iSize; i++)
	{
		PrecacheSound(szFileList[i], true);
	}
}
stock void PrepareMaterial(const char[] szMaterialPath) //For single custom materials, omit file extensions as it prepares VMT + VTF
{
	char s[PLATFORM_MAX_PATH];
	Format(s, sizeof(s), "%s%s", szMaterialPath, ".vtf");
	CheckDownload(s);
	Format(s, sizeof(s), "%s%s", szMaterialPath, ".vmt");
	CheckDownload(s);
}
stock void DownloadMaterialList(const char[][] szFileList, int iSize) //For arrays of custom materials
{
	char s[PLATFORM_MAX_PATH];
	for (int i = 0; i < iSize; i++)
	{
		strcopy(s, sizeof(s), szFileList[i]);
		CheckDownload(s);
	}
}
stock int PrepareModel(const char[] szModelPath, bool bMdlOnly = false) //For custom models, do NOT omit .MDL extension
{
	char szBase[PLATFORM_MAX_PATH];
	char szPath[PLATFORM_MAX_PATH];
	int i;
	strcopy(szBase, sizeof(szBase), szModelPath);
	SplitString(szBase, ".mdl", szBase, sizeof(szBase)); //Kind of redundant, but eh.
	
	if (!bMdlOnly)
	{
		for (i = 0; i < sizeof(extensions); i++)
		{
			Format(szPath, PLATFORM_MAX_PATH, "%s%s", szBase, extensions[i]);
			CheckDownload(szPath);
		}
	}
	else
		CheckDownload(szModelPath);
	
	return PrecacheModel(szModelPath, true);
}
//End 'Prepare' stocks
public bool TraceRayDontHitSelf(int entity, int mask, any data)
{
	return (entity != data);
}
stock void ForceTeamWin(const int team)
{
	int entity = FindEntityByClassname(-1, "team_control_point_master");
	if (entity <= 0) {
		entity = CreateEntityByName("team_control_point_master");
		DispatchSpawn(entity);
		AcceptEntityInput(entity, "Enable");
	}
	SetVariantInt(team);
	AcceptEntityInput(entity, "SetWinner");
}
stock int GetItemIndex(const int item)
{
	if (IsValidEdict(item) && IsValidEntity(item))
		return GetEntProp(item, Prop_Send, "m_iItemDefinitionIndex");
	return -1;
}
stock int GetItemQuality(const int item)
{
	if (IsValidEdict(item) && IsValidEntity(item))
		return GetEntProp(item, Prop_Send, "m_iEntityQuality");
	return -1;
}
stock int GetIndexOfWeaponSlot(const int client, const int slot)
{
	int weapon = GetPlayerWeaponSlot(client, slot);
	return GetItemIndex(weapon);
}
stock bool IsInRange(const int entity, const int target, const float dist, bool pTrace = false)
{
	float entitypos[3]; GetEntPropVector(entity, Prop_Data, "m_vecAbsOrigin", entitypos);
	float targetpos[3]; GetEntPropVector(target, Prop_Data, "m_vecAbsOrigin", targetpos);

	if ( GetVectorDistance(entitypos, targetpos) <= dist )
	{
		if (!pTrace)
			return true;
		else {
			TR_TraceRayFilter( entitypos, targetpos, MASK_SHOT, RayType_EndPoint, TraceRayDontHitSelf, entity );
			if ( TR_GetFraction() > 0.98 )
				return true;
		}
	}
	return false;
}
stock int AttachParticle(const int ent, const char[] particleType, float offset = 0.0, bool battach = true)
{
	int particle = CreateEntityByName("info_particle_system");
	char tName[32];
	float pos[3]; GetEntPropVector(ent, Prop_Send, "m_vecOrigin", pos);
	pos[2] += offset;
	TeleportEntity(particle, pos, NULL_VECTOR, NULL_VECTOR);
	Format(tName, sizeof(tName), "target%i", ent);
	DispatchKeyValue(ent, "targetname", tName);
	DispatchKeyValue(particle, "targetname", "tf2particle");
	DispatchKeyValue(particle, "parentname", tName);
	DispatchKeyValue(particle, "effect_name", particleType);
	DispatchSpawn(particle);
	SetVariantString(tName);
	if (battach) {
		AcceptEntityInput(particle, "SetParent", particle, particle, 0);
		SetEntPropEnt(particle, Prop_Send, "m_hOwnerEntity", ent);
	}
	ActivateEntity(particle);
	AcceptEntityInput(particle, "start");
	CreateTimer(3.0, RemoveEnt, EntIndexToEntRef(particle));
	return particle;
}
stock void CreateParticles(char[] particlename, float Pos[3] = NULL_VECTOR, float time)
{
	int particle = CreateEntityByName("info_particle_system");
	if (IsValidEdict(particle)) {
		DispatchKeyValue(particle, "effect_name", particlename);
		ActivateEntity(particle);
		AcceptEntityInput(particle, "start");
		TeleportEntity(particle, Pos, NULL_VECTOR, NULL_VECTOR);
		CreateTimer(time, RemoveEnt, EntIndexToEntRef(particle));
	}
	else LogError("CreateParticles: **** Couldn't Create 'info_particle_system Entity' ****");
}
stock void SetWeaponInvis(const int client, const int alpha)
{
	int transparent = alpha;
	for (int i = 0; i < 5; i++) {
		int entity = GetPlayerWeaponSlot(client, i); 
		if ( IsValidEdict(entity) && IsValidEntity(entity) )
		{
			if (transparent > 255)
				transparent = 255;
			if (transparent < 0)
				transparent = 0;
			SetEntityRenderMode(entity, RENDER_TRANSCOLOR); 
			SetEntityRenderColor(entity, 150, 150, 150, transparent); 
		}
	}
	return;
}
stock int SetWeaponAmmo(const int weapon, const int ammo)
{
	int owner = GetEntPropEnt(weapon, Prop_Send, "m_hOwnerEntity");
	if (owner <= 0)
		return 0;
	if (IsValidEntity(weapon)) {
		int iOffset = GetEntProp(weapon, Prop_Send, "m_iPrimaryAmmoType", 1)*4;
		int iAmmoTable = FindSendPropInfo("CTFPlayer", "m_iAmmo");
		SetEntData(owner, iAmmoTable+iOffset, ammo, 4, true);
	}
	return 0;
}
stock int GetWeaponAmmo(int weapon)
{
	int owner = GetOwner(weapon);
	if (owner <= 0)
		return 0;
	if (IsValidEntity(weapon)) {
		int iOffset = GetEntProp(weapon, Prop_Send, "m_iPrimaryAmmoType", 1)*4;
		int iAmmoTable = FindSendPropInfo("CTFPlayer", "m_iAmmo");
		return GetEntData(owner, iAmmoTable+iOffset, 4);
	}
	return 0;
}
stock int GetWeaponClip(const int weapon)
{
	if (IsValidEntity(weapon)) {
		int AmmoClipTable = FindSendPropInfo("CTFWeaponBase", "m_iClip1");
		return GetEntData(weapon, AmmoClipTable);
	}
	return 0;
}
stock int SetWeaponClip(const int weapon, const int ammo)
{
	if (IsValidEntity(weapon)) {
		int iAmmoTable = FindSendPropInfo("CTFWeaponBase", "m_iClip1");
		SetEntData(weapon, iAmmoTable, ammo, 4, true);
	}
	return 0;
}
stock int ShootRocket(const int client, bool bCrit = false, float vPosition[3], float vAngles[3], const float flSpeed, const float dmg, const char[] model, bool arc=false)
{
//new String:strEntname[45] = "tf_projectile_spellfireball";
/*
switch (spell)
	{
		case FIREBALL: 		strEntname = "tf_projectile_spellfireball";
		case LIGHTNING: 	strEntname = "tf_projectile_lightningorb";
		case PUMPKIN: 		strEntname = "tf_projectile_spellmirv";
		case PUMPKIN2: 		strEntname = "tf_projectile_spellpumpkin";
		case BATS: 			strEntname = "tf_projectile_spellbats";
		case METEOR: 		strEntname = "tf_projectile_spellmeteorshower";
		case TELE: 			strEntname = "tf_projectile_spelltransposeteleport";
		case BOSS:			strEntname = "tf_projectile_spellspawnboss";
		case ZOMBIEH:		strEntname = "tf_projectile_spellspawnhorde";
		case ZOMBIE:		strEntname = "tf_projectile_spellspawnzombie";
	}
	switch(spell)
	{
		//These spells have arcs.
		case BATS, METEOR, TELE:
		{
			vVelocity[2] += 32.0;
		}
	}

CTFGrenadePipebombProjectile m_bCritical
CTFProjectile_Rocket m_bCritical
CTFProjectile_SentryRocket m_bCritical
CTFWeaponBaseGrenadeProj m_bCritical
CTFMinigun m_bCritShot
CTFFlameThrower m_bCritFire
CTFProjectile_Syringe
CTFPlayer m_iCritMult
SetEntPropFloat(iProjectile, Prop_Send, "m_flDamage", dmg);
	}
*/
	int iTeam = GetClientTeam(client);
	int iProjectile = CreateEntityByName("tf_projectile_rocket");
	
	if (!IsValidEdict(iProjectile))
		return 0;

	float vVelocity[3];
	GetAngleVectors(vAngles, vVelocity, NULL_VECTOR, NULL_VECTOR);

	if (!arc)
		NormalizeVector(vVelocity, vVelocity);
	else
		vVelocity[2] -= 0.025;

	ScaleVector(vVelocity, flSpeed);
	SetEntPropEnt(iProjectile,	Prop_Send, "m_hOwnerEntity", client);
	SetEntProp(iProjectile,		Prop_Send, "m_bCritical", (bCrit ? 1 : 0));
	SetEntProp(iProjectile,		Prop_Send, "m_iTeamNum", iTeam, 1);
	SetEntProp(iProjectile,		Prop_Send, "m_nSkin", (iTeam-2));

	SetVariantInt(iTeam);
	AcceptEntityInput(iProjectile, "TeamNum", -1, -1, 0);
	SetVariantInt(iTeam);
	AcceptEntityInput(iProjectile, "SetTeam", -1, -1, 0);
	SetEntDataFloat(iProjectile, FindSendPropInfo("CTFProjectile_Rocket", "m_iDeflected")+4, dmg, true);

	TeleportEntity(iProjectile, vPosition, vAngles, vVelocity); 
	DispatchSpawn(iProjectile);
	if (arc)
		SetEntityMoveType(iProjectile, MOVETYPE_FLYGRAVITY);
	if ( model[0] != '\0' )
		SetEntityModel(iProjectile, model);
	return iProjectile;
}

stock void SetClientOverlay(const int client, const char[] strOverlay)
{
	int iFlags = GetCommandFlags("r_screenoverlay") & (~FCVAR_CHEAT);
	SetCommandFlags("r_screenoverlay", iFlags);
	ClientCommand(client, "r_screenoverlay \"%s\"", strOverlay);
}

stock bool IsValidClient(const int client, bool replaycheck = true)
{
	if (client <= 0 || client > MaxClients)
		return false;
	if (!IsClientInGame(client))
		return false;
	if (GetEntProp(client, Prop_Send, "m_bIsCoaching"))
		return false;
	if (replaycheck)
		if (IsClientSourceTV(client) || IsClientReplay(client))
			return false;
	if (TF2_GetPlayerClass(client) == TFClass_Unknown)
		return false;
	return true;
}
stock int GetOwner(const int ent)
{
	if ( IsValidEdict(ent) && IsValidEntity(ent) )
		return GetEntPropEnt(ent, Prop_Send, "m_hOwnerEntity");
	return -1;
}
stock int GetBuilder(const int ent)
{
	if ( IsValidEdict(ent) && IsValidEntity(ent) )
		return GetEntPropEnt(ent, Prop_Send, "m_hBuilder");
	return -1;
}
stock int GetGroundEntity(const int client)
{
	if ( IsValidClient(client, false)) )
		return GetEntPropEnt(client, Prop_Send, "m_hGroundEntity");
	return -1;
}
stock int FindTeleOwner(const int client)
{
	int teleporter = GetGroundEntity(client);
	char classname[32];
	if (IsValidEntity(teleporter) && GetEdictClassname(teleporter, classname, sizeof(classname)) && !strcmp(classname, "obj_teleporter", false))
	{
		int owner = GetBuilder(teleporter);
		if (IsValidClient(owner, false))
			return owner;
	}
	return -1;
}
stock void TeleportToSpawn(const int client, int team = 0)	// Props to Chdata!
{
	int iEnt = -1;
	float vPos[3], vAng[3];
	ArrayList hArray = new ArrayList();
	while ((iEnt = FindEntityByClassname(iEnt, "info_player_teamspawn")) != -1)
	{
		if (team <= 1)
			hArray.Push(iEnt);
		else {
			int iSpawnTeam = GetEntProp(iEnt, Prop_Send, "m_iTeamNum");
			if (iSpawnTeam == team)
				hArray.Push(iEnt);
		}
	}
	iEnt = hArray.Get(GetRandomInt(0, hArray.Length - 1));
	hArray.Close();

	// Technically you'll never find a map without a spawn point. Not a good map at least.
	GetEntPropVector(iEnt, Prop_Send, "m_vecOrigin", vPos);
	GetEntPropVector(iEnt, Prop_Send, "m_angRotation", vAng);
	TeleportEntity(client, vPos, vAng, nullvec);

	/*if (Special == VSHSpecial_HHH) //reserved for HHH boss
	{
		CreateTimer(3.0, RemoveEnt, EntIndexToEntRef(AttachParticle(iEnt, "ghost_appearation", _, false)));
		EmitSoundToAll("misc/halloween/spell_teleport.wav", _, _, SNDLEVEL_GUNFIRE, SND_NOFLAGS, SNDVOL_NORMAL, 100, _, vPos, nullvec, false, 0.0);
	}*/
}
stock void DoExplosion(const int owner, const int damage, const int radius, float pos[3])
{
	int explode = CreateEntityByName("env_explosion");
	if ( !IsValidEntity(explode) )
		return;

	DispatchKeyValue(explode, "targetname", "exploder");
	DispatchKeyValue(explode, "spawnflags", "4");
	DispatchKeyValue(explode, "rendermode", "5");

	SetEntPropEnt(explode, Prop_Data, "m_hOwnerEntity", owner);
	SetEntProp(explode, Prop_Data, "m_iMagnitude", damage);
	SetEntProp(explode, Prop_Data, "m_iRadiusOverride", radius);

	int team = GetClientTeam(owner);
	SetVariantInt(team); AcceptEntityInput(explode, "TeamNum");
	SetVariantInt(team); AcceptEntityInput(explode, "SetTeam");

	TeleportEntity(explode, pos, NULL_VECTOR, NULL_VECTOR);
	DispatchSpawn(explode);
	ActivateEntity(explode);
	AcceptEntityInput(explode, "Explode");
	AcceptEntityInput(explode, "Kill");
}

stock float[] nSubtractVectors(const float vec1[3], const float vec2[3])
{
	float result[3];
	result[0] = vec1[0] - vec2[0];
	result[1] = vec1[1] - vec2[1];
	result[2] = vec1[2] - vec2[2];
	return result;
}
stock float[] nAddVectors(const float vec1[3], const float vec2[3])
{
	float result[3];
	result[0] = vec1[0] + vec2[0];
	result[1] = vec1[1] + vec2[1];
	result[2] = vec1[2] + vec2[2];
	return result;
}
stock float[] nScaleVector(const float vec[3], const float scale)
{
	float result[3];
	result[0] = vec[0] * scale;
	result[1] = vec[1] * scale;
	result[2] = vec[2] * scale;
	return result;
}
stock float[] nNegateVector(const float vec[3])
{
	float result[3];
	result[0] = -vec[0];
	result[1] = -vec[1];
	result[2] = -vec[2];
	return result;
}
stock bool VectorCompare (const float v1[3], const float v2[3])
{
	for (int i=0 ; i<3 ; i++)
		if (v1[i] != v2[i])
			return false;
			
	return true;
}
stock bool OnlyScoutsLeft(const int team)
{
	for (int i=MaxClients ; i ; --i) {
		if ( ! IsValidClient(i) or ! IsPlayerAlive(i) )
			continue;
		if (GetClientTeam(i) == team and TF2_GetPlayerClass(i) != TFClass_Scout)
			return false;
	}
	return true;
}
stock int GetLivingPlayers(const int team)
{
	int AlivePlayers = 0;
	for (int i=MaxClients ; i ; --i) {
		if ( IsValidClient(i) && IsPlayerAlive(i) && GetClientTeam(i) == team )
			++AlivePlayers;
	}
	return AlivePlayers;
}
stock void IncrementHeadCount(const int client, bool addhealth = true, int addheads = 1)
{
	if ( (TF2_GetPlayerClass(client) == TFClass_DemoMan) and  ! TF2_IsPlayerInCondition(client, TFCond_DemoBuff) )
		TF2_AddCondition(client, TFCond_DemoBuff, TFCondDuration_Infinite); //Apply this condition to Demomen to give them their glowing eye effect.
	int decapitations = GetEntProp(client, Prop_Send, "m_iDecapitations");
	SetEntProp(client, Prop_Send, "m_iDecapitations", decapitations + addheads);
	if ( addhealth )
	{
		int health = GetClientHealth(client);
		//health += (decapitations >= 4 ? 10 : 15);
		if ( health + (15 * addheads) <= 300 ) //TODO: Replace this with an overheal calculation (MaxHP * 1.5) OR add a maxhealth arg. 
			health += 15 * addheads;
		else
			health = 300;
		SetEntProp(client, Prop_Data, "m_iHealth", health);
		SetEntProp(client, Prop_Send, "m_iHealth", health);
	}
	TF2_AddCondition(client, TFCond_SpeedBuffAlly, 0.01);   //recalc their speed
}
stock void SpawnSmallHealthPackAt(const int client, int ownerteam = 0)
{
	if (!IsValidClient(client) || !IsPlayerAlive(client))
		return;
	int healthpack = CreateEntityByName("item_healthkit_small");
	if ( IsValidEntity(healthpack) ) {
		float pos[3]; GetClientAbsOrigin(client, pos);
		pos[2] += 20.0;
		DispatchKeyValue(healthpack, "OnPlayerTouch", "!self,Kill,,0,-1");  //for safety, though it normally doesn't respawn
		DispatchSpawn(healthpack);
		SetEntProp(healthpack, Prop_Send, "m_iTeamNum", ownerteam, 4);
		SetEntityMoveType(healthpack, MOVETYPE_VPHYSICS);
		float vel[3];
		vel[0] = float(GetRandomInt(-10, 10)), vel[1] = float(GetRandomInt(-10, 10)), vel[2] = 50.0;
		TeleportEntity(healthpack, pos, NULL_VECTOR, vel);
		//CreateTimer(17.0, Timer_RemoveCandycaneHealthPack, EntIndexToEntRef(healthpack), TIMER_FLAG_NO_MAPCHANGE);
	}
	return;
}
public void SickleClimbWalls(const int client, const int weapon, const float upwardvel, const float health, const bool attackdelay)
//Credit to Mecha the Slag
{
	if ( !IsValidClient(client) || (GetClientHealth(client) <= health) )
		return;

	char classname[64];
	float vecClientEyePos[3];
	float vecClientEyeAng[3];
	GetClientEyePosition(client, vecClientEyePos);   // Get the position of the player's eyes
	GetClientEyeAngles(client, vecClientEyeAng);	   // Get the angle the player is looking

	//Check for colliding entities
	TR_TraceRayFilter(vecClientEyePos, vecClientEyeAng, MASK_PLAYERSOLID, RayType_Infinite, TraceRayDontHitSelf, client);

	if ( !TR_DidHit(null) )
		return;

	int TRIndex = TR_GetEntityIndex(null);
	GetEdictClassname(TRIndex, classname, sizeof(classname));
	if (!StrEqual(classname, "worldspawn"))
		return;

	float fNormal[3];
	TR_GetPlaneNormal(null, fNormal);
	GetVectorAngles(fNormal, fNormal);

	if (fNormal[0] >= 30.0 && fNormal[0] <= 330.0)
		return;
	if (fNormal[0] <= -30.0)
		return;

	float pos[3];
	TR_GetEndPosition(pos);
	float distance = GetVectorDistance(vecClientEyePos, pos);

	if (distance >= 100.0)
		return;

	float fVelocity[3];
	GetEntPropVector(client, Prop_Data, "m_vecVelocity", fVelocity);

	fVelocity[2] = upwardvel;

	TeleportEntity(client, NULL_VECTOR, NULL_VECTOR, fVelocity);

	SDKHooks_TakeDamage(client, client, client, health, DMG_CLUB, GetPlayerWeaponSlot(client, TFWeaponSlot_Melee));

	//if (!IsBoss[client])
	//	ClientCommand(client, "playgamesound \"%s\"", "player\\taunt_clip_spin.wav");

	if (attackdelay)
		SetPawnTimer(NoAttacking, 0.1, EntIndexToEntRef(weapon));
}
public void NoAttacking(const int wepref)
{
	int weapon = EntRefToEntIndex(wepref);
	SetNextAttack(weapon, 1.56);
}
stock void SetNextAttack(const int weapon, float duration = 0.0)
{
	if (weapon <= MaxClients || !IsValidEntity(weapon))
		return;
	float next = GetGameTime()+duration;
	SetEntPropFloat(weapon, Prop_Send, "m_flNextPrimaryAttack", next);
	SetEntPropFloat(weapon, Prop_Send, "m_flNextSecondaryAttack", next);
}
stock void ResetPlayer(const int client)
{
	TF2_RegeneratePlayer(client);
	SetEntityHealth( client, GetEntProp(client, Prop_Data, "m_iMaxHealth") );
	SetEntProp( client, Prop_Send, "m_iHealth", GetEntProp(client, Prop_Data, "m_iMaxHealth") );
}


/**
 * Wrapper function for easily setting up non-repeating timers
 *
 * @param func			Function pointer to call desired function when time elapses
 * @param thinktime		time in seconds when timer function will be called
 * @param param1		1st param for the call back function
 * @param param2		2nd param for the call back function
 *
 * @noreturn
 */

/*
If you need to use this and your function uses 3 parameters, modify it if necessary.
BUG/GLITCH: For some strange reason, SetPawnTimer doesn't work when u attempt to callback stock functions, interesting...
*/
stock void SetPawnTimer(Function func, float thinktime = 0.1, any param1 = -999, any param2 = -999)
{
	DataPack thinkpack = new DataPack();
	thinkpack.WriteFunction(func);
	thinkpack.WriteCell(param1);
	thinkpack.WriteCell(param2);

	CreateTimer(thinktime, DoThink, thinkpack, TIMER_DATA_HNDL_CLOSE);
}

public Action DoThink(Handle hTimer, DataPack hndl)
{
	hndl.Reset();

	Function pFunc = hndl.ReadFunction();
	Call_StartFunction( null, pFunc );

	any param1 = hndl.ReadCell();
	if ( param1 != -999 )
		Call_PushCell(param1);

	any param2 = hndl.ReadCell();
	if ( param2 != -999 )
		Call_PushCell(param2);

	Call_Finish();
	return Plugin_Continue;
}

stock bool DoSuperJump(const int client, const float charge, const float JumpAngle, int& changecharge, const int subtractCharge)
{
	float ang[3]; GetClientEyeAngles(client, ang);
	if ( ang[0] <= JumpAngle ) // default JumpAngle = -45.0
	{
		float vel[3]; GetEntPropVector(client, Prop_Data, "m_vecVelocity", vel);
		vel[2] = 750 + charge * 13.0;
		SetEntProp(client, Prop_Send, "m_bJumping", 1);
		vel[0] *= (1+Sine(charge * FLOAT_PI / 50));
		vel[1] *= (1+Sine(charge * FLOAT_PI / 50));
		TeleportEntity(client, nullvec, nullvec, vel);
		changecharge = subtractCharge;
		return true;
	}
	return false;
}


stock bool DoWeighdown(const int client, float& weighdowntime, const float AngleForWayDown, const float SetWeighdown)
{
	if ( weighdowntime >= 1.0 ) {
		float ang[3]; GetClientEyeAngles(client, ang);
		if ( ang[0] >= AngleForWayDown ) {	//default AngleForWayDown = 60.0
			float fVelocity[3];
			GetEntPropVector(client, Prop_Data, "m_vecVelocity", fVelocity);
			fVelocity[2] = -1000.0;
			TeleportEntity(client, nullvec, nullvec, fVelocity);
			SetEntityGravity(client, 6.0);
			CreateTimer(2.0, TimerGravityCat, GetClientUserId(client), TIMER_FLAG_NO_MAPCHANGE);
			weighdowntime = SetWeighdown;
			return true;
		}
	}
	return false;
}
stock void ForceClientTeamChange(const int client, const int iTeam)
{
	SetEntProp(client, Prop_Send, "m_lifeState", 2);
	ChangeClientTeam(client, iTeam);
	SetEntProp(client, Prop_Send, "m_lifeState", 0);
	TF2_RespawnPlayer(client);
}
stock int CalcBossHealth(const float initial, const int playing, const float subtract, const float exponent, const float additional)
{
	return RoundFloat( Pow((((initial)+playing)*(playing-subtract)), exponent)+additional );
}
stock bool IsVSHMap()	// just use arena maps as vsh/ff2 maps
{
	char config[FULLPATH], currentmap[99];
	GetCurrentMap(currentmap, sizeof(currentmap));
	if ( FileExists("bNextMapToFF2") )
		{return true;}
	else if ( FileExists("bNextMapToHale") )
		{return true;}

	BuildPath(Path_SM, config, FULLPATH, "configs/freak_fortress_2/maps.cfg");	// Compatibility :)
	if (! FileExists(config)) {
		BuildPath(Path_SM, config, FULLPATH, "configs/saxton_hale/saxton_hale_maps.cfg");
		if (! FileExists(config)) {
			LogError("[VSH 2] ERROR: **** Unable to find VSH/FF2 Compatibility Map Configs, Disabling VSH 2 ****");
			return false;
		}
	}

	File file = OpenFile(config, "r");
	if ( ! file ) {
		LogError("[VSH 2] **** Error Reading Maps from %s Config, Disabling VSH Engine ****", config);
		return false;
	}

	int tries;
	while ( file.ReadLine(config, sizeof(config)) and tries < 100 )
	{
		++tries;
		if (tries == 100) {
			LogError("[VSH 2] **** Breaking Loop Looking For a Map ****");
			return false;
		}

		Format(config, strlen(config)-1, config);
		if ( !strncmp(config, "//", 2, false) )
			continue;

		if ( StrContains(currentmap, config, false) != -1 || StrContains(config, "all", false) != -1 )
		{
			file.Close();
			return true;
		}
	}
	delete file;
	return false;
	//if (FindEntityByClassname(-1, "tf_logic_arena") != -1) return true;
	//return false;
}
stock void SetArenaCapEnableTime(const float time)
{
	int ent = -1;
	char strTime[32]; FloatToString(time, strTime, sizeof(strTime));
	if ( (ent = FindEntityByClassname(-1, "tf_logic_arena")) != -1 )
		DispatchKeyValue(ent, "CapEnableDelay", strTime);
}
stock void SetClientGlow(const int client, const float time1, float clampfl = 0.0, float &glowtimer)
{
	if (IsValidClient(client)) {
		glowtimer += time1;
		if (clampfl > 0.0)
			glowtimer = clampfl;
		if (glowtimer <= 0.0) {
			glowtimer = 0.0;
			SetEntProp(client, Prop_Send, "m_bGlowEnabled", 0);
		}
		else SetEntProp(client, Prop_Send, "m_bGlowEnabled", 1);
	}
}
stock void SetControlPoint(const bool enable)
{
	int CPm = -1;
	while ( (CPm = FindEntityByClassname(CPm, "team_control_point")) != -1 )
	{
		if (CPm > MaxClients and IsValidEdict(CPm)) {
			AcceptEntityInput(CPm, (enable ? "ShowModel" : "HideModel"));
			SetVariantInt(enable ? 0 : 1);
			AcceptEntityInput(CPm, "SetLocked");
		}
	}
}
public void EnableCap()
{
	SetControlPoint(true);
	int ent = -1;
	while ((ent = FindEntityByClassname(ent, "func_door")) != -1)
	{
		AcceptEntityInput(ent, "Open");
		AcceptEntityInput(ent, "Unlock");
		//AcceptEntityInput(ent, "Kill");
	}
	//if (doorchecktimer == null)
	//	doorchecktimer = CreateTimer(5.0, Timer_CheckDoors, _, TIMER_FLAG_NO_MAPCHANGE|TIMER_REPEAT);
}
stock void SpawnRandomAmmo()
{
	int iEnt = MaxClients+1;
	float vPos[3], vAng[3];
	int spawned;
	int maxlim = cvarVSH2[AmmoKitLimitMax].IntValue;
	int minlim = cvarVSH2[AmmoKitLimitMin].IntValue;
	while( (iEnt = FindEntityByClassname(iEnt, "info_player_teamspawn")) != -1 ) {
		if( minlim )
			if( spawned >= minlim )
				if( GetRandomInt(0, 3) )
					continue;
		if( maxlim )
			if( spawned >= maxlim )
				break;
		// Technically you'll never find a map without a spawn point.
		GetEntPropVector(iEnt, Prop_Send, "m_vecOrigin", vPos);
		GetEntPropVector(iEnt, Prop_Send, "m_angRotation", vAng);
		int ammo = CreateEntityByName("item_ammopack_small");
		TeleportEntity(ammo, vPos, vAng, NULL_VECTOR);
		DispatchSpawn(ammo);
		SetEntProp(ammo, Prop_Send, "m_iTeamNum", bEnabled.BoolValue ? 2 : 0, 4);
		++spawned;
	}
}
stock void SpawnRandomHealth()
{
	int iEnt = MaxClients+1;
	float vPos[3], vAng[3];
	int spawned;
	int maxlim = cvarVSH2[HealthKitLimitMax].IntValue;
	int minlim = cvarVSH2[HealthKitLimitMin].IntValue;
	while( (iEnt = FindEntityByClassname(iEnt, "info_player_teamspawn")) != -1 ) {
		if( minlim )
			if( spawned >= minlim )
				if( GetRandomInt(0, 3) )
					continue;
		if( maxlim )
			if( spawned >= maxlim )
				break;
		// Technically you'll never find a map without a spawn point.
		GetEntPropVector(iEnt, Prop_Send, "m_vecOrigin", vPos);
		GetEntPropVector(iEnt, Prop_Send, "m_angRotation", vAng);
		int healthkit = CreateEntityByName("item_healthkit_small");
		TeleportEntity(healthkit, vPos, vAng, NULL_VECTOR);
		DispatchSpawn(healthkit);
		SetEntProp(healthkit, Prop_Send, "m_iTeamNum", bEnabled.BoolValue ? 2 : 0, 4);
		++spawned;
	}
}
stock int GetTeamPlayerCount(const int team)
{
	int count = 0;
	for (int i=MaxClients ; i ; --i) {
		if (IsValidClient(i) and GetClientTeam(i) == team)
			{count++;}
	}
	return count;
}
stock int GetClientCloakIndex(const int client)
{
	if (! IsValidClient(client))
		return -1;
	int wep = GetPlayerWeaponSlot(client, 4);
	if (! IsValidEntity(wep))
		return -1;
	char classname[32];
	GetEntityClassname(wep, classname, sizeof(classname));
	if ( ! strncmp(classname, "tf_wea", 6, false) )
		return GetItemIndex(wep);
	return -1;
}
stock int GetSlotFromWeapon(const int iClient, const int iWeapon)
{
	for (int i=0; i<5; i++) {
		if ( iWeapon == GetPlayerWeaponSlot(iClient, i) )
			{return i;}
	}
	return -1;
}
stock bool RemoveDemoShield(int iClient) //Taken from VSH1
{
	int iEnt = MaxClients + 1;
	while ((iEnt = FindEntityByClassname(iEnt, "tf_wearable_demoshield")) != -1)
	{
		if (GetEntPropEnt(iEnt, Prop_Send, "m_hOwnerEntity") == iClient && !GetEntProp(iEnt, Prop_Send, "m_bDisguiseWearable"))
		{
			TF2_RemoveWearable(iClient, iEnt);
			return true; //Returns true if Shield was removed...
		}
	}
	return false; //...and returns False if one wasn't found.
}
stock void RemovePlayerBack(int client, int[] indices, int len)
{
	if (len <= 0)
		return;
	int edict = MaxClients+1;
	while ((edict = FindEntityByClassname(edict, "tf_wearable")) != -1)
	{
		char netclass[32];
		if (GetEntityNetClass(edict, netclass, sizeof(netclass)) and StrEqual(netclass, "CTFWearable"))
		{
			int idx = GetItemIndex(edict);
			if (GetOwner(edict) == client and ! GetEntProp(edict, Prop_Send, "m_bDisguiseWearable"))
			{
				for (int i = 0; i < len; i++) {
					if (idx == indices[i]) {
						TF2_RemoveWearable(client, edict);
						//AcceptEntityInput(edict, "Kill");
					}
				}
			}
		}
	}
	edict = MaxClients+1;
	while ((edict = FindEntityByClassname(edict, "tf_powerup_bottle")) != -1)
	{
		char netclass[32];
		if (GetEntityNetClass(edict, netclass, sizeof(netclass)) and StrEqual(netclass, "CTFPowerupBottle"))
		{
			int idx = GetItemIndex(edict);
			if (GetOwner(edict) == client and ! GetEntProp(edict, Prop_Send, "m_bDisguiseWearable"))
			{
				for (int i=0; i < len; i++) {
					if (idx == indices[i]) {
						TF2_RemoveWearable(client, edict);
						//AcceptEntityInput(edict, "Kill");
					}
				}
			}
		}
	}
	edict = MaxClients+1;
	while ((edict = FindEntityByClassname(edict, "tf_wearable_razorback")) != -1)
	{
		char netclass[32];
		if (GetEntityNetClass(edict, netclass, sizeof(netclass)) && StrEqual(netclass, "CTFWearableRazorback"))
		{
			int idx = GetItemIndex(edict);
			if (GetOwner(edict) == client && !GetEntProp(edict, Prop_Send, "m_bDisguiseWearable"))
			{
				for (int i = 0; i < len; i++) {
					if (idx == indices[i]) {
						TF2_RemoveWearable(client, edict);
						//AcceptEntityInput(edict, "Kill");
					}
				}
			}
		}
	}
}
stock int FindPlayerBack(int client, int[] indices, int len)
{
	if (len <= 0)
		return -1;
	int edict = MaxClients+1;
	while ((edict = FindEntityByClassname(edict, "tf_wearable")) != -1)
	{
		char netclass[32];
		if (GetEntityNetClass(edict, netclass, sizeof(netclass)) && StrEqual(netclass, "CTFWearable"))
		{
			int idx = GetItemIndex(edict);
			if (GetOwner(edict) == client && !GetEntProp(edict, Prop_Send, "m_bDisguiseWearable"))
			{
				for (int i = 0; i < len; i++) {
					if (idx == indices[i])
						return edict;
				}
			}
		}
	}
	edict = MaxClients+1;
	while ((edict = FindEntityByClassname(edict, "tf_powerup_bottle")) != -1)
	{
		char netclass[32];
		if (GetEntityNetClass(edict, netclass, sizeof(netclass)) && StrEqual(netclass, "CTFPowerupBottle"))
		{
			int idx = GetItemIndex(edict);
			if (GetOwner(edict) == client && !GetEntProp(edict, Prop_Send, "m_bDisguiseWearable"))
			{
				for (int i = 0; i < len; i++) {
					if (idx == indices[i])
						return edict;
				}
			}
		}
	}
	edict = MaxClients+1;
	while ((edict = FindEntityByClassname(edict, "tf_wearable_razorback")) != -1)
	{
		char netclass[32];
		if (GetEntityNetClass(edict, netclass, sizeof(netclass)) && StrEqual(netclass, "CTFWearableRazorback"))
		{
			int idx = GetItemIndex(edict);
			if (GetOwner(edict) == client && !GetEntProp(edict, Prop_Send, "m_bDisguiseWearable"))
			{
				for (int i = 0; i < len; i++) {
					if (idx == indices[i]) {
						return edict;
					}
				}
			}
		}
	}
	return -1;
}
stock int FindSentry(const int client)
{
	int i = -1;
	while ( (i = FindEntityByClassname(i, "obj_sentrygun")) != -1 )
		if (GetEntPropEnt(i, Prop_Send, "m_hBuilder") == client)
			return i;
	return -1;
}
stock bool TF2_IsPlayerCritBuffed(const int client)
{
	return (TF2_IsPlayerInCondition(client, TFCond_Kritzkrieged)
			or TF2_IsPlayerInCondition(client, TFCond_HalloweenCritCandy)
			or TF2_IsPlayerInCondition(client, view_as<TFCond>(34))
			or TF2_IsPlayerInCondition(client, view_as<TFCond>(35))
			or TF2_IsPlayerInCondition(client, TFCond_CritOnFirstBlood)
			or TF2_IsPlayerInCondition(client, TFCond_CritOnWin)
			or TF2_IsPlayerInCondition(client, TFCond_CritOnFlagCapture)
			or TF2_IsPlayerInCondition(client, TFCond_CritOnKill)
			or TF2_IsPlayerInCondition(client, TFCond_CritMmmph)
			);
}
stock void OverNineThousand()
{
	EmitSoundToAll("saxton_hale/9000.wav");
	EmitSoundToAll("saxton_hale/9000.wav");
}
public void _NoHonorBound(const int userid)
{
	int client = GetClientOfUserId(userid);
	if (IsValidClient(client) and IsPlayerAlive(client)) {
		int weapon = GetPlayerWeaponSlot(client, TFWeaponSlot_Melee);
		int index = GetItemIndex(weapon);
		int active = GetEntPropEnt(client, Prop_Send, "m_hActiveWeapon");
		char classname[32];
		if ( IsValidEdict(active) )
			GetEdictClassname(active, classname, sizeof(classname));
		if ( index == 357 and active == weapon and ! strcmp(classname, "tf_weapon_katana", false) )
		{
			SetEntProp(weapon, Prop_Send, "m_bIsBloody", 1);
			if (GetEntProp(client, Prop_Send, "m_iKillCountSinceLastDeploy") < 1)
				SetEntProp(client, Prop_Send, "m_iKillCountSinceLastDeploy", 1);
		}
	}
}
stock int GetMaxAmmo(const int client, const int slot)
{
	if (!IsValidClient(client))
		return 0;
	int weapon = GetPlayerWeaponSlot(client, slot);
	if (IsValidEntity(weapon))
	{
		switch (slot)
		{ //FindDataMapInfo(client, "m_iAmmo");
			case TFWeaponSlot_Primary: return GetEntData(client, FindDataMapInfo(client, "m_iAmmo")+4);
			case TFWeaponSlot_Secondary: return GetEntData(client, FindDataMapInfo(client, "m_iAmmo")+8);
			case TFWeaponSlot_Melee: return GetEntData(client, FindDataMapInfo(client, "m_iAmmo")+12);
		}
	}
	return 0;
}
stock TF2Item PrepareItemHandle(TF2Item hItem, char[] name = "", int index = -1, const char[] att = "", bool dontpreserve = false)
{
	static TF2Item hWeapon = null;
	int addattribs = 0;

	char weaponAttribsArray[32][32];
	int attribCount = ExplodeString(att, " ; ", weaponAttribsArray, 32, 32);

	int flags = OVERRIDE_ATTRIBUTES;
	if (!dontpreserve)
		flags |= PRESERVE_ATTRIBUTES;

	if ( !hWeapon )
		hWeapon = new TF2Item(flags);
	else hWeapon.iFlags = flags;
//	Handle hWeapon = TF2Items_CreateItem(flags);	//null;

	if (hItem != null) {
		addattribs = hItem.iNumAttribs;
		if (addattribs) {
			for (int i=0; i < 2*addattribs; i+=2) {
				bool dontAdd = false;
				int attribIndex = hItem.GetAttribID(i);
				for (int z=0; z < attribCount+i; z += 2) {
					if (StringToInt(weaponAttribsArray[z]) == attribIndex)
					{
						dontAdd = true;
						break;
					}
				}
				if (!dontAdd) {
					IntToString(attribIndex, weaponAttribsArray[i+attribCount], 32);
					FloatToString(hItem.GetAttribValue(i), weaponAttribsArray[i+1+attribCount], 32);
				}
			}
			attribCount += 2*addattribs;
		}
		delete hItem;
	}

	if (name[0] != '\0') {
		flags |= OVERRIDE_CLASSNAME;
		hWeapon.SetClassname(name);
	}
	if (index != -1) {
		flags |= OVERRIDE_ITEM_DEF;
		hWeapon.iItemIndex = index;
	}
	if (attribCount > 1) {
		hWeapon.iNumAttribs = (attribCount/2);
		int i2 = 0;
		for (int i=0; i<attribCount and i<32; i += 2)
		{
			hWeapon.SetAttribute(i2, StringToInt(weaponAttribsArray[i]), StringToFloat(weaponAttribsArray[i+1]));
			i2++;
		}
	}
	else hWeapon.iNumAttribs = 0;
	hWeapon.iFlags = flags;
	return hWeapon;
}
stock bool IsWeaponSlotActive(const int client, const int slot)
{
	return GetPlayerWeaponSlot(client, slot) == GetActiveWep(client);
}
public void _StopTickle(const int userid)
{
	int client = GetClientOfUserId(userid);
	if (!IsValidClient(client) || !IsPlayerAlive(client))
		return;
	if (!GetEntProp(client, Prop_Send, "m_bIsReadyToHighFive") && !IsValidEntity(GetEntPropEnt(client, Prop_Send, "m_hHighFivePartner")))
		TF2_RemoveCondition(client, TFCond_Taunting);
}
stock bool MapHasMusic(bool forceRecalc = false)	// Does this even work?
{
	static bool hasMusic;
	static bool found = false;
	if (forceRecalc) {
		found = false;
		hasMusic = false;
	}
	if (!found) {
		int i = -1;
		char name[64];
		while ((i = FindEntityByClassname(i, "info_target")) != -1)
		{
			GetEntPropString(i, Prop_Data, "m_iName", name, sizeof(name));
			if ( !strcmp(name, "hale_no_music", false) )
				hasMusic = true;
		}
		found = true;
	}
	return hasMusic;
}
stock bool IsValidAdmin(const int client, const char[] flags)
{
	if (!IsClientInGame(client))
		return false;
	int ibFlags = ReadFlagString(flags);
	if (!StrEqual(flags, "")) {
		if ( (GetUserFlagBits(client) & ibFlags) == ibFlags )
			return true;
	}
	if ( GetUserFlagBits(client) & ADMFLAG_ROOT )
		return true;
	return false;
}
stock int AttachProjectileModel(const int entity, const char[] strModel, char[] strAnim = "")
{
	if (!IsValidEntity(entity))
		return -1;
	int model = CreateEntityByName("prop_dynamic");
	if (IsValidEdict(model)) {
		float pos[3], ang[3];
		GetEntPropVector(entity, Prop_Send, "m_vecOrigin", pos);
		GetEntPropVector(entity, Prop_Send, "m_angRotation", ang);
		TeleportEntity(model, pos, ang, NULL_VECTOR);
		DispatchKeyValue(model, "model", strModel);
		DispatchSpawn(model);
		SetVariantString("!activator");
		AcceptEntityInput(model, "SetParent", entity, model, 0);
		if (strAnim[0] != '\0') {
			SetVariantString(strAnim);
			AcceptEntityInput(model, "SetDefaultAnimation");
			SetVariantString(strAnim);
			AcceptEntityInput(model, "SetAnimation");
		}
		SetEntPropEnt(model, Prop_Send, "m_hOwnerEntity", entity);
		return model;
	}
	else LogError("(AttachProjectileModel): Could not create prop_dynamic");
	return -1;
}
stock bool GetRandomBool()
{
	return ( !GetRandomInt(0, 1) ? false : true );
}
stock void SetAmmo(const int client, const int slot, const int ammo)
{
	int weapon = GetPlayerWeaponSlot(client, slot);
	if (IsValidEntity(weapon)) {
		int iOffset = GetEntProp(weapon, Prop_Send, "m_iPrimaryAmmoType", 1)*4;
		int iAmmoTable = FindSendPropInfo("CTFPlayer", "m_iAmmo");
		SetEntData(client, iAmmoTable+iOffset, ammo, 4, true);
	}
}
stock void SetClip(const int client, const int slot, const int ammo)
{
	int weapon = GetPlayerWeaponSlot(client, slot);
	if (IsValidEntity(weapon)) {
		int iAmmoTable = FindSendPropInfo("CTFWeaponBase", "m_iClip1");
		SetEntData(weapon, iAmmoTable, ammo, 4, true);
	}
}
stock int GetAmmo(const int client, const int slot)
{
	if (!IsValidClient(client))
		return 0;
	int weapon = GetPlayerWeaponSlot(client, slot);
	if (IsValidEntity(weapon)) {
		int iOffset = GetEntProp(weapon, Prop_Send, "m_iPrimaryAmmoType", 1)*4;
		int iAmmoTable = FindSendPropInfo("CTFPlayer", "m_iAmmo");
		return GetEntData(client, iAmmoTable+iOffset);
	}
	return 0;
}
stock int GetClip(const int client, const int slot)
{
	int weapon = GetPlayerWeaponSlot(client, slot);
	if (IsValidEntity(weapon)) {
		int AmmoClipTable = FindSendPropInfo("CTFWeaponBase", "m_iClip1");
		return GetEntData(weapon, AmmoClipTable);
	}
	return 0;
}
stock void SkipBossPanelNotify(const int client, bool newchoice = true)
{
	if (!bEnabled.BoolValue or !IsValidClient(client) or IsFakeClient(client) or IsVoteInProgress())
		return;

	Panel panel = new Panel();
	char strNotify[64];

	panel.SetTitle("[VSH2] You're The Next Boss!");
	Format(strNotify, sizeof(strNotify), "You are going to be a Boss soon! Type /halenext to check/reset your queue points.\nAlternatively, use !resetq.");

	panel.DrawItem(strNotify);
	panel.Send(client, SkipHalePanelH, 30);
	delete panel;
}
stock void _SetCapOwner(const int iCapTeam, float flEnableTime = 30.0)
{
	int i = -1;
	int cap_master = FindEntityByClassname(-1, "team_control_point_master");
	while ( (i = FindEntityByClassname(i, "team_control_point")) != -1 )
	{
		if (IsValidEntity(i)) {	//Adapted from Arena:Respawn
			SetVariantInt(iCapTeam);
			AcceptEntityInput(i, "SetOwner", -1, cap_master); //Must have team_control_point_master as the activator, less it will just ignore the Input
			SetVariantInt(1);
			AcceptEntityInput(i, "SetLocked");
			SetPawnTimer(EnableCap, flEnableTime);
		}
	}
}
stock bool StrStarts(const char[] szStr, const char[] szSubStr, bool bCaseSensitive = true)
{
	return !StrContains(szStr, szSubStr, bCaseSensitive);
}
stock bool ValidateName(const char[] name)
{
	int length = strlen(name);
	for (int i=0 ; i<length ; ++i) {
		int holder = name[i];
		// Invalid name, names may only contains numbers, underscores and normal letters
		if (!(IsCharAlpha(holder) || IsCharNumeric(holder) || holder == '_'))
			return false;
	}
	// A name is, of course, only valid if it's 1 or more chars long, though longer is recommended
	return (length > 0);
}
