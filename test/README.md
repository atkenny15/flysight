```
python3 -m venv venv
source venv/bin/activate
pip install -r requirements.txt

conan install . --output-folder=build --build=missing
cmake --preset conan-release
cd build
make -j4
make test

conan install . --output-folder=build --build=missing -s build_type=Debug
cmake --preset conan-release
cd build
make -j4
make test
```
