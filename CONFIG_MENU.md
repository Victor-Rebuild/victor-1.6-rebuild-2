# Configuration submenu information
1.6-rebuild has a submenu in CCIS for configuring bot settings and toggling some features on and off, this goes over them all.

The common options that appear on every page are:
```
EXIT
NEXT PAGE (If not on the last page)
PREV PAGE (If not on page 1)
```
These 3 options are pretty self explanitory.

### Do please note these are subject to change, I'm still updating rebuild and adding new things.

## CONFIGURATION PAGE 1
### SELF TEST
Anki's built in semi-auto self-test option. The self-test is very prone to erroring out so there is a doc with the mapped out error codes [here.](/docs/development/self-test-errors.md)

### CHANGE SLOT
Vector has 2 system slots, slot a and slot b. This option runs the `sysswitch` command which switches whatever the inactive slot is to the active one, and reboot the robot into the other system slot.

### WIREOS LIGHTS / ANKI LIGHTS
Toggles the WireOS backpack lights instead of the stock Anki or vice-versa ones if those are preferred. If custom backpack lights are in use this option will display `CUSTOM LIGHTS ON` instead and won't do anything.

## CONFIGURATION PAGE 2
### ENTER RECOVERY
Reboots Vector into recovery mode by running `/usr/sbin/reboot recovery`. Pretty simple.

### CHANGE PERF PROFILE
Vector has 3 performance profiles provided by Wired. `REGULAR`, `BALANCED`, and `PERFORMANCE`. This is a quick option that sends web requests to wired and tell it to change the performance profile if you don't have a device on hand or your network is too slow to load Wired.

## CONFIGURATION PAGE 3
### TOGGLE 30 FPS / TOGGLE 60 FPS
1.6-rebuild has Vector's refresh rate for the screen set to 60fps instead of the normal 30fps and with some of the workarounds I've done it works amazingly. But some people don't like 60fps or their bot is old enough that 60fps strains the battery too much causing shutdowns. This is a nice little toggle for those who want to go back to the classic Vector 30fps. When in 30fps mode the option switches to `TOGGLE 60 FPS` so that you know which mode Vector is currently running in.

## UPDATE SETTTINGS
Contains a few options related to updating Vector

- DISABLE UPDATING / ENABLE UPDATING
    - Disables all auto updates. If you're doing dev work for instance and your bot updates it would be pretty annoying, or if you'd rather delay the updates for some other time it'd be worth enabling this option. If auto updates are off this option will become `ENABLE UPDATING`.
- CHECK FOR UPDATES
    - It be what it's called

## CONFIGURATION PAGE 4
### SLEEP SETTINGS
Contains a few options for when it's nighttime and Vector is sleeping. (THESE ONLY APPLY WHEN IT'S NIGHT)

- DISABLE RTP / ENABLE RTP
    - In the night Vector will occasionally wake up and see if there's any people or enough light, if he sees either he'll sit awake for a bit before going back to sleep. This will toggle that.
- DISABLE RTS / ENABLE RTS
    - If Vector hears a really loud sound, even late at night he CAN wake up. This option makes it so that Vector ignores all sounds while he's asleep.
- DISABLE SNORING / ENABLE SNORING
    - Vector snores, even at night. Some people who keep Vector in the same room as them when they sleep have very much found this very annoying. This makes it so that he won't snore at night and sleap peacefully.

### DTTB RANDOM EYES
On every beat Vector will randomly change his eye color to one of the eye color presets he has, and reverts it once he's done dancing.

### USE BETA ALEXA / USE MODERN ALEXA
Old beta Alexa actually had it's voice run through the stock Vector voice filter instead of having it's own unfiltered channel. This restores that behavior.
