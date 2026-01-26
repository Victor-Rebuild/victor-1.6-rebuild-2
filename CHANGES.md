# Changes from regular Anki 1.6

## Backported features from >1.6
- Add Vector 2.0 support (Firmware 2.0)
- Custom Eye Colors (Firmware 1.8)
- Intent graph backported from firmware 1.8
- Fixed occasional bouncy lift ([Anki commit](https://github.com/kercre123/victor/commit/54cfb37))
- Fixed self confirming fist bump ([Anki Commit](https://github.com/kercre123/victor/commit/2d5213e))
- Fixed path planning to stop head bobbing loop ([Anki Commit](https://github.com/kercre123/victor/commit/4110afc))
- Fixed going back to charger loop ([Anki Commit 1](https://github.com/kercre123/victor/commit/ac54369) [Anki Commit 2](https://github.com/kercre123/victor/commit/211c40d))

## Animation related changes
- BinaryEyes when leaving charger
- Good looking Vector 2.0 eyes (Used from WireOS + some tweaks from me)
- Smoother pre-1.6 eye darts (Last in 1.5, ported to Viccyware and used code from there)
- Added the previously unused second timer end beep animation
- Rainbow Eyes!
- Rebuild Eyes, default and can be set by asking Vector `Change eye color to Cross Media Bar` (Changes eye color on the same schedule as the PS3 XMB)!
- Petting Lights (Code from WireOS but petting colors are mine)
- Alt power on eye animations
- More expressive power on anims
- Face image overlays (Code used from WireOS)
- Toggle between Anki lights and wireOS lights! (Run `touch /data/data/wirelights` over ssh)
- Screen doesn't flash as much when booting up (ThommoMC's fix)
- Add back the "Pew" sound when petting ([Reverts this Anki Commit](https://github.com/kercre123/victor/commit/48344a779ad6be70e398b96f3c79db069263e8a1))
- Add old 1.5-era voice command response timing ([Reverts this anki commit](https://github.com/kercre123/victor/commit/6c3df37c6f3d929cc1562be0572185f10575858d))
- New onboarding animation

## Behavior related changes
- Unintentional and Intentional performances ([Anki Commit 1](https://github.com/kercre123/victor/commit/d3fa225) [Anki Commit 2](https://github.com/kercre123/victor/commit/2184b33))
- Can now play Blackjack on charger
- Now plays the wakeup after onboarding is finished from web setup (Code from Viccyware)
- Fixed Blackjack so that Vector actually says "Dealer" correctly
- Added the ability for Vector to say how old Vector is in years once 12 months passes
- Vector can be named in <vector-ip>:8080 in a web browser, name can be asked for by asking the "What is your name?" Voice command if you're connected to the custom server environment.
- Timer now works up to 1 day (WireOS commit)

## Cloud changes
- vic-cloud that works with wirepod and regular servers
- vic-gateway merged into cloud
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
- Gamma correction from WireOS