/*
 * main.cpp
 *
 * Main entry point for the simulation, reads the CLI and starts the simulation.
 */
#include <fmt/format.h>

#include <args.hxx>
#include <iostream>
#include <thread>

#include "easylogging++.h"
/* #include "error_handler.hxx" */
#include "Helpers/DbLoader.hxx"
#include "Helpers/OSHelpers.h"
#include "Model.h"
#include "Reporters/Reporter.h"

// Version information, be sure to update the file for major builds
#include "version.h"

// NOLINTBEGIN(cert-err58-cpp) / Assume that the logger will work correctly
INITIALIZE_EASYLOGGINGPP
// NOLINTEND(cert-err58-cpp)

void handle_cli(Model* model, int argc, char** argv, int &job_number,
                std::string &path);

void config_logger() {
  const std::string OUTPUT_FORMAT = "[%level] [%func] [%loc] %msg";

  // Set global logging flags
  el::Loggers::addFlag(el::LoggingFlag::DisableApplicationAbortOnFatalLog);

  // Create the default configuration
  el::Configurations default_conf;
  default_conf.setToDefault();
  // default_conf.set(el::Level::Global, el::ConfigurationType::Enabled,
  // "false");
  default_conf.set(el::Level::Debug, el::ConfigurationType::Format,
                   OUTPUT_FORMAT);
  default_conf.set(el::Level::Error, el::ConfigurationType::Format,
                   OUTPUT_FORMAT);
  default_conf.set(el::Level::Fatal, el::ConfigurationType::Format,
                   OUTPUT_FORMAT);
  default_conf.set(el::Level::Trace, el::ConfigurationType::Format,
                   OUTPUT_FORMAT);
  default_conf.set(el::Level::Info, el::ConfigurationType::Format,
                   "[%level] %msg");
  default_conf.set(el::Level::Warning, el::ConfigurationType::Format,
                   "[%level] %msg");
  default_conf.set(el::Level::Verbose, el::ConfigurationType::Format,
                   "[%level-%vlevel] %msg");
  default_conf.setGlobally(el::ConfigurationType::ToFile, "false");
  default_conf.setGlobally(el::ConfigurationType::ToStandardOutput, "true");
  default_conf.setGlobally(el::ConfigurationType::LogFlushThreshold, "100");
  el::Loggers::reconfigureLogger("default", default_conf);
}

int main(const int argc, char** argv) {
  // Parse the CLI
  int job_number = 0;
  std::string path;
  auto* m = new Model();
  handle_cli(m, argc, argv, job_number, path);

  // Prepare the logger
  config_logger();
  START_EASYLOGGINGPP(argc, argv);
  LOG(INFO) << fmt::format("MaSim version {0}", VERSION);
  VLOG(1) << "Processor Count: " << std::thread::hardware_concurrency();
  VLOG(1) << "Physical: " << OsHelpers::getPhysicalMemoryUsed() << " Kb";
  VLOG(1) << "Virtual: " << OsHelpers::getVirtualMemoryUsed() << " Kb";

  // Run the model
  m->initialize(job_number, path);
  m->run();

  // Report final memory usage
  LOG(INFO) << fmt::format("Memory used: {0} Kb physical, {1} Kb virtual",
                           OsHelpers::getPhysicalMemoryUsed(),
                           OsHelpers::getVirtualMemoryUsed());

  // Clean-up and return
  delete m;
  exit(EXIT_SUCCESS);
}

void handle_cli(Model* model, int argc, char** argv, int &job_number,
                std::string &path) {
  /* QUICK REFERENCE
   * -c / --config - config file
   * -h / --help   - help screen
   * -i / --input  - input file
   * -j            - cluster job number
   * -l / --load   - load genotypes and exit
   * -o            - path for output files
   * -r            - reporter type
   * -s            - study to associate with the configuration, database id
   *
   * --dump        - dump the movement matrix as calculated
   * --lr          - list the possible reporters
   * --lg          - list the possible genotypes and their ids
   * --im          - record individual movement data
   * --mc          - record the movement between cells
   * --md          - record the movement between districts
   */
  // NOLINTBEGIN(cppcoreguidelines-slicing) / Disable the slicing check while we
  // define the argument parser
  args::ArgumentParser parser("Individual-based simulation for malaria.",
                              "Boni Lab at Temple University");
  args::Group commands(parser, "commands");
  args::HelpFlag help(commands, "help", "Display this help menu",
                      {'h', "help"});
  args::ValueFlag<std::string> input_file(
      commands, "string",
      "The config file (YAML format). \nEx: MaSim -i input.yml",
      {'i', 'c', "input", "config"});
  args::ValueFlag<int> cluster_job_number(
      commands, "int", "Cluster job number. \nEx: MaSim -j 1", {'j'});
  args::ValueFlag<int> study_number(
      commands, "int",
      "Study number to associate with the configuration. \nEx: MaSim -s 1",
      {'s'});
  args::ValueFlag<std::string> reporter(
      commands, "string",
      "Reporter type, with additional seperated by commas \nEx: MaSim -r MMC",
      {'r'});
  args::ValueFlag<std::string> input_path(
      commands, "string",
      "Path for output files, default is current directory. \nEx: MaSim -p out",
      {'o'});
  args::Flag dump_movement(commands, "dump",
                           "Dump the movement matrix as calculated", {"dump"});
  args::Flag list_reporters(commands, "lr", "List the possible reporters",
                            {"lr"});
  args::Flag list_genotypes(commands, "lg", "List the possible genotypes",
                            {"lg"});
  args::Flag individual_movement(commands, "im",
                                 "Record individual movement detail", {"im"});
  args::Flag cell_movement(
      commands, "mc", "Record the movement between cells, cannot run with --md",
      {"mc"});
  args::Flag district_movement(
      commands, "md",
      "Record the movement between districts, cannot run with --mc", {"md"});
  args::Flag load_genotypes(
      commands, "load", "Load the genotypes to the database", {'l', "load"});

  // Allow the --v=[int] flag to be processed by START_EASYLOGGINGPP
  args::Group arguments(parser, "verbosity", args::Group::Validators::DontCare,
                        args::Options::Global);
  args::ValueFlag<int> verbosity(
      arguments, "int", "Sets the verbosity of the logging, default zero",
      {"v"});
  // NOLINTEND(cppcoreguidelines-slicing)

  try {
    parser.ParseCLI(argc, argv);

    // Check to see if we are listing reporters, do that and exit
    if (list_reporters) {
      std::cout << "Possible reporters for use with -r switch:\n\n";
      for (auto const &report : Reporter::ReportTypeMap) {
        std::cout << '\t' << report.first << std::endl;
      }
      std::cout << std::endl << "Note that names are case sensitive!\n";
      exit(EXIT_SUCCESS);
    }

    // Check to if both --mc and --md are set, if so, generate an error
    if (cell_movement && district_movement) {
      std::cerr << "--mc and --md are mutual exclusive and may not be run "
                   "together.\n";
      exit(EXIT_FAILURE);
    }
  } catch (const args::Help &e) {
    std::cout << "MaSim v. " << VERSION << std::endl;
    std::cout << e.what() << parser;
    exit(EXIT_SUCCESS);
  } catch (const args::ParseError &e) {
    std::string parserInfo;
    std::ostringstream oss;
    oss << parser;
    parserInfo = oss.str();

    LOG(ERROR) << fmt::format("{0} {1}", e.what(), parserInfo);
    exit(EXIT_FAILURE);
  } catch (const args::ValidationError &e) {
    std::string parserInfo;
    std::ostringstream oss;
    oss << parser;
    parserInfo = oss.str();

    LOG(ERROR) << fmt::format("{0} {1}", e.what(), parserInfo);
    exit(EXIT_FAILURE);
  }

  // Verify that the input file seems okay, exit if it isn't
  const auto input = input_file ? args::get(input_file) : "input.yml";
  if (!OsHelpers::file_exists(input)) {
    LOG(ERROR) << fmt::format(
        "File {0} does not exists. Rerun with -h or --help for help.", input);
    exit(EXIT_FAILURE);
  }
  if (input.find(".yml") == std::string::npos
      && input.find(".yaml") == std::string::npos) {
    LOG(ERROR) << fmt::format("File {0} does not appear to be a YAML file",
                              input);
    exit(EXIT_FAILURE);
  }
  model->set_config_filename(input);

  // Check to see if we are listing genotypes, do that and exit
  if (list_genotypes) {
    std::cout << "Genotypes present in configuration and their ids:"
              << std::endl;
    auto* config = new Config();
    config->read_from_file(model->config_filename());
    for (auto id = 0ul; id < config->number_of_parasite_types(); id++) {
      auto genotype = (*config->genotype_db())[id];
      std::cout << id << ":\t" << genotype->to_string(config) << std::endl;
    }
    exit(EXIT_SUCCESS);
  }

  // Check to see if we are doing a genotype load, do that and exit
  if (load_genotypes) {
    std::cout << "Loading genotypes..." << std::endl;
    if (DbLoader::load_genotypes(input)) {
      std::cout << "Load complete!" << std::endl;
    } else {
      std::cout << "Terminated with error(s)." << std::endl;
    }
    exit(EXIT_SUCCESS);
  }

  // Set the remaining values if given
  path = input_path ? args::get(input_path) : path;
  job_number = cluster_job_number ? args::get(cluster_job_number) : 0;
  model->set_cluster_job_number(job_number);
  const auto reporter_type = reporter ? args::get(reporter) : "";
  model->set_reporter_type(reporter_type);

  // Note the default to -1, not a valid sequence in the database
  int value = study_number ? args::get(study_number) : -1;
  model->set_study_number(value);

  // Flags related to how movement is recorded (or not)
  model->set_dump_movement(dump_movement);
  model->set_individual_movement(individual_movement);
  model->set_cell_movement(cell_movement);
  model->set_district_movement(district_movement);
}
