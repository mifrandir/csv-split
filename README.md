# CSV Split
![Build](https://github.com/MiltFra/csv-split/workflows/Build/badge.svg)
![Tests](https://github.com/MiltFra/csv-split/workflows/Tests/badge.svg)

A simple command line program that splits [CSV](https://en.wikipedia.org/wiki/Comma-separated_values) files into smaller parts.

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

Simply run the executable with the desired inputs and flags.
Then it will create a set of files of the form `<NEW_FILE><INDEX>.csv` that contain the 
rows of the original file in the range `[INDEX+2,INDEX+2+COUNT)` where line `1` is the input header. 

## Installation

If you are using Arch Linux, you can use the [AUR package](https://aur.archlinux.org/packages/csv-split/).

Otherwise, clone the repository and change into directory:

```
$ git clone https://github.com/miltfra/csv-split
$ cd csv-split
```

Create binary in `bin/`:

```
$ make
```

Install to `/usr/local/bin/` (superuser):

```
# make install
```

Install to local directory (e.g. `$HOME/.local/bin`):

```
$ make DESTDIR=$HOME/.local/bin install-local
```

Installations can be undone with the correspodning `uninstall` and `uninstall-local` commands.

To force a complete rebuild, use

```
$ make clean
$ make build
```

## Testing 

You can verify that everything compiled successfully by running

```
$ make check
``` 

after `make`.

If you want to verify that the software behaves as intended, run

```
$ make test
```

and check the output. If you find a bug, please file a bug report on [GitHub](https://github.com/miltfra/csv-split).

## About 

This program is heavily inspired by [imartingraham/csv-split](https://github.com/imartingraham/csv-split). 
It makes use of the already existing interface and aims to improve on speed by switching from Ruby to C.

This software has not yet been fully tested and should therefore be used with care.
