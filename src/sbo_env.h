#ifndef SBO_ENV_H_INCLUDED
#define SBO_ENV_H_INCLUDED

#include <string>

using namespace std;

class sbo_env_t
{
public:
  sbo_env_t();
  string const sub_email;
  string const info_maint;
  string const info_email;

private:
  string getenv(char const *pname);
};

#endif
