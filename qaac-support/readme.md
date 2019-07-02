qaac requires CoreAudioToolbox.dll.   THis is part of iTunes (ugh)  and/or QuickTime (yuck).

Turns out it's embedded in an installer *wihtin* the iTunes installer, which I've extracted as described here: 

        Quote from: cjc1103 on 2018-12-28 22:56:13
        Anyone have any idea how to install the 64 bit version of QuickTime support files (including CoreAudioToobox.dll) on Windows..?
        1: Download iTunes https://www.apple.com/itunes/download/win64
        2: Open iTunes64Setup.exe (use 7-zip > Open Archive)
        3: Find AppleApplicationSupport64.msi, extract it somewhere and install it.
        4: Done.

(source: https://hydrogenaud.io/index.php/topic,117089.0.html)