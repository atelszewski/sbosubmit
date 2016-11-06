#include <vector>
#include <string>
#include "sbo_category.h"

static vector<string> const category
{
  "Academic",
  "Accessibility",
  "Audio",
  "Business",
  "Desktop",
  "Development",
  "Games",
  "Gis",
  "Graphics",
  "Ham",
  "Haskell",
  "Libraries",
  "Misc",
  "Multimedia",
  "Network",
  "Office",
  "Perl",
  "Python",
  "Ruby",
  "System"
};

/******************************************************************************/
bool sbo_check_cat(string const &pcat)
/******************************************************************************/
{
  for (auto cat = category.begin(); cat != category.end(); ++cat)
  {
    if (*cat == pcat) return true;
  }

  return false;
}
