#include <algorithm>
#include <cassert>
#include <cmath>
#include <exception>
#include <fstream>
#include <functional>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

using d_data = std::vector<double>;
using thermo = struct thermoData {
  thermoData(){};
  // store all data as double type, transform into int in Python if needed.
  std::vector<d_data> attr_table;                      //where the rest properties store [property][particle ID]
  std::string attr_order;                              //i.e. "q c_pe c_voro[1] c_ackland ... "
  std::unordered_map<std::string, size_t> attr_index;  //dict store index and names. i.e. d["q"]=0 d["c_pe"]=1

  d_data &get(std::string attr) {
    size_t index;
    if (this->attr_index.count(attr)) {
      index = this->attr_index.operator[](attr);
    } else {
      std::cout << "input attr " << attr << " not find\n"
                << "supported attrs are " << this->attr_order << std::endl;
      throw std::runtime_error("\n");
    }
    return this->attr_table.operator[](index);
  }

  size_t size() const {
    return attr_table[0].size();
  }

  void describe() {
    std::stringstream ss(this->attr_order);
    std::string token;
    std::cout << "thermo data attributes are:\n";

    while (ss >> token)
      std::cout << std::left << std::setw(12) << token;
    std::cout << "\n";
    if (this->size() > 10) {
      for (size_t i = 0; i < 5; i++) {
        for (size_t j = 0; j < this->attr_table.size(); j++) {
          std::cout << std::left << std::setw(12) << this->attr_table[j][i];
        }
        std::cout << "\n";
      }
      for (size_t i = 0; i < this->attr_table.size(); i++)
        std::cout << std::left << std::setw(12) << "...";
      std::cout << "\n";

      for (size_t i = this->size() - 5; i < this->size(); i++) {
        for (size_t j = 0; j < this->attr_table.size(); j++) {
          std::cout << std::left << std::setw(12) << this->attr_table[j][i];
        }
        std::cout << "\n";
      }
    } else {
      for (size_t i = 0; i < this->size(); i++) {
        for (size_t j = 0; j < this->attr_table.size(); j++) {
          std::cout << std::left << std::setw(12) << this->attr_table[j][i];
        }
        std::cout << "\n";
      }
    }
  }
};

class logReader {
private:
  std::vector<thermo *> runs;

public:
  logReader(){};
  logReader(const std::string &file) {
    read(file);
  };
  ~logReader() {
    for (size_t i = 0; i < getSize(); i++) {
      delete runs[i];
    }
  };

  void read(std::string);
  int checker(std::string);

  // return # of thermoData
  size_t getSize() const {
    return runs.size();
  }
  void describe() const {
    for (size_t i = 0; i < getSize(); i++) {
      runs[i]->describe();
      std::cout << std::endl;
    }
  }

  // ####### API for CPP ##################
  // return ptr of thermoData

  thermo *operator[](int index) {
    return runs[index];
  }

  const thermo *operator[](int index) const {
    return runs[index];
  }
  // ####### API for CPP ##################

  // ####### API for PYTHON ###############
  std::vector<thermo *> &PythonAPI_getdata() {
    return runs;
  }
  // ####### API for PYTHON ###############
};
