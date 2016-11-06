#include <string>
#include "sbo_subfile_base.h"
#include "sbo_slack_desc.h"

/******************************************************************************/
sbo_slack_desc_t::sbo_slack_desc_t() :
  sbo_subfile_base_t("slack-desc")
/******************************************************************************/
{
}

/******************************************************************************/
void sbo_slack_desc_t::load(string const &pname)
/******************************************************************************/
{
  int res;

  (void)pname;

  content.clear();
  imsg(1) << "loading..." << endl;

  res = file_into_array("slack-desc", content);
  if (res != 0) return;
}
