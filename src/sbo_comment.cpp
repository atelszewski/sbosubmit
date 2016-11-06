#include <string>
#include <iostream>
#include <ostream>
#include "sbo_util.h"
#include "sbo_subfile_base.h"
#include "sbo_comment.h"

/******************************************************************************/
ostream &operator<<(ostream &os, sbo_comment_t const &cl)
/******************************************************************************/
{
  if (cl.comment.length() == 0U)
  {
    return os << "none";
  }

  return os << "\n" << cl.comment;
}

/******************************************************************************/
sbo_comment_t::sbo_comment_t() :
  sbo_subfile_base_t("comment")
/******************************************************************************/
{
}

/******************************************************************************/
void sbo_comment_t::load(string const &pname)
/******************************************************************************/
{
  int res;

  comment.clear();

  if (pname.length() == 0U)
  {
    imsg(1) << "not loading." << endl;
    return;
  }

  if (pname == "@file")
  {
    imsg() << "loading from file..." << endl;
    res = file_into_string("comment.txt", comment);
    if (res != 0) return;
  }
  else
  {
    imsg() << "loading from command line..." << endl;
    comment = pname;
  }

  if (is_all_whitespace(comment))
  {
    emsg() << "no text found." << endl;
    set_error();
    return;
  }

  trimr(comment);

  if (get_verbosity() > 0)
  {
    string icomment;

    icomment = comment;
    trimlr(icomment);
    whitespace_to_space(icomment);
    multispace_to_space(icomment);
    imsg() << "loaded: " << icomment << endl;
  }
}
