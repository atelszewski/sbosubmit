#include <string>
#include <stdlib.h>
#include <unistd.h>
#include "sbo_html_filter.h"

/******************************************************************************/
sbo_html_filter_t::sbo_html_filter_t() :
  sbo_classlog_t("html")
/******************************************************************************/
{
}

/******************************************************************************/
void sbo_html_filter_t::set_filename(string const &pfilename)
/******************************************************************************/
{
  filename = pfilename;
}

/******************************************************************************/
int sbo_html_filter_t::setup(void)
/******************************************************************************/
{
  int rc = 0;

  if (access("/usr/bin/links", R_OK | X_OK) == 0)
  {
    fltapp = "/usr/bin/links";
  }
  else if (access("/usr/bin/lynx", R_OK | X_OK) == 0)
  {
    fltapp = "/usr/bin/lynx";
  }

  if (fltapp.length() == 0U)
  {
    wmsg() << "neither /usr/bin/links nor /usr/bin/lynx seems to be availabe."
           << endl;
    rc = -1;
  }
  else
  {
    imsg(1) << "html filter selected: " << fltapp << endl;

    if (fltapp == "/usr/bin/links")
    {
      fltapp += " -force-html -dump ";
    }
    else if (fltapp == "/usr/bin/lynx")
    {
      fltapp += " -force_html -dump ";
    }
  }

  return rc;
}

/******************************************************************************/
int sbo_html_filter_t::dump(void)
/******************************************************************************/
{
  int rc = 0;
  string cmdline;

  if (filename.length() == 0U)
  {
    emsg() << "filename is not set." << endl;
    rc = -1;
    goto doreturn;
  }

  if (access(filename.c_str(), R_OK) == -1)
  {
    emsg() << "file: '" << filename << "'' is not readable." << endl;
    rc = -1;
    goto doreturn;
  }

  cmdline = fltapp + filename;
  if ((rc = system(cmdline.c_str())) != 0)
  {
    rc = -1;
    goto doreturn;
  }

doreturn:
  return rc;
}
