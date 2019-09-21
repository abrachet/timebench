# timebench

timebench(1) is an alternative to time(1) and is fully compatible with GNU time 1.7. timebench adds features like the ability to run a command multiple times.

## Installation

timebench can be built from source easily using cmake.

```bash
git clone https://github.com/abrachet/timebench.git
cd timebench
mkdir build ; cd build
cmake ..
make timebench
# mv timebench to bin dir of choice
```

## Usage

A useful addition to timebench versus time, is that we can optionally run the command multiple times and collect either average output or output statistics per run. For example if we wanted to create a csv file this can be done easily with a command like this:

```bash 
timebench -r 10 -f "%e,%S,%U" -o command.csv <command>
```

The `-r` flag is an alias to `--runs`, it specifies how many times to run the given command. `-f` or `--format=arg` specifies a format string to use. This format string uses the same convention as GNU's time(1). By default, unless the `--average` or `-A` flag is given, will write this output for every run. In this case we want this behavior because we want to measure `<command>` 10 times and write to the csv file. The `-o` flag specifies an output file to create.

The performance of many programs is reliant on the filesystem, if this is potentially an issue the `--warmup` flag can be used to specify the number of times to run the program before considering it for measurement. In this case the caches will always be hot which is not always preferable, nor does it represent real world use cases. For this the `--prep` flag exists, it specifies a command to be run before running the command to time. For example if we wanted to drop filesystem caches we could use `--prep "sync; echo 3 > /proc/sys/vm/drop_caches"`.
