#ifndef SBO_FILELIST_H_INCLUDED
#define SBO_FILELIST_H_INCLUDED

#include <string>
#include <vector>
#include <sys/stat.h>
#include "sbo_classlog.h"

using namespace std;

class sbo_filelist_t :
  private sbo_classlog_t
{
  friend class sbo_archive_t;

public:
  typedef void (*file_callback_t)(string const &pname);

  sbo_filelist_t();
  int find(void);
  void sort(void);
  void set_file_cb(file_callback_t const pcb);

protected:
  typedef struct item_t
  {
    string name;
    struct stat stat;

    bool operator<(struct item_t const &pa) const
    {
      return name < pa.name;
    }
  } item_t;

  typedef vector<item_t> result_t;

  result_t result;

private:
  struct FTW
  {
    int base;
    int level;
  };

  struct history
  {
    struct history *chain;
    dev_t dev;
    ino_t ino;
    int level;
    int base;
  };

  int nftw(const char *ppath, int pfdlimit, int pflags);
  int nftw_do(char *ppath, int pfdlimit, int pflags, struct history *ph);
  int nftw_cb(const char * const pfpath, const struct stat * const psb,
              int const ptypeflag, struct FTW * const pftwbuf);
  bool check_filename(string const &pfn) const;
  file_callback_t file_callback;
};

#endif
