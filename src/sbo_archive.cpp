#include <string>
#include <vector>
#include <iostream>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <archive.h>
#include <archive_entry.h>
#include "sbo_util.h"
#include "sbo_classlog.h"
#include "sbo_filelist.h"
#include "sbo_archive.h"

// Buffer size for file reading.
static unsigned int const RBUF_SIZE = (64U * 1024U);

/******************************************************************************/
sbo_archive_t::sbo_archive_t() :
  sbo_classlog_t("archive"),
  data(nullptr), size(0U), add_file_cb(nullptr), list_file_cb(nullptr)
/******************************************************************************/
{
}

/******************************************************************************/
sbo_archive_t::~sbo_archive_t()
/******************************************************************************/
{
  free();
}

/******************************************************************************/
void sbo_archive_t::set_prgnam(string const &pprgnam)
/******************************************************************************/
{
  prgnam = pprgnam;

  if (prgnam.length() > 0U)
  {
    arname = prgnam + ".tar.gz";
  }
  else
  {
    arname = "";
  }
}

/******************************************************************************/
int sbo_archive_t::create(sbo_filelist_t const &pflst, size_t const pmaxsize)
/******************************************************************************/
{
  struct archive *ar = nullptr;
  int res = ARCHIVE_OK;
  archive_entry *en = nullptr;
  int rfd = -1;
  char rbuf[RBUF_SIZE];

  if (prgnam.length() == 0U)
  {
    emsg() << "PRGNAM must be set." << endl;
    return -1;
  }

  if (pflst.result.size() == 0U)
  {
    emsg() << "file list is empty." << endl;
    return -1;
  }

  if (size != 0U || data != nullptr)
  {
    emsg() << "previous archive not freed." << endl;
    return -1;
  }

  data = new(std::nothrow) char[pmaxsize];
  if (data == nullptr)
  {
    emsg() << "out of memory." << endl;
    goto failure;
  }

  ar = archive_write_new();
  if (ar == nullptr)
  {
    emsg() << "out of memory." << endl;
    goto failure;
  }

  if (res == ARCHIVE_OK) res = archive_write_add_filter_gzip(ar);
  if (res == ARCHIVE_OK) res = archive_write_set_format_pax_restricted(ar);
  if (res == ARCHIVE_OK) res = archive_write_set_format_ustar(ar);
  if (res == ARCHIVE_OK) res =
    archive_write_open_memory(ar, data, pmaxsize, &size);
  if (res != ARCHIVE_OK)
  {
    emsg_archive(ar);
    goto failure;
  }

  en = archive_entry_new();
  if (en == nullptr)
  {
    emsg() << "out of memory." << endl;
    goto failure;
  }

  for (auto item = pflst.result.begin(); item != pflst.result.end(); ++item)
  {
    string apath;
    string rpath;
    ssize_t rsize;

    apath = prgnam + "/" + item->name;
    rpath =                item->name;

    if (add_file_cb != nullptr) add_file_cb(rpath, apath);

    archive_entry_set_pathname(en, apath.c_str());
    archive_entry_copy_stat(en, &item->stat);

    res = archive_write_header(ar, en);
    if (res != ARCHIVE_OK)
    {
      emsg_archive(ar);
      goto failure;
    }

    rfd = open(rpath.c_str(), O_RDONLY);
    if (rfd == -1)
    {
      emsg() << "add: " << strerror(errno) << ": "
             << "'" << rpath << "'" << endl;
      goto failure;
    }

    rsize = read(rfd, rbuf, RBUF_SIZE);
    while (rsize > 0)
    {
      ssize_t wrote = 0;
      ssize_t wsize = rsize;

      while (wrote < rsize && wsize > 0)
      {
        wsize = archive_write_data(
          ar, &rbuf[wrote], static_cast<size_t>(rsize - wrote));
        wrote += wsize;
      }

      if (wsize == 0)
      {
        emsg() << "0 size write." << endl;
        goto failure;
      }

      if (wsize == -1)
      {
        emsg_archive(ar);
        goto failure;
      }

      rsize = read(rfd, rbuf, RBUF_SIZE);
    }

    if (rsize == -1)
    {
      emsg() << "add: " << strerror(errno) << ": "
             << "'" << rpath << "'" << endl;
      goto failure;
    }

    close(rfd);
    rfd = -1;
    archive_entry_clear(en);
  }

  archive_entry_free(en);
  en = nullptr;

  res = archive_write_free(ar);
  ar = nullptr;

  if (res != ARCHIVE_OK)
  {
    emsg() << "archive_write_free()" << endl;
    goto failure;
  }

  return 0;

failure:
  if (rfd != -1) close(rfd);
  if (en) archive_entry_free(en);
  if (ar) archive_write_free(ar);
  if (data) delete[] data;

  data = nullptr;
  size = 0U;

  return -1;
}

/******************************************************************************/
int sbo_archive_t::dump(string const &pname) const
/******************************************************************************/
{
  string name;
  int fd;
  ssize_t wsize = static_cast<ssize_t>(size);
  size_t wrote = 0U;

  if (pname.length() == 0U && arname.length() == 0U)
  {
    emsg() << "name is not set." << endl;
    return -1;
  }
  else
  {
    if (pname.length() > 0U)
    {
      name = pname;
    }
    else
    {
      name = arname;
    }
  }

  if (data == nullptr || size == 0U)
  {
    emsg() << "invalid data." << endl;
    return -1;
  }

  fd = open(name.c_str(), O_WRONLY | O_CREAT | O_TRUNC,
            S_IRUSR | S_IWUSR | S_IRWXG | S_IROTH);
  if (fd == -1)
  {
    emsg() << strerror(errno) << endl;
    return -1;
  }

  while (wrote < size && wsize > 0)
  {
    wsize = write(fd, &data[wrote], size - wrote);
    wrote += static_cast<size_t>(wsize);
  }

  if (wsize == 0)
  {
    emsg() << "0 size write" << endl;
    close(fd);
    return -1;
  }

  if (wsize == -1)
  {
    emsg() << strerror(errno) << endl;
    close(fd);
    return -1;
  }

  close(fd);

  return 0;
}

/******************************************************************************/
int sbo_archive_t::list(void) const
/******************************************************************************/
{
  struct archive *ar;
  struct archive_entry *en;
  int res = ARCHIVE_OK;

  if (data == nullptr || size == 0U)
  {
    return 0;
  }

  ar = archive_read_new();
  if (ar == nullptr)
  {
    emsg() << "out of memory." << endl;
    goto failure;
  }

  if (res == ARCHIVE_OK) res = archive_read_support_filter_all(ar);
  if (res == ARCHIVE_OK) res = archive_read_support_format_all(ar);
  if (res == ARCHIVE_OK) res = archive_read_open_memory(ar, data, size);
  if (res != ARCHIVE_OK)
  {
    emsg_archive(ar);
    goto failure;
  }

  while ((res = archive_read_next_header(ar, &en)) == ARCHIVE_OK)
  {
    char const *name;

    if ((name = archive_entry_pathname(en)) != nullptr)
    {
      if (list_file_cb)
      {
        list_file_cb(name, archive_entry_size(en));
      }
    }
    else
    {
      emsg_archive(ar);
      goto failure;
    }
  }

  if (res != ARCHIVE_EOF)
  {
    emsg_archive(ar);
    goto failure;
  }

  res = archive_read_free(ar);
  ar = nullptr;

  if (res != ARCHIVE_OK)
  {
    emsg() << "archive_read_free()" << endl;
    goto failure;
  }

  return 0;

failure:
  if (ar) archive_read_free(ar);
  return -1;
}

/******************************************************************************/
void sbo_archive_t::free(void)
/******************************************************************************/
{
  if (data != nullptr) delete[] data;
  data = nullptr;
  size = 0U;
}

/******************************************************************************/
size_t sbo_archive_t::get_size(void) const
/******************************************************************************/
{
  return size;
}

/******************************************************************************/
void sbo_archive_t::set_add_file_cb(add_file_callback_t const pcb)
/******************************************************************************/
{
  add_file_cb = pcb;
}

/******************************************************************************/
void sbo_archive_t::set_list_file_cb(list_file_callback_t const pcb)
/******************************************************************************/
{
  list_file_cb = pcb;
}

/******************************************************************************/
void sbo_archive_t::emsg_archive(struct archive *parchive) const
/******************************************************************************/
{
  char const *cmsg;
  string msg = "unknown error.";

  if (parchive)
  {
    cmsg = archive_error_string(parchive);
    if (cmsg) msg = cmsg;
  }

  emsg() << msg << endl;
}
