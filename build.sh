rm -rf build
export CPLUS_INCLUDE_PATH=/System/Library/Frameworks/Python.framework/Headers
export C_INCLUDE_PATH=/System/Library/Frameworks/Python.framework/Headers
swig -c++ -python example.i
python setup.py install
