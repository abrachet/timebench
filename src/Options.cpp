/**
 * Alex Brachet (abrachet@cs.purdue.edu)
 */

#include "Options.h"
#include <boost/program_options.hpp>
#include <ostream>

#include <iostream>

namespace po = boost::program_options;

static po::options_description tbDescription("timebench usage");

TimebenchOptions parseTimeBench(int argc, const char** argv) {
  TimebenchOptions options;

  // clang-format off
  tbDescription.add_options()
    ("help", po::bool_switch(&options.help), "Display help message and exit.")
    ("version,V", po::bool_switch(&options.version),
      "Display verison information and exit.")
    ("output,o", po::value<std::string>(&options.output),
      "Send result to file not stderr.")
    ("append,a", po::bool_switch(&options.append),
      "Append to file specified by -o.")
    ("format,f", po::value<std::string>(&options.format),
      "Specify the output format.")
    ("warmup", po::value<unsigned>(&options.warmup)->default_value(0),
      "How many times to run the command before measurements.")
    ("runs,r", po::value<unsigned>(&options.runs)->default_value(1),
      "How many times to run the command.")
    ("average,A", po::bool_switch(&options.average),
      "Show average statistics, only use format string once in"
      "output, not [runs] times.")
    ("command", po::value<std::vector<std::string>>(&options.command),
      "Command to time.");
  // clang-format on

  po::positional_options_description p;
  p.add("command", -1);

  po::variables_map vm;
  po::parsed_options parsed = po::command_line_parser(argc, argv)
                                  .options(tbDescription)
                                  .positional(p)
                                  .allow_unregistered()
                                  .run();
  po::store(parsed, vm);
  po::notify(vm);

  options.unrecognized_options =
      po::collect_unrecognized(parsed.options, po::exclude_positional);

  return std::move(options);
}

void printHelp(std::ostream& out) { out << tbDescription; }
