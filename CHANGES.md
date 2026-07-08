# Changes from regular Anki 1.6

## Backported features from >1.6
- Add Vector 2.0 support (Firmware 2.0)
- Custom Eye Colors (Firmware 1.8)
- Intent graph backported from firmware 1.8
- Fixed occasional bouncy lift ([Anki commit](https://github.com/kercre123/victor/commit/54cfb37))
- Fixed self confirming fist bump ([Anki Commit](https://github.com/kercre123/victor/commit/2d5213e))
- Fixed path planning to stop head bobbing loop ([Anki Commit](https://github.com/kercre123/victor/commit/4110afc))
- Fixed going back to charger loop ([Anki Commit 1](https://github.com/kercre123/victor/commit/ac54369) [Anki Commit 2](https://github.com/kercre123/victor/commit/211c40d))
- SDK support in-line with firmware 2.0.1

## Animation related changes
- Vector runs at 60fps now (30 previously)
- BinaryEyes when leaving charger
- Good looking Vector 2.0 eyes (Used from WireOS + some tweaks from me)
- Smoother pre-1.6 eye darts (Last in 1.5, ported to Viccyware and used code from there)
- Added the previously unused second timer end beep animation
- Rainbow Eyes! (Ported from WireOS, can be set by asking Vector `Change eye color to Rainbow Eyes`)
- Rebuild Eyes! Default and can be set by asking Vector `Change eye color to Cross Media Bar` (Changes eye color on the same schedule as the PS3 XMB)!
- Mystery Eyes! (Random eye color every boot)
- Petting Lights (Code from WireOS but petting colors are mine)
- Alt power on eye animations
- More expressive power on anims
- Face image overlays (Code used from WireOS)
- Toggle between Anki lights and WireOS lights! (Selectable from the CCIS configuration menu)
- Custom backpack lights! (Following [this](https://github.com/os-vector/wire-os-victor/pull/30) PR)
- Screen doesn't flash as much when booting up (AmyMC's fix)
- New onboarding animation
- Less screen tearing on Vector 2.0
- New image for when Vector can't calibrate his gyro
- Bunch of old prototype anims have been reimplemented
- Unused sleep and low battery anim implmemented
- Now plays the wakeup after onboarding is finished from web setup (Code from Viccyware)

## Behavior related changes
- Unintentional and Intentional performances ([Anki Commit 1](https://github.com/kercre123/victor/commit/d3fa225) [Anki Commit 2](https://github.com/kercre123/victor/commit/2184b33))
- Vector can now either play blackjack on the charger or drive off if he wants to
- Add back the "Pew" sound when petting ([Reverts this Anki Commit](https://github.com/kercre123/victor/commit/48344a779ad6be70e398b96f3c79db069263e8a1))
- Add old 1.5-era voice command response timing ([Reverts this anki commit](https://github.com/kercre123/victor/commit/6c3df37c6f3d929cc1562be0572185f10575858d))
- Fixed Blackjack so that Vector actually says "Dealer" correctly
- Added the ability for Vector to say how old Vector is in years once 12 months passes
- Vector can be named in <vector-ip>:8080 in a web browser, name can be asked for by asking the "What is your name?" Voice command if you're connected to the custom server environment.
- Timer now works up to 1 day (WireOS commit)
- Can now ask Vector the date, date can be asked for by asking the "What is the date?" Voice command if you're connected to the custom server environment.
- More sensitive cliff detection
- ReactToHand drives faster and is more forgiving on the angle
- Cozmo cube support ported from Viccyware ([Thank you AmyMC](https://github.com/Victor-Rebuild/victor-1.6-rebuild-2/commit/137bd65e4aa70f498ed29fcabb8f3b4bed32c0d8))
- Prototype Eyes charger support ([Thank you AmyMC](https://github.com/Victor-Rebuild/victor-1.6-rebuild-2/commit/4316fa8b7ee710ca2540265a8e95821ece5f4674))
- Can mute Vector from Voice Command (Hey Vector, Volume Mute)
- Power Off/Reboot Voice Command

## Cloud changes
- vic-cloud that works with wirepod and regular servers from WireOS
- vic-gateway merged into cloud from WireOS
- New public server environment (Setup at https://anki2.ca/1.6/)

## Miscellaneous changes
- Improved Japanese, French and German TTS voices
- Proper translations for each language
- OpenCV 4.14 mainlined, newer than what's in WireOS (4.12)
- Upped temprature limit for Vector 2.0
- Support for DVT bodyboards
- Compiling with -O2 and fast math
- Picovoice 1.5 for customizable wakeword (Code used from WireOS)
- C++ upgrade engine from WireOS
- Reonboard menu to easily connect Vector to the voice command server
- Change slot option in CCIS to change Vector's system slot
- Useful configuration menu in CCIS
- Gamma correction and camera denoiser from WireOS
- Intent Graph optimizations
- Brightness changing on the fly to help battery life

## Configuration options added to CCIS
All of these are available under the `CONF` or `CONFIGURATION` menu in CCIS:
- Can swap system slot
- Enter Recovery mode
- Toggle WireOS lights
- Can change performance profile
- Toggle between 60/30fps
- Disable snoring
- Beta Alexa
- Dance To The Beat random eye colors