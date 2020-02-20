**sbosubmit** is a tool to facilitate uploading of SlackBuilds to SlackBuilds.org.
It does it by performing checks of the files to be submitted, packaging them
and then uploading the resulting tarball to the SlackBuilds.org server for
later approval.

For example, submitting *libreoffice* SlackBuild is as easy as::

  $ cd /path/to/slackbuilds/office/libreoffice
  $ sbosubmit

.. warning::

  sbosubmit as of now is still under first release candidate umbrella.
  You need to add *--address=sbo* command line option for
  the submission to be actually sent to the SlackBuilds.org server.

  Please make the submission only after you have ensured
  that your work is ready to be seen by the public.

Please consult the man page for more information.

**Happy Slacking!**
