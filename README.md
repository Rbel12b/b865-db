# B865-db

A command-line tool for debugging c and asssembly code written for the b865 processor.
It's commands are like gdb's, but with some differences.

## Usage

The debugger includes an emulator for the b865 processor, this emulator is used to run the code.
It supports debug files generated by SDCC (.cdb files).

Currently only the CDB parser is implemented.

```bash
./b865-db <debug-file>
```

## Installation

To build and install b865-db, ensure you have a C++ compiler and make installed on your system. Then, follow these steps:

### 1. Clone the Repository

```bash
git clone https://github.com/Rbel12b/b865-db.git
cd b865-db
```

### 2. Build the Debugger

```bash
make
```
