#include <string>
#include "sbo_util.h"
#include "sbo_subfile_base.h"
#include "sbo_readme.h"

/******************************************************************************/
sbo_readme_t::sbo_readme_t() :
  sbo_subfile_base_t("README")
/******************************************************************************/
{
}

/******************************************************************************/
void sbo_readme_t::load(void)
/******************************************************************************/
{
  int res;
  string readme;

  imsg(1) << "loading..." << endl;
  res = file_into_string("README", readme);
  if (res != 0) return;

  if (is_all_whitespace(readme))
  {
    emsg() << "no text found." << endl;
    set_error();
    return;
  }
}
