#include<stdlib.h>

#include <argtable2.h>

#define SFN_PROG_NAME "sfn"



int main(int const argc, char **argv)
{
	struct arg_int *arg_init_num_nodes = arg_intn("n", NULL, "<nodes>", 0, 1,
			"Initial number of nodes (n >= 1).");
    if (arg_init_num_nodes != NULL)
    {
        arg_init_num_nodes->ival[0] = 1;
    }

    struct arg_int *arg_num_links = arg_intn("m", NULL, "<links>", 0, 1,
            "Number of links to each new node (m <= n).");
    if (arg_num_links != NULL)
    {
        arg_num_links->ival[0] = 3;
    }

    struct arg_int *arg_time_steps = arg_intn("T", NULL, "<steps>", 0, 1,
            "The number of time steps (T >= 0).");
    if (arg_time_steps != NULL)
    {
        arg_time_steps->ival[0] = 1000;
    }

	struct arg_end *end = arg_end(10);

	void *arg_table[] = { arg_init_num_nodes, arg_num_links, arg_time_steps,
            end };

	int exit_code = EXIT_SUCCESS;

    if (arg_nullcheck(arg_table) != 0)
	{
		printf("[ERROR] Insufficient memory.\n");
		exit_code = EXIT_FAILURE;
		goto exit;
	}

	int const num_errors = arg_parse(argc, argv, arg_table);

    int const init_num_nodes = arg_init_num_nodes->ival[0];
    int const num_links = arg_num_links->ival[0];
    int const time_steps = arg_time_steps->ival[0];

	if (num_errors > 0 && num_links >= 1 && time_steps >= 0 &&
            num_links > init_num_nodes)
	{
		arg_print_errors(stdout, end, SFN_PROG_NAME);
		exit_code = EXIT_SUCCESS;
		goto exit;
	}

exit:
	arg_freetable(arg_table, sizeof(arg_table) / sizeof(arg_table[0]));

	return exit_code;
}

