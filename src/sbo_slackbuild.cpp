#include <string>
#include "sbo_subfile_base.h"
#include "sbo_slackbuild.h"

/******************************************************************************/
sbo_slackbuild_t::sbo_slackbuild_t() :
  sbo_subfile_base_t("SlackBuild")
/******************************************************************************/
{
}

/******************************************************************************/
void sbo_slackbuild_t::load(string const &pname)
/******************************************************************************/
{
  int res;

  content.clear();
  imsg(1) << "loading..." << endl;

  res = file_into_array(pname + ".SlackBuild", content);
  if (res != 0) return;
}
