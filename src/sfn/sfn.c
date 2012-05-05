#include <assert.h>
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
    size_t total_degree;
    sfn_node_t *nodes;
    bool *adjacency;
    double *dist;
} sfn_t;

static FILE *anim = NULL;

static sfn_t *sfn_alloc(
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
    double *dist = malloc(max_nodes * sizeof(*dist));

    if (sfn == NULL || nodes == NULL || adjacency == NULL || neighbours == NULL
            || dist == NULL)
    {
        goto error;
    }

    sfn->num_nodes = 0;
    sfn->max_nodes = max_nodes;
    sfn->total_degree = 0;
    sfn->nodes = nodes;
    sfn->adjacency = adjacency;
    sfn->dist = dist;

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
    free(dist);
    return NULL;
}

static void sfn_add_link(
        sfn_t *const sfn,
        size_t i,
        size_t j)
{
    assert(i != j);

    sfn->total_degree += 2;
    sfn->adjacency[i * sfn->max_nodes + j] = true;
    sfn->adjacency[j * sfn->max_nodes + i] = true;
    sfn->nodes[i].neighbours[sfn->nodes[i].degree++] =
        sfn->nodes + j;
    sfn->nodes[j].neighbours[sfn->nodes[j].degree++] =
        sfn->nodes + i;
}

static void sfn_init(
        sfn_t *const sfn,
        int const num_nodes,
        double const link_prob)
{
    sfn->num_nodes = num_nodes;
    fprintf(anim, "I %lu\n", sfn->num_nodes);

    if (link_prob == 0.0)
    {
        return;
    }

    for (size_t i = 0; i < num_nodes; ++i)
    {
        for (size_t j = 0; j < i; ++j)
        {
            double const p = (double)rand() / (double)RAND_MAX;

            if (link_prob > p)
            {
                fprintf(anim, "R %lu %lu\n", i, j);
                sfn_add_link(sfn, i, j);
            }
        }
    }
}

static void sfn_ba(
        sfn_t *const sfn,
        int const num_links,
        int const time_steps)
{
    for (int t = 1; t <= time_steps; ++t)
    {
        for (size_t i = 0; i < sfn->num_nodes; ++i)
        {
            sfn->dist[i] = ((double)sfn->nodes[i].degree + 1)
                / ((double)sfn->total_degree + sfn->num_nodes);
        }

        size_t i = sfn->num_nodes++;
        fprintf(anim, "A\n");
        double tp = 1.0;

        for (size_t l = 0; l < num_links; ++l)
        {
            double const p = ((double)rand() * tp) / (double)RAND_MAX;
            double q = 0.0;

            size_t j = 0;
            for (;;)
            {
                q += sfn->dist[j];
                if (p <= q)
                {
                    break;
                }
                j++;

            }

            sfn_add_link(sfn, i, j);
            fprintf(anim, "L %lu %lu\n", i, j);
            tp -= sfn->dist[j];
            sfn->dist[j] = 0.0;
        }
    }
}

static double calculate_clustering_coefficient(
        sfn_t const *const sfn)
{
    double cc = 0.0;

    for (int i = 0; i < sfn->num_nodes; ++i)
    {
        sfn_node_t const *const node = &sfn->nodes[i];

        int const k = node->degree;

        if (k < 2)
        {
            continue;
        }

        int e = 0;

        for (int j = 0; j < k; ++j)
        {
            sfn_node_t const *const neighbour = node->neighbours[j];

            for (int n = 0; n < neighbour->degree; ++n)
            {
                sfn_node_t const *const n_neighbour = neighbour->neighbours[n];

                if (sfn->adjacency[node->id * sfn->max_nodes + n_neighbour->id])
                {
                    e += 1;
                }
            }
        }

        double const local_cc = (double)e / (double)(k * (k - 1));
        cc += local_cc;
    }

    return cc / (double)sfn->num_nodes;
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
    int skips = 0;
    for (int k = 0; k < num_samples; ++k)
    {
        sfn_node_t const j = sfn->nodes[get_random_index(sfn->num_nodes - 1)];
        if (j.degree < 2)
        {
            skips++;
            continue;
        }
        sfn_node_t const u = *j.neighbours[get_random_index(j.degree -1)];
        sfn_node_t v;
        do
        {
            v = *j.neighbours[get_random_index(j.degree - 1)];
        } while (u.id == v.id);
        if (sfn->adjacency[u.id * sfn->max_nodes + v.id])
        {
            current_clustering_coefficient += 1.0;
        }
    }
    return current_clustering_coefficient
            / ((double) num_samples - (double) skips);
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
    for (size_t i = 0; i < sfn->num_nodes; ++i)
    {
        for (size_t j = 0; j < sfn->nodes[i].degree; ++j)
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
    free(sfn->dist);
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

    struct arg_str *arg_init_link_prob = arg_strn("p", NULL, "<prob>", 0, 1,
            "Initial link probability (0 <= p <= 1).");
    if (arg_init_link_prob != NULL)
    {
        arg_init_link_prob->sval[0] = "0.2";
    }

    struct arg_int *arg_num_links = arg_intn("m", NULL, "<n>", 0, 1,
            "Number of links to each new node (m <= n).");

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

    struct arg_str *arg_anim_path = arg_strn("a", NULL, "<path>", 0, 1,
            "Path to write the animation scipt (stdout default).");

    struct arg_end *end = arg_end(10);

    void *arg_table[] = { arg_init_num_nodes, arg_init_link_prob,
                    arg_num_links, arg_time_steps, arg_dot_path, arg_seed,
                    arg_num_samples, arg_anim_path, end };

    int exit_code = EXIT_SUCCESS;

    sfn_t *sfn = NULL;

    if (arg_nullcheck(arg_table) != 0)
    {
        printf("[ERROR] Insufficient memory for argtable.\n");
        exit_code = EXIT_FAILURE;
        goto exit;
    }

    int const num_errors = arg_parse(argc, argv, arg_table);

    if (num_errors > 0)
    {
        arg_print_errors(stdout, end, SFN_PROG_NAME);
        exit_code = EXIT_FAILURE;
        goto exit;
    }

    int const init_num_nodes = arg_init_num_nodes->ival[0];
    double const init_link_prob = strtod(arg_init_link_prob->sval[0], NULL);
    int num_links;
    if (arg_num_links->count == 0)
    {
        num_links = init_num_nodes;
    }
    else
    {
        num_links = arg_num_links->ival[0];
    }
    int const time_steps = arg_time_steps->ival[0];
    int const num_samples = arg_num_samples->ival[0];

    if (init_num_nodes < 1 || time_steps < 0 || num_links > init_num_nodes ||
            init_link_prob > 1 || init_link_prob <= 0)
    {
        fprintf(stderr, "Invalid arguments.\n");
        exit_code = EXIT_FAILURE;
        goto exit;
    }

    if (arg_seed->count > 0)
    {
        printf("Seeding PRNG with %d.\n", arg_seed->ival[0]);
        srand(arg_seed->ival[0]);
    }

    if (arg_anim_path->count == 0)
    {
        anim = stdout;
    }
    else
    {
        if ((anim = fopen(arg_anim_path->sval[0], "w")) == NULL)
        {
            fprintf(stderr, "[ERROR} Cannot open animation path.");
            exit_code = EXIT_FAILURE;
            goto exit;
        }

    }

    if ((sfn = sfn_alloc(init_num_nodes, num_links, time_steps)) == NULL)
    {
        fprintf(stderr, "[ERROR] Insufficient memory for sfn.\n");
        exit_code = EXIT_FAILURE;
        goto exit;
    }

    sfn_init(sfn, init_num_nodes, init_link_prob);
    sfn_ba(sfn, num_links, time_steps);

    double const approx_cc = approximate_clustering_coefficient(sfn,
            num_samples);
    double const real_cc = calculate_clustering_coefficient(sfn);

    printf("Approx CC: %f\n", approx_cc);
    printf("  Real CC: %f\n", real_cc);

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
    if (sfn != NULL)
    {
        sfn_free(sfn);
    }
    if (anim != NULL)
    {
        fclose(anim);
    }
    arg_freetable(arg_table, sizeof(arg_table) / sizeof(arg_table[0]));
    return exit_code;
}
