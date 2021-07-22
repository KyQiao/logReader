#include "logReader.hpp"
// #include <pybind11/iostream.h>
// #include <pybind11/numpy.h>
#include <pybind11/operators.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;
// using intArray_t = py::array_t<int, py::array::c_style | py::array::forcecast>;

PYBIND11_MODULE(_logReader, m) {
  py::class_<thermoData>(m, "thermoData")
      .def("get", (&thermoData::get))
      .def_readonly("data", (&thermoData::attr_table))
      .def_readonly("string", (&thermoData::attr_order));
  py::class_<logReader>(m, "logReader")
      // .def(py::init<>())
      .def(py::init<const std::string&>())
      .def("read", (&logReader::read))
      .def("describe", (&logReader::describe))
      // by default pybind11 return policy is take onwership which is not necessary here
      // change to reference to solve double free problem
      .def("PythonAPI_getdata", (&logReader::PythonAPI_getdata), py::return_value_policy::reference);
  // .def("update", (&logReader::update), py::arg("n") = 1)
  // .def("getData", [](logReader &self)
  //  { return intArray_t({self._size()}, self.getData()); });

#ifdef VERSION_INFO
  m.attr("__version__") = VERSION_INFO;
#else
  m.attr("__version__") = "0.1.2";
#endif
}