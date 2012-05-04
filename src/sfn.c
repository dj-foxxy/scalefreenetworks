#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#include <argtable2.h>

#define SFN_PROG_NAME "sfn"

typedef struct _sfn_node_t
{
    size_t id;
    size_t degree;
    struct _sfn_node_t **neighbours;
} sfn_node_t;

typedef struct
{
    size_t num_nodes;
    size_t max_nodes;
    sfn_node_t *nodes;
    bool *adjacency;
} sfn_t;

sfn_t *sfn_alloc(
        int const init_num_nodes,
        int const num_links,
        int const time_steps)
{
    int const max_nodes = init_num_nodes + time_steps;

    sfn_t *const sfn = malloc(sizeof(*sfn));
    sfn_node_t *const nodes = malloc(max_nodes * sizeof(*nodes));
    bool *const adjacency = malloc(
            max_nodes * max_nodes  * sizeof(*adjacency));
    sfn_node_t **const neighbours = malloc(
            max_nodes * max_nodes * sizeof(*neighbours));

    if (sfn == NULL || nodes == NULL || adjacency == NULL || neighbours == NULL)
    {
        goto error;
    }

    sfn->num_nodes = 0;
    sfn->max_nodes = max_nodes;
    sfn->nodes = nodes;
    sfn->adjacency = adjacency;

    for (int i = 0; i < max_nodes; ++i)
    {
        nodes[i].id = i;
        nodes[i].degree = 0;
        nodes[i].neighbours = neighbours + max_nodes * i;
    }

    for (int i = 0; i < max_nodes * max_nodes; ++i)
    {
        adjacency[i] = false;
    }

    return sfn;

error:
    free(sfn);
    free(nodes);
    free(adjacency);
    free(neighbours);
    return NULL;
}

void sfn_init(
            sfn_t *const sfn,
            int const num_nodes,
            double const link_prob)
{
    sfn->num_nodes = num_nodes;
    if (link_prob == 0.0)
    {
            return;
    }

    for (int i = 0; i < num_nodes; ++i)
    {
        for (int j = 0; j < i; ++j)
        {
            double const p = (double)rand() / (double)RAND_MAX;

            if (p <= link_prob)
            {
                sfn->adjacency[i * num_nodes + j] = true;
                sfn->nodes[i].neighbours[sfn->nodes[i].degree++] =
                                sfn->nodes + j;
                sfn->nodes[j].neighbours[sfn->nodes[j].degree++] =
                                sfn->nodes + i;
            }
        }
    }
}

int get_random_index(int max)
{
    return (int) round((double)(rand() * (double)max)
            / (double)RAND_MAX);
}


double approximate_clustering_coefficient(sfn_t const *const sfn,
        int const num_samples)
{
    double current_clustering_coefficient = 0.0;
    for (int k = 0; k < num_samples; ++k)
    {
        sfn_node_t const j = sfn->nodes[get_random_index(sfn->num_nodes - 1)];
        if (j.degree < 2)
        {
            continue;
        }
        sfn_node_t const u = *j.neighbours[get_random_index(j.degree -1)];
        sfn_node_t v;
        do
        {
            v = *j.neighbours[get_random_index(j.degree - 1)];
        } while (u.id == v.id);
        sfn_node_t first;
        sfn_node_t second;
        if (u.id > v.id)
        {
            first = u;
            second = v;
        }
        else
        {
            first = v;
            second = u;
        }
        if (sfn->adjacency[first.id * sfn->num_nodes + second.id])
        {
            current_clustering_coefficient += 1.0;
        }
    }
    return current_clustering_coefficient / (double) num_samples;
}

bool sfn_write_dot_file(sfn_t const *const sfn, char const *const path)
{
    FILE *const dot_file = fopen(path, "w");
    if (dot_file == NULL)
    {
        return false;
    }
    bool success = true;
    if (fprintf(dot_file, "graph sfn{") < 0)
    {
        success = false;
        goto error;
    }
    for (int i = 0; i < sfn->num_nodes; ++i)
    {
        if (fprintf(dot_file, "%lu;", sfn->nodes[i].id) < 0)
        {
            success = false;
            goto error;
        }
    }
    for (int i = 0; i < sfn->num_nodes; ++i)
    {
        for (int j = 0; j < sfn->nodes[i].degree; ++j)
        {
            if (sfn->nodes[i].neighbours[j]->id > i)
            {
                if(fprintf(dot_file, "%lu--%lu;", i,
                        sfn->nodes[i].neighbours[j]->id) < 0)
                {
                    success = false;
                    goto error;
                }
            }
        }
    }
    if (fprintf(dot_file, "}\n") < 0)
    {
        success = false;
        goto error;
    }
error:
    fclose(dot_file);
    return success;
}

void sfn_free(
        sfn_t *const sfn)
{
    free(sfn->nodes[0].neighbours);
    free(sfn->adjacency);
    free(sfn->nodes);
    free(sfn);
}

int main(
    int const argc,
    char **argv)
{
    struct arg_int *arg_init_num_nodes = arg_intn("n", NULL, "<n>", 0, 1,
			"Initial number of nodes (n >= 1).");
    if (arg_init_num_nodes != NULL)
    {
        arg_init_num_nodes->ival[0] = 1;
    }

    struct arg_dbl *arg_init_link_prob = arg_dbln("p", NULL, "<prob>", 0, 1,
                    "Initial link probability (0 <= p <= 1).");
    if (arg_init_link_prob != NULL)
    {
            arg_init_link_prob->dval[0] = 0.2;
    }

    struct arg_int *arg_num_links = arg_intn("m", NULL, "<n>", 0, 1,
            "Number of links to each new node (m <= n).");
    if (arg_num_links != NULL)
    {
        arg_num_links->ival[0] = 3;
    }

    struct arg_int *arg_time_steps = arg_intn("T", NULL, "<n>", 0, 1,
            "The number of time steps (T >= 0).");
    if (arg_time_steps != NULL)
    {
        arg_time_steps->ival[0] = 1000;
    }

    struct arg_str *arg_dot_path = arg_strn("d", NULL, "<path>", 0, 1,
            "Path to write a dot file to.");

    struct arg_int *arg_seed = arg_intn("s", NULL, "<seed>", 0, 1,
                    "Random seed.");

    struct arg_int *arg_num_samples = arg_intn("k", NULL, "<samples>", 0, 1,
            "Number of samples to use for the clustering coefficient approx.");
    if (arg_num_samples != NULL)
    {
        arg_num_samples->ival[0] = 50000;
    }

    struct arg_end *end = arg_end(10);

    void *arg_table[] = { arg_init_num_nodes, arg_init_link_prob,
                    arg_num_links, arg_time_steps, arg_dot_path, arg_seed,
                    arg_num_samples, end };

    int exit_code = EXIT_SUCCESS;

    sfn_t *sfn = NULL;

    if (arg_nullcheck(arg_table) != 0)
    {
            printf("[ERROR] Insufficient memory for argtable.\n");
            exit_code = EXIT_FAILURE;
            goto exit;
    }

    int const num_errors = arg_parse(argc, argv, arg_table);

    int const init_num_nodes = arg_init_num_nodes->ival[0];
    double const init_link_prob = arg_init_link_prob->dval[0];
    int const num_links = arg_num_links->ival[0];
    int const time_steps = arg_time_steps->ival[0];
    int const num_samples = arg_num_samples->ival[0];

    if (num_errors > 0 && num_links >= 1 && time_steps >= 0 &&
        num_links > init_num_nodes)
    {
            arg_print_errors(stdout, end, SFN_PROG_NAME);
            exit_code = EXIT_FAILURE;
            goto exit;
    }

    if (arg_seed->count > 0)
    {
            printf("Seeding PRNG with %d.\n", arg_seed->ival[0]);
            srand(arg_seed->ival[0]);
    }

    if ((sfn = sfn_alloc(init_num_nodes, num_links, time_steps)) == NULL)
    {
        fprintf(stderr, "[ERROR] Insufficient memory for sfn.\n");
        exit_code = EXIT_FAILURE;
        goto exit;
    }

    sfn_init(sfn, init_num_nodes, init_link_prob);

    double approx_cc = approximate_clustering_coefficient(sfn, num_samples);

    printf("approx cc: %f\n", approx_cc);

    if (arg_dot_path->count > 0)
    {
	printf("Writing DOT file...");
    	fflush(stdout);
        if (!sfn_write_dot_file(sfn, arg_dot_path->sval[0]))
        {
            fprintf(stderr, "[ERROR] Failed to write dot file.\n");
            exit_code = EXIT_FAILURE;
            goto exit;
        }
	printf("done.\n");
    }

exit:
    arg_freetable(arg_table, sizeof(arg_table) / sizeof(arg_table[0]));
    sfn_free(sfn);
    return exit_code;
}

