
#include <tula/logging.h>
#include <cstdlib>
#include <netcdf>
#include <Eigen/Dense>
#include <tula/config/flatconfig.h>
#include <tula/cli.h>

int bench_netcdf4(int nx, int ny)
{
  // Return this in event of a problem
  constexpr int nc_err = 2;

  Eigen::MatrixXd mat;
  {
    tula::logging::scoped_timeit TULA_X("create data matrix");
    mat = Eigen::MatrixXd::Random(ny, nx);
  }
  std::string filename = fmt::format("bench_netcdf4_{}.nc", mat.size());

  {
    tula::logging::scoped_timeit TULA_X("write data to file");
    try
    {
      netCDF::NcFile nc(filename, netCDF::NcFile::replace);

      auto d_x = nc.addDim("x", nx);
      auto d_y = nc.addDim("y", ny);

      auto v_data = nc.addVar("data", netCDF::ncInt, {d_x, d_y});

      v_data.putVar(mat.data());
    }
    catch (netCDF::exceptions::NcException &e)
    {
      SPDLOG_ERROR("Error write file: {}", e.what());
      return nc_err;
    }
  }
  // Now read the data back in
  {
    tula::logging::scoped_timeit TULA_X("read data back");
    Eigen::MatrixXd mat_in;
    try
    {
      // Open the file for read access
      netCDF::NcFile nc(filename, netCDF::NcFile::read);

      // Retrieve the variable named "data"
      auto v_data = nc.getVar("data");
      if (v_data.isNull()) {
        return nc_err;
      }
      auto dims = v_data.getDims();
      mat_in.resize(dims.at(0).getSize(), dims.at(1).getSize());
      v_data.getVar(mat_in.data());
    }
    catch (netCDF::exceptions::NcException &e)
    {
      SPDLOG_ERROR("Error read file: {}", e.what());
      return nc_err;
    }
  }
}

int main(int argc, char *argv[])
{

  using config_t = tula::config::FlatConfig;
  using namespace tula::cli::clipp_builder;
  // clang-format off
    auto parse = config_parser<config_t, config_t>{};
    auto screen = tula::cli::screen{
    // =======================================================================
                        "bmn",  "bmn", "v0.1",
                                "Benchmark NetCDF4"};
    auto [cli, rc, cc] = parse([&](auto &r, auto &c) { return (
    // rc -- prog config
    // cc -- cli config
    // =======================================================================
    c(p(           "h", "help"), "Print help information and exit"),
    c(p(             "version"), "Print version information and exit"),
    r(p(             "x", "nx"), "Data dimension to test", 100, opt_int()),
    r(p(             "y", "ny"), "Data dimension to test", 100, opt_int())
    // =======================================================================
    );}, screen, argc, argv);
  // clang-format on
  if (cc.get_typed<bool>("help"))
  {
    screen.manpage(cli);
    std::exit(EXIT_SUCCESS);
  }
  else if (cc.get_typed<bool>("version"))
  {
    screen.version();
    std::exit(EXIT_SUCCESS);
  }

  SPDLOG_INFO("Run config: {}", rc.pformat());
  bench_netcdf4(rc.get_typed<int>("nx"), rc.get_typed<int>("ny"));
  return EXIT_SUCCESS;
}
