#ifndef SBO_KEYWORDS_H_INCLUDED
#define SBO_KEYWORDS_H_INCLUDED

#include <string>
#include <ostream>
#include "sbo_subfile_base.h"

using namespace std;

class sbo_keywords_t;
ostream &operator<<(ostream &os, sbo_keywords_t const &cl);

class sbo_keywords_t :
  public sbo_subfile_base_t
{
  friend ostream &operator<<(ostream &, sbo_keywords_t const &);
  friend class sbo_xfer_t;

public:
  sbo_keywords_t();
  void load(string const &pname);

private:
  string keywords;
};

#endif
