
It is important to acknowledge the contribution of

	Kevlin Henney
        kevlin@acm.org, kevlin@curbralan.com

to seeif.exe, which is a another diff, chiefly for comparing
Microsoft directories and files in straightforward command line
environment

seeif is als built for Linux and OSX

Directory operations in the Microsoft executable are
carried out with Mr Henney's dirent subroutines

seeif command line is

	seeif [-avVWRs] path path

All the options are single letter and can be in all
positions of the line, all at one if desired

paths are either two files or two directories

	-a	text, ignore stuff like CR

	-v	display differences

	-V	keep comparing after the first difference
		try to resync text files

	-W	treat whitespace fields as one space
		ignore blank lines

	-R	recurse subdirectories

	-s	say if files are equal


