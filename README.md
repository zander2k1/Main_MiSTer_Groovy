# Main_MiSTer_Groovy

This is the MiSTer main program with [Groovy_MiSTer](https://github.com/psakhis/Groovy_MiSTer) support, kept up to date with the official [Main_MiSTer](https://github.com/MiSTer-devel/Main_MiSTer).

The Groovy core needs its own version of the main program (`MiSTer_groovy`), which receives video, audio and input over the network from programs like GroovyMAME, Groovy Dolphin and Groovy OpenMW. psakhis made it from the official program in 2024. He passed away later that year, so his version stopped receiving the official updates. This repo moves his Groovy changes onto the current official code.

## Download

**[MiSTer_groovy](https://github.com/zander2k1/Main_MiSTer_Groovy/releases/download/latest/MiSTer_groovy)** (built automatically from this repo), plus the optional **[groovy_wake](https://github.com/zander2k1/Main_MiSTer_Groovy/releases/download/latest/groovy_wake)** helper (see below)

## Install

1. Rename your current `/media/fat/MiSTer_groovy` to `MiSTer_groovy.old` as a backup.
2. Copy the new `MiSTer_groovy` to `/media/fat` (with WinSCP or FileZilla, use binary transfer mode).
3. Make sure `/media/fat/MiSTer.ini` still has these lines at the end:

   ```
   [Groovy]
   main=MiSTer_groovy
   ```

4. Start the Groovy core from `_Utility`. If anything goes wrong, rename the backup back to `MiSTer_groovy`.

This only affects the Groovy core. Every other core keeps using the normal MiSTer program.

The optional XDP network mode (`MiSTer_groovy_XDP`, which needs a patched Linux kernel) is still in the code but is not built yet.

## Start the Groovy core from the PC (optional)

`groovy_wake` is a small helper that lets Groovy Dolphin and Groovy OpenMW start the Groovy core for you. When they can't reach the MiSTer, they send a short "start Groovy" message to UDP port 32110. The helper then loads the newest `Groovy_*.rbf` in `/media/fat/_Utility`, the same way MiSTer scripts load cores. It only ever loads the Groovy core and ignores anything else.

1. Download **[groovy_wake](https://github.com/zander2k1/Main_MiSTer_Groovy/releases/download/latest/groovy_wake)** and copy it to `/media/fat/linux` (binary transfer mode).
2. Add this line to `/media/fat/linux/user-startup.sh` (create the file if it doesn't exist):

   ```
   /media/fat/linux/groovy_wake > /dev/null 2>&1 &
   ```

3. In a terminal on the MiSTer, run `chmod +x /media/fat/linux/groovy_wake`, then reboot the MiSTer.

If a different core is running when you start Groovy Dolphin or Groovy OpenMW, the MiSTer switches to the Groovy core.

## Credits

- psakhis (Sergi Clara) for Groovy_MiSTer and the Groovy support in this program
- The MiSTer developers for Main_MiSTer

Everything below is the official Main_MiSTer README.

---

# Main_MiSTer Main Binary and Wiki Repo

This repo serves as the home for the MiSTer Main binaries and the Wiki.

For the purposes of getting google to crawl the wiki, here's a link to the (not for humans) [crawlable wiki](https://github-wiki-see.page/m/MiSTer-devel/Wiki_MiSTer/wiki)

If you're a human looking for the wiki, that's [here](https://github.com/MiSTer-devel/Wiki_MiSTer/wiki)

To compile this application, read more about that [here](https://mister-devel.github.io/MkDocs_MiSTer/developer/mistercompile/#general-prerequisites-for-arm-cross-compiling)

This version has been modified to add support for MiSTer Floppy, SCP and IPF support - see https://mister.robsmithdev.co.uk