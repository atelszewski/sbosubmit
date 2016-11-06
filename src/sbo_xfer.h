#ifndef SBO_XFER_H_INCLUDED
#define SBO_XFER_H_INCLUDED

#include <string>
#include "sbo_classlog.h"
#include "sbo_archive.h"
#include "sbo_comment.h"
#include "sbo_keywords.h"

class sbo_xfer_t :
  private sbo_classlog_t
{
  friend size_t curl_write_wrapper(char *, size_t, size_t, void *);

public:
  sbo_xfer_t();
  ~sbo_xfer_t();
  int upload(sbo_archive_t const &par);
  void set_url(string const &purl);
  string const &get_url(void) const;
  void set_category(string const &pcat);
  void set_tags(sbo_keywords_t const &pkeywords);
  void set_submail(string const &pemail);
  void set_comments(sbo_comment_t const &pcomment);

private:
  size_t curl_write(char const *pdata, size_t const psize);
  void fdclose(int &pfd);
  void path_unlink(string const &ppath);
  string url;
  string category;
  string tags;
  string submail;
  string comments;
  int tmpfd;
};

#endif
