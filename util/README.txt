
Freeware developer utilities published under the GNU Public Licence 3rd Version.
Text of the GPL v3 is supplied with this document under the filename gnu_gpl.txt 


These utilities are

        a number of simple utilities on osx.x86 osx.ppc ubuntu.x86 Windows32
        of which the most important are usea seeif and data

                usea =  another grep, importantly useable without much expertise
                        in Windows command prompt as also in Unix shells

                seeif = another diff, importantly useable in plain and basic
                        Windows command prompt as also in Unix shells. Optionally
                        recurses subdirectories

                data =  another DOS line editor, importantly useable without much
                        keyboarding in Windows command prompt as also in Unix shells

                        data is 100% text-command driven, and can be used from 
                        scripts for gang-updates. Used in bshell / MS prompt

                fds =   a datafile hex / character viewer used in bshell / MS prompt

                flo     displays IEEE754 64-bit floating values in hex notation
                        for floating library developers, used in bshell / MS prompt

                csum    generates and displays expected IP / TCP / UDP checksums
                        for PDUs represented in text tokens of 4 hex symbols representing
                        16-bit words. For network integrators checking implementations
                        of protocol stacks. Used in bshell / MS prompt

                stripcr reads a text file as stdin and writes it as stdout
                        discarding CR. MS stdout will of course reintroduce CRs
                        so stripcr is mostly for tidying textfiles which are
                        being migrated from MS to Unix. ftp ASCII mode does the
                        same thing when sending from MS to Unix. Used in bshell / MS prompt
			There are dozens of other utilities which do the same thing

                stuffcr reads a text file as stdin and writes it as stdout, adding
                        CR before unaccompanied LF. ftp ASCII mode does the same
                        thing when sending from Unix to MS. Used in bshell / MS prompt
			There are dozens of other utilties which do the same thing




__________________________________________________
It is important to acknowledge the contribution of
__________________________________________________

	Kevlin Henney
        kevlin@acm.org, kevlin@curbralan.com

to seeif.exe, which is a another diff, chiefly for comparing
Microsoft directories and files in Microsoft command prompt
simplest version with no special environment

Mr Henney's dirent routines allow applications like seeif
to read Microsoft directories much as POSIX platforms allow

seeif is also built for Linux and OSX. diff is already very
good on any Unix platform, but seeif optionally recurses any
depth of directories

Directory operations in the Microsoft executable are
carried out with Mr Henney's dirent subroutines

seeif command line is

	seeif [-avVWRs] path path

All the options are single letter and can be in all
positions of the line, all at once if desired

paths are either two files or two directories

	-a	text, ignore stuff like CR

	-v	display differences

	-V	keep comparing after the first difference
		try to resync text files

	-W	treat whitespace fields as one space
		ignore blank lines

	-R	recurse subdirectories

	-s	say if files are equal


