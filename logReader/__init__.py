from ._logReader import logReader as m


class thermo(object):
    def __init__(self, thermodata):
        self.data = thermodata.data
        self.attr = thermodata.string

        for index, key in enumerate(self.keys(), start=0):
            # if not str.isspace(key):
            if key != "":
                setattr(self, self.legalKey(key), self.data[index])

    def __repr__(self) -> str:
        return "attrs are "+self.attr

    def keys(self):
        return self.attr.split(" ")

    def legalKey(self, s):
        tmp = s.replace('[', '_').replace(']', '_').split("_")
        if (tmp[0] == 'f' or tmp[0] == 'c' or tmp[0] == 'v'):
            tmp.pop(0)
        return "".join(tmp)


class logReader(object):
    """reading lammps file"""

    def __init__(self, fileName):
        super(logReader, self).__init__()
        self.cppReader = m(fileName)
        self.runs = self.cppReader.PythonAPI_getdata()
        self.data = []
        for data in self.runs:
            self.data.append(thermo(data))

    def describe(self):
        self.cppReader.describe()

    def keys(self):
        return self.attr.split(" ")

    def __getitem__(self, key):
        return self.data[key]
