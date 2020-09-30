#include "main.h"

#include "config.h"
#include "split.h"

int main(int argc, char *argv[]) {
	struct Config cfg;
	initialise_config(&cfg);
	parse_config(&cfg, argc, argv);
	process_config(&cfg);
	split_csv(&cfg);
}
