#ifndef SBO_HTML_FILTER_H_INCLUDED
#define SBO_HTML_FILTER_H_INCLUDED

#include <string>
#include "sbo_classlog.h"

using namespace std;

class sbo_html_filter_t :
  private sbo_classlog_t
{
public:
  sbo_html_filter_t();
  void set_filename(string const &pfilename);
  int setup(void);
  int dump(void);

private:
  string filename;
  string fltapp;
};

#endif
