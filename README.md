# logReader

A log reader to read LAMMPS log files. Using C++ as backend and Python as interface. Automatically parsing LAMMPS log files and translate `thermo` outputs into Python attributes.

##  Example

Here’s a short example of how to using `logReader`:

```python
>>> from logReader import logReader
>>> t=logReader("T15_06_08_P50.log")
>>> t.describe()
thermo data attributes are:
Step        Temp        E_pair      E_mol       TotEng      Press       Volume      
0           1.49996     13.5451     0           15.7949     67.1466     23405.7     
4e+06       1.51405     3.19639     0           5.46733     50.3107     19692.7     

thermo data attributes are:
Step        Temp        PotEng      KinEng      TotEng      Enthalpy    Volume      Press       
0           1.51405     3.19639     2.27093     5.46733     65.9382     19692.7     50.3107     
1000        1.49421     3.06943     2.24118     5.31061     65.1286     19719.8     49.6992     
2000        1.51361     3.15507     2.27028     5.42535     65.6919     19702.3     50.1164     
3000        1.48297     3.13433     2.22432     5.35865     65.4995     19703.3     50.0093     
4000        1.50583     3.1298      2.2586      5.3884      65.5081     19705.1     49.9872     
...         ...         ...         ...         ...         ...         ...         ...         
3.996e+06   0.6044      2.77837     0.906545    3.68491     62.7584     19340.4     50.0436     
3.997e+06   0.592388    2.75465     0.888528    3.64318     62.562      19348.7     49.891      
3.998e+06   0.600167    2.80198     0.900195    3.70217     62.904      19336       50.1635     
3.999e+06   0.59712     2.73543     0.895625    3.63106     62.4631     19354.8     49.8018     
4e+06       0.59765     2.8091      0.89642     3.70552     62.9421     19333.8     50.1988   
>>> dir(t[1])
['Enthalpy', 'KinEng', 'PotEng', 'Press', 'Step', 'Temp', 'TotEng', 'Volume', '__class__', '__delattr__', '__dict__', '__dir__', '__doc__', '__eq__', '__format__', '__ge__', '__getattribute__', '__gt__', '__hash__', '__init__', '__init_subclass__', '__le__', '__lt__', '__module__', '__ne__', '__new__', '__reduce__', '__reduce_ex__', '__repr__', '__setattr__', '__sizeof__', '__str__', '__subclasshook__', '__weakref__', 'attr', 'data', 'keys', 'legalKey']
>>> print(t[1].Press[:10])
[50.310728, 49.699177, 50.11637, 50.009271, 49.987162, 49.998432, 49.899897, 50.119902, 49.863181, 50.150043]
```

each column will be stored as Python attributes for easily accessing.

## Installation

Only Windows binary is provided in release. For Linux and MacOS, build from source using:

```bash
git clone git@github.com:KyQiao/logReader.git
cd logReader
git submodule update --init
python setpy.py bdist_wheel
cd dist && pip install xxx.whl
```
