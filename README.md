# SourceMod Blind Hook Extension
Extension adds forward for flashbang's blind control.  
Only for Counter-Strike: Global Offensive.
### Download
Use [releases page](https://github.com/Kailo97/sm-blindhook/releases) for getting latest version.
### Forward
```
/**
 * Called when a flashbang grenade perform player blinding.
 * Return Plugin_Continue to ignore or return a higher
 * action to protect player from blinding.
 *
 * @param client		Client index which must be blinded
 * @param attacker		Entity index of the grenade thrower or -1 if not available
 * @param inflictor		Entity index of the grenade projectile
 */
forward Action CS_OnBlindPlayer(int client, int attacker, int inflictor);
```
### Usage example
```
#include <blindhook>

public Action CS_OnBlindPlayer(int client, int attacker, int inflictor)
{
    // Don't blind yourself
    if (attacker == client)
        return Plugin_Stop;

    return Plugin_Continue;
}
```
