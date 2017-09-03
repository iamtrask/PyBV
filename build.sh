rm -rf build

swig -c++ -python BV.i #1> /dev/null 2> /dev/null
python setup.py install #1> /dev/null 2> /dev/null
