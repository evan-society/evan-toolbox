The batch script 'copyDllsToEvan.bat' copies all libs to Evan's 'external' folder. It uses robocopy to do its job. Unluckily, according to
http://social.technet.microsoft.com/Forums/windowsserver/en-US/29936777-8171-4c2b-87d7-dc553c3c2108/are-any-of-the-tools-in-rktoolsexe-redistributable?forum=winservergen
it is not allowed to redistribute robocopy (Robust File Copy). If this program is not already installed on your computer, please download it here
(robocopy is part of 'Windows Server 2003 Resource Kit Tools')
http://www.microsoft.com/en-us/download/details.aspx?id=17657
All editions of Win 7 should have robocopy included already - future Windows version may ship it as well...

Before running 'copyDllsToEvan.bat', adjust all variables at the top of the script, since most probably the pathes will be different on your system!