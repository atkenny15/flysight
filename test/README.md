```
python3 -m venv venv
source venv/bin/activate
pip3 install conan
conan install . --output-folder=build --build=missing
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..

conan install . --output-folder=build --build=missing -s build_type=Debug
cd build
cmake -DCMAKE_BUILD_TYPE=Debug ..
```
