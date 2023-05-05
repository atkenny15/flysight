```
python3 -m venv venv
source venv/bin/activate
pip3 install conan
mkdir build
cd build
conan install . --output-folder=build --build=missing
cmake -DCMAKE_BUILD_TYPE=Release ..
```
