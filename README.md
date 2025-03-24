# Market Data Parser

- The project is developed and tested on **Windows**.

## Setup Instructions

### Install Dependencies
1. Install **MinGW** and add it to the system **PATH**.
2. Download **GoogleTest** and place it in the project folder.
   ```sh
   git clone https://github.com/google/googletest.git
   ```

### Build & Run GoogleTest
```sh
cd googletest
mkdir build
cd build
cmake .. -G "MinGW Makefiles"
mingw32-make
```

### Build Project & Run Tests
Run these commands in the **project folder**:
```sh
mingw32-make all
./test_parser
```

## Notes
- A demonstration video is attached.
- The test output will be generated in the project folder.