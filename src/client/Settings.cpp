
#include "Settings.h"
#include "common/output.h"

const auto config_filename = "keymapper.conf";

#if defined(_WIN32)

bool interpret_commandline(Settings& settings, int argc, wchar_t* argv[]) {
# define T(text) L##text
 
  settings.config_file_path = config_filename;

  for (auto i = 1; i < argc; i++) {
    const auto argument = std::wstring_view(argv[i]);

#else // !defined(_WIN32)

# include <unistd.h>
# include <pwd.h>

namespace {
  std::string get_home_directory() {
    if (auto homedir = ::getenv("HOME"))
      return homedir;
    return ::getpwuid(::getuid())->pw_dir;
  }
} // namespce

bool interpret_commandline(Settings& settings, int argc, char* argv[]) {
# define T(text) text

  settings.config_file_path = 
    get_home_directory() + "/.config/" + config_filename;

  for (auto i = 1; i < argc; i++) {
    const auto argument = std::string_view(argv[i]);
#endif // !defined(_WIN32)

    if (argument == T("-u") || argument == T("--update")) {
      settings.auto_update_config = true;
    }
    else if (argument == T("-c") || argument == T("--config")) {
      if (++i >= argc)
        return false;
      settings.config_file_path = argv[i];
    }
    else if (argument == T("-v") || argument == T("--verbose")) {
      settings.verbose = true;
    }
    else if (argument == T("--no-color")) {
      settings.no_color = true;
    }
    else if (argument == T("--check")) {
      settings.check_config = true;
    }
    else {
      return false;
    }
  }
  return true;
}

void print_help_message() {
  const auto version =
#if __has_include("../../_version.h")
# include "../../_version.h"
  " ";
#else
  "";
#endif

  message(
    "keymapper %s(c) 2019-%s by Albert Kalchmair\n"
    "\n"
    "Usage: keymapper [-options]\n"
    "  -c, --config <path>  configuration file.\n"
    "  -u, --update         reload configuration file when it changes.\n"
    "  -v, --verbose        enable verbose output.\n"
    "  --no-color           no color on error output.\n"
    "  --check              check the config for errors.\n"
    "  -h, --help           print this help.\n"
    "\n"
    "All Rights Reserved.\n"
    "This program comes with absolutely no warranty.\n"
    "See the GNU General Public License, version 3 for details.\n"
    "\n", version, (__DATE__) + 7);
}