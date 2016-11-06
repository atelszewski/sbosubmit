#ifndef SBO_COMMENT_H_INCLUDED
#define SBO_COMMENT_H_INCLUDED

#include <string>
#include <ostream>
#include "sbo_subfile_base.h"

using namespace std;

class sbo_comment_t;
ostream &operator<<(ostream &os, sbo_comment_t const &cl);

class sbo_comment_t :
  public sbo_subfile_base_t
{
  friend ostream &operator<<(ostream &, sbo_comment_t const &);
  friend class sbo_xfer_t;

public:
  sbo_comment_t();
  void load(string const &pname);

private:
  string comment;
};

#endif
