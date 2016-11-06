#ifndef SBO_CLASSLOG_H_INCLUDED
#define SBO_CLASSLOG_H_INCLUDED

#include <string>
#include <ostream>

using namespace std;

class sbo_classlog_t
{
protected:
  sbo_classlog_t(string const &pcname);
  ostream &imsg(int const vl = 0) const;
  ostream &wmsg(void) const;
  ostream &emsg(void) const;

private:
  string const class_name;
};

#endif
