// wdkg 2008-2010

#include <cstdio>
#include <cstring>
#include "ew/Form3.h"
#include "ew/Version.h"
#include "../src/File.h"
#include "FilterForm3.h"

namespace {
  const char Author[] = "wdkg";
  const char Name[] = "filter_form3";
  const char Options[] = "c";
  const char Help[] =
   "Rewrite form3 xml files.\n"
   "Usage:\n"
   "  filter_form3 [-c] <infile> <outfile>\n"
   "Options:\n"
   "  -c  compress output\n"
   "The file names must be canonical\n";
}

int
ewtst::FilterForm3::exec(int ac, char **av)
{
  if (ac == 2 && std::strcmp(av[1], "--version") == 0) {
    std::printf("%s %s %s\n", Name, Author, ew::Version::version_str());
    return 0;
  }
  if (ac == 2 &&
   (std::strcmp(av[1], "-h") == 0 || std::strcmp(av[1], "--help") == 0)) {
// Cannot use a const char * for the format because it disables gcc type safety.
    std::printf("%s", Help);
    return 0;
  }
  bool option_ons[128];
  const char *option_vals[128];
  for (int i = 0; i < 128; i += 1) {
    option_ons[i] = false;
    option_vals[i] = 0;
  }
  while (ac > 1 && av[1][0] == '-' && av[1][1] != 0) {
    ac -= 1;
    av += 1;
    if (av[0][1] == '-' && av[0][2] == 0) {
      break;
    }
    const char *p = av[0] + 1;
    while (p[0] != 0) {
      bool got_option = false;
      for (int i = 0; Options[i] != 0; i += 1) {
        if (p[0] == Options[i] && Options[i] != '_') {
          option_ons[p[0]] = true;
          if (Options[i + 1] == '_') {
            const char *val;
            if (p[1] != 0) {
              val = p + 1;
              while (p[0] != 0) {
                p += 1;
              }
            } else {
              if (ac <= 1) {
                std::fprintf(stderr, "%s: fatal error, bad usage\n", Name);
                return 1;
              }
              val = av[1];
              ac -= 1;
              av += 1;
            }
            option_vals[p[0]] = val;
          }
          p += 1;
          got_option = true;
          break;
        }
      }
      if (!got_option) {
        std::fprintf(stderr, "%s: fatal error, bad usage\n", Name);
        return 1;
      }
    }
  }
  ac -= 1;
  av += 1;
  if (ac != 2) {
    std::fprintf(stderr, "%s: fatal error, bad usage\n", Name);
    return 1;
  }
  if (ew::File::pathlen(av[0]) == 0 || ew::File::pathlen(av[1]) == 0) {
    std::fprintf(stderr, "%s: fatal error, non-canonical paths\n", Name);
    return 1;
  }
  try {
    ew::Form3 form;
    form.read_file(av[0]);
    form.write_file(av[1], option_ons['c']);
  } catch (std::logic_error &e) {
    std::fprintf(stderr, "%s: fatal error, logic_error %s\n", Name, e.what());
    return 1;
  } catch (std::runtime_error &e) {
    std::fprintf(stderr, "%s: fatal error, runtime_error %s\n", Name, e.what());
    return 1;
  } catch (std::exception &e) {
    std::fprintf(stderr, "%s: fatal error, std::exception %s\n", Name,
     e.what());
    return 1;
  } catch (...) {
    std::fprintf(stderr, "%s: fatal error, unknown exception\n", Name);
    return 1;
  }
  return 0;
}
