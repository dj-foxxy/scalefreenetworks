#include<stdlib.h>

#include <argtable2.h>

#define SFN_PROG_NAME "sfn"

int main(int const argc, char **argv)
{
	struct arg_int *init_num_nodes = arg_intn("n", NULL, "<n>", 0, 1,
			"Initial number of nodes.");
    if (init_num_nodes != NULL)
    {
        init_num_nodes->ival[0] = 1;
    }
	struct arg_end *end = arg_end(1);
	void *arg_table[] = { init_num_nodes, end };

	int exit_code = EXIT_SUCCESS;

    if (arg_nullcheck(arg_table) != 0)
	{
		printf("[ERROR] Insufficient memory.\n");
		exit_code = EXIT_FAILURE;
		goto exit;
	}

	int const num_errors = arg_parse(argc, argv, arg_table);

	if (num_errors > 0)
	{
		arg_print_errors(stdout, end, SFN_PROG_NAME);
		exit_code = EXIT_SUCCESS;
		goto exit;
	}

exit:
	arg_freetable(arg_table, sizeof(arg_table) / sizeof(arg_table[0]));

	return exit_code;
}

