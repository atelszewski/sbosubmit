#ifndef SBO_ARCHIVE_H_INCLUDED
#define SBO_ARCHIVE_H_INCLUDED

#include <string>
#include <stddef.h>
#include <archive.h>
#include <archive_entry.h>
#include "sbo_classlog.h"
#include "sbo_filelist.h"

using namespace std;

class sbo_archive_t :
  private sbo_classlog_t
{
  friend class sbo_xfer_t;

public:
  typedef void (*add_file_callback_t)
    (string const &piname, string const &poname);
  typedef void (*list_file_callback_t)
    (string const &pname, int64_t const psize);

  sbo_archive_t();
  ~sbo_archive_t();
  void set_prgnam(string const &pprgnam);
  int create(sbo_filelist_t const &pflst, size_t const pmaxsize = 1048576U);
  int dump(string const &pname = "") const;
  int list(void) const;
  void free(void);
  size_t get_size(void) const;
  void set_add_file_cb(add_file_callback_t const pcb);
  void set_list_file_cb(list_file_callback_t const pcb);

private:
  void emsg_archive(struct archive *parchive) const;
  char *data;
  size_t size;
  string prgnam;
  string arname;
  add_file_callback_t add_file_cb;
  list_file_callback_t list_file_cb;
};

#endif
