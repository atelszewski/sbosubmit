#include <string>
#include <stdlib.h>
#include "sbo_env.h"

/******************************************************************************/
sbo_env_t::sbo_env_t() :
  sub_email( getenv("SBOSUBMIT_SUBMISSION_EMAIL")),
  info_maint(getenv("SBOSUBMIT_INFO_MAINTAINER")),
  info_email(getenv("SBOSUBMIT_INFO_EMAIL"))
/******************************************************************************/
{
}

/******************************************************************************/
string sbo_env_t::getenv(char const *pname)
/******************************************************************************/
{
  string env;
  char const *cenv;

  cenv = ::getenv(pname);
  if (cenv) env = cenv;

  return env;
}
