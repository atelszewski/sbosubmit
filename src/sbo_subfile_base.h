#ifndef SBO_SUBFILE_BASE_H_INCLUDED
#define SBO_SUBFILE_BASE_H_INCLUDED

#include <string>
#include <vector>
#include "sbo_classlog.h"

using namespace std;

class sbo_subfile_base_t :
  protected sbo_classlog_t
{
public:
  bool is_error(bool const pprev = false) const;

protected:
  sbo_subfile_base_t(string const &pname);
  int file_into_string(string const &ppath, string &pstr);
  int file_into_array(string const &ppath, vector<string> &parray);
  void set_error(void);

private:
  bool error_flag;
};

#endif
