#ifndef SBO_README_H_INCLUDED
#define SBO_README_H_INCLUDED

#include <string>
#include "sbo_subfile_base.h"

using namespace std;

class sbo_readme_t :
  public sbo_subfile_base_t
{
public:
  sbo_readme_t();
  void load(void);
};

#endif
