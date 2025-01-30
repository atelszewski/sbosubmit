sbosubmit 1
===========

# NAME

**sbosubmit** - the SlackBuilds.org submission tool

# SYNOPSIS

**sbosubmit** \[**options**\]

# DESCRIPTION

**sbosubmit** is a tool to facilitate uploading of SlackBuilds to
SlackBuilds.org. It does it by performing checks of the files to be submitted,
packaging them and then uploading the resulting tarball to the SlackBuilds.org
server for later approval.

Using traditional methods, when you want to submit a SlackBuild,
you have to perform at least the following steps:

- very often you have to cleanup the SlackBuild directory from files used during
  development stage,
- create the archive of the files to be submitted,
- open web browser,
- attach the archive,
- fill in all the submission details,
- press the submit button,
- very often delete the created archive.

**sbosubmit** allows you to perform all these steps (and a bit more) by typing
a single command from within the SlackBuild directory. If you use the proposed
_tmpdir_ directory for additional development files (e.g. extracted source
tarball), then you don't have to cleanup anything before the submission:
_tmpdir_ is skipped when **sbosubmit** creates the submission archive.
Read on to learn more.

Before using the tool, familiarize yourself with the submission guidelines
available from https://slackbuilds.org/guidelines/. Keep in mind that this tool
has been created to facilitate the submission and not to make the life of
the SlackBuilds.org maintainers harder than it already is.
**ALWAYS DOUBLE CHECK** your submission before proceeding. Although the author
of the tool paid a lot of attention to code different checks of the files being
submitted, it is you who is responsible for making sure the files are
the correct ones.

For proper operation, **sbosubmit** expects certain directory structure to work
with, namely the one that is used by **sbopkg** and SlackBuilds.org itself.
This means that you have to keep the SlackBuilds organized in the
`CATEGORY/PRGNAM` fashion. Only the `CATEGORY/PRGNAM` component of the path is
important, other than that, your SlackBuilds can reside in whichever location
you want.

Example for submitting _libreoffice_ would be:

```
$ cd /path/to/slackbuilds/office/libreoffice
$ sbosubmit
```

Before uploading, **sbosubmit** performs basic checks on the following files:

- PRGNAM.SlackBuild
- slack−desc
- PRGNAM.info
- README

If present in the directory, the following files/directories won't be packaged
and uploaded:

- non−regular file except directory,
- empty directory,
- soft link,
- _tmpdir_ directory (or regular file),
- keywords.txt,
- comment.txt

_soft link_ can be used to point to the program source tarball, so that
the tarball itself does not have to reside in the same directory as
the SlackBuild.

_tmpdir_ can be used freely during SlackBuild development, for example to
extract the program source tarball for which the SlackBuild is being written.
On my system, the backup software is configured so that it excludes _tmpdir_
under my SlackBuilds repository path.

_keywords.txt_ can be used to keep the keywords for SlackBuild submission.
This file is only processed if the **--keywords** option is passed to sbosubmit.
Refer to **--keywords** option description for more information.

_comment.txt_ can be used to keep the comment for the SlackBuild submission.
This file is only processed if the **--comment** option is passed to sbosubmit.
Refer to **--comment** option description for more information.

# OPTIONS

**--address**=_ADDRESS_

Allows to specify alternative submission address, e.g. for testing purposes.
Default address is set to https://slackbuilds.org/

**--comment**_\[=COMMENT\]_

Allows to specify the comment for SlackBuild submission. If the _COMMENT_ string
is omitted, the comment is taken from the _comment.txt_ file residing in
the SlackBuild directory. You should only add the comment if there's something
important about the submission that the SlackBuilds.org maintainers should be
aware of, e.g. if you are taking over the maintenance of the SlackBuild from
someone who abandoned it.

**--email**=_EMAIL_

Allows to specify your e-mail address for submission. This is required in
the absence of the environment variable **SBOSUBMIT_SUBMISSION_EMAIL**.

**--help**

Display usage information and exit.

**--keywords**_\[=KEYWORDS\]_

Allows to specify the keywords for submission. If the _KEYWORDS_ string is
omitted, the keywords are taken from the _keywords.txt_ file residing in
the SlackBuild directory. Keywords are comma−separated words that help searchers
find your submission. Note:

- if you are submitting a version update, don't pass the **--keywords** option,
- if you wish to add a new keyword to the list, type only the new keyword (this
  applies both to the _keywords.txt_ file and the command line supplied
  keyword.)

**--verbose**

Be verbose (quiet by default.)

**--version**

Display version information and exit.

# FILE FORMATS

**COMMENT**

Comment file is simply a text file. The text contained within the comment file
is sent _as is_.

**KEYWORDS**

Keywords file is a text file, where the keywords are separated by comma.
The keywords can span multiple lines, but remember that new line does not
separate the individual keywords, only comma does.

# ENVIRONMENT

**SBOSUBMIT_SUBMISSION_EMAIL**

Your e-mail address as expected by SlackBuilds.org submission process.

**SBOSUBMIT_INFO_MAINTAINER**

The name that you put into _MAINTAINER_ field of the _.info_ file of
the submission. This value is only used to check if what you put into _.info_
file is correct - if this value is empty, the check is not performed.

**SBOSUBMIT_INFO_EMAIL**

The e−mail address that you put into _EMAIL_ field of the _.info_ file of
the submission (some people prefer to put obfuscated e-mail there, so it's
different from **SBOSUBMIT_SUBMISSION_EMAIL**, which has to be your real working
address.) This value is only used to check if what you put into _.info_ file is
correct - if this value is empty, the check is not performed.

# BUGS

- If the submission is interrupted during the transfer to the server,
  the temporary file _/tmp/sbosubmit.XXX_ is not being removed.

# AUTHOR

Andrzej Telszewski \<andrzej@telszewski.com\>

# SEE ALSO

**slackpkg**(8), **sbopkg**(8), **slackrepo**(8).
