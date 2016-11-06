#include <string>
#include <iostream>
#include <ostream>
#include "sbo_util.h"
#include "sbo_subfile_base.h"
#include "sbo_keywords.h"

/******************************************************************************/
ostream &operator<<(ostream &os, sbo_keywords_t const &cl)
/******************************************************************************/
{
  if (cl.keywords.length() == 0U)
  {
    return os << "none";
  }

  return os << cl.keywords;
}

/******************************************************************************/
sbo_keywords_t::sbo_keywords_t() :
  sbo_subfile_base_t("keywords")
/******************************************************************************/
{
}

/******************************************************************************/
void sbo_keywords_t::load(string const &pname)
/******************************************************************************/
{
  int res;
  size_t ifrom = 0U;
  size_t ito = 0U;
  string keywords_in;

  keywords.clear();

  if (pname.length() == 0U)
  {
    imsg(1) << "not loading." << endl;
    return;
  }

  if (pname == "@file")
  {
    imsg() << "loading from file..." << endl;
    res = file_into_string("keywords.txt", keywords_in);
    if (res != 0) return;
  }
  else
  {
    imsg() << "loading from command line..." << endl;
    keywords_in = pname;
  }

  whitespace_to_space(keywords_in);
  multispace_to_space(keywords_in);

  /* Find all the keywords, that is words beteen commas. */
  while (true)
  {
    string keyw;

    ifrom = keywords_in.find_first_not_of(",\f\n\r\t\v ", ito);
    if (ifrom == string::npos) break;
    ito = keywords_in.find_first_of(",", ifrom);
    keyw = keywords_in.substr(ifrom, ito - ifrom);
    trimlr(keyw);
    keywords += keyw + ", ";
    if (ito != string::npos) ito++;
  }

  /* Remove final ", ". */
  if (keywords.length() > 0U)
  {
    keywords = keywords.substr(0U, keywords.length() - 2U);
  }

  if (keywords.length() == 0U)
  {
    emsg() << "could not find any keywords." << endl;
    set_error();
  }
  else
  {
    imsg(1) << "loaded: " << keywords << endl;
  }
}
