**Update 22/1/2024:** This project is heavily outdated. While technically changing serials, it will not have any effect on any modern anti-cheat. The only remotely useful part is the SMBIOS parsing, but keep in mind that I wrote this when I was 16 years old and it's not really handling all edge cases well. If you want to save yourself lots of work and hassle, just create those tables from scratch and replace them.

# mutante
Windows kernel-mode hardware identifier (HWID) spoofer. It does not use any hooking, so it can be completely unloaded after use. Tested on Windows 10 x64 2004 (19041.264).

## Features
- Disk serials (works on both SATA and NVMe drives)
- Disable S.M.A.R.T functionality
- SMBIOS (tables 0-3) modification (not zeroing)

## Credits
- Me (@SamuelTulach) - Putting it all together
- n0Lin (@Alex3434) - [Static disk spoofing without hooks](https://github.com/Alex3434/wmi-static-spoofer)
- IChooseYou - [Disable S.M.A.R.T functionality](https://www.unknowncheats.me/forum/2441916-post67.html) and [finding SMBIOS physical address](https://www.unknowncheats.me/forum/2436698-post9.html)
- btdt (@btdt) - [Finding SMBIOS physical address (again)](https://github.com/btbd/hwid/blob/master/Kernel/main.c#L558) and [signanture scanning functions](https://github.com/btbd/hwid/blob/master/Kernel/util.c#L112)
