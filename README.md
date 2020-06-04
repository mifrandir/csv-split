![C/C++ CI](https://github.com/MiltFra/csv-split/workflows/C/C++%20CI/badge.svg?branch=master)

# CSV Split

A simple command line program that splits CSV (comma-separated values) files into smaller parts.

**DISCLAIMER**: This is software has not been tested properly.

## Installation

Clone the repository:

```
$ git clone https://github.com/miltfra/csv-split
```

Change into directory and make binary:

```
$ cd csv-split
$ make
```

You can move the resulting binary (`bin/csv-split`) into your path (e.g. `$HOME/.local/bin`) for convenient use anywhere on your system.

## Usage

```
$ csv-split -h
Split CSV files by lines

USAGE:
        csv-split [OPTIONS] <FILE_TO_SPLIT>

OPTIONS:
        -n, --new-file-name <NEW_FILE>          Name of the new files. This will be appended with an incremented number (default: "split")
        -e, --exclude-headers                   Exclude headers in new files (default: false)
        -l, --line-count <COUNT>                Number of lines per file (default: 1)
        -d, --delimiter <DELIMITER>             Character used for column separation (default: ',')
        -r, --remove-columns <FILE>             Specify column names to be removed during processing in specified file.
        -i, --include-remainders                Include remainder rows in the split files (default: false).
        -h, --help                              Display this message
```

## About 

This program is heavily inspired by [imartingraham/csv-split](https://github.com/imartingraham/csv-split). 
It makes use of the already existing interface and aims to improve on speed by switching from Ruby to C.

This software has not yet been fully tested and should therefore be used with care.
