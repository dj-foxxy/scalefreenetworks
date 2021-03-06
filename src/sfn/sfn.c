#define _BSD_SOURCE

#include <assert.h>
#include <math.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>

#include <argtable2.h>

#define SFN_PROG_NAME "sfn"

#define COLOUR_HOME "green"
#define COLOUR_HOME_UNVISITED "white"
#define COLOUR_HOME_VISITED "grey"
#define COLOUR_NEIGHBOUR "red"
#define COLOUR_NEIGHBOUR_VISITED "plum1"
#define COLOUR_N_NEIGHBOUR "powderblue"
#define COLOUR_N_NEIGHBOUR_VISITED "powderblue"
#define COLOUR_HIT "yellow"

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

static void sfn_anim(char const *const fmt, ...)
{
    if (anim == NULL)
    {
        return;
    }

    va_list va;
    va_start(va, fmt);
    vfprintf(anim, fmt, va);
    va_end(va);
}

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
    sfn_anim("I %lu\n", sfn->num_nodes);

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
                sfn_anim("R %lu %lu\n", i, j);
                sfn_add_link(sfn, i, j);
            }
        }
    }

    sfn_anim("F\n");
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

        size_t const i = sfn->num_nodes++;
        sfn_anim("A\nC %lu red\nF\n", i);
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
            sfn_anim("L %lu %lu\nE %lu %lu red\nF\nE %lu %lu black\n",
                    i, j, i, j, i, j);
            tp -= sfn->dist[j];
            sfn->dist[j] = 0.0;
        }
        sfn_anim("C %lu white\n", i);
    }
}

static double calculate_clustering_coefficient(
        sfn_t const *const sfn)
{
    double cc = 0.0;
    bool *const neighbours_visited = malloc(sizeof(bool) * sfn->max_nodes);
    if (neighbours_visited == NULL)
    {
        goto error;
    }

    for (size_t i = 0; i < sfn->num_nodes; ++i)
    {
        for (size_t j= 0; j < sfn->num_nodes; ++j)
        {
            neighbours_visited[j] = false;
        }
        sfn_node_t const *const node = &sfn->nodes[i];
        sfn_anim("C %lu %s\nF\n", node->id, COLOUR_HOME);

        int const k = node->degree;

        if (k < 2)
        {
            sfn_anim("C %lu %s\n", node->id, COLOUR_HOME_VISITED);
            continue;
        }

        int e = 0;

        for (size_t j = 0; j < k; ++j)
        {
            sfn_node_t const *const neighbour = node->neighbours[j];

            sfn_anim("C %lu %s\nE %zu %zu %s\nF\n", 
                    neighbour->id, 
                    COLOUR_NEIGHBOUR,
                    node->id,
                    neighbour->id,
                    COLOUR_NEIGHBOUR);

            for (size_t n = 0; n < neighbour->degree; ++n)
            {
                sfn_node_t const *const n_neighbour = neighbour->neighbours[n];

                if (n_neighbour->id == node->id)
                {
                    continue;
                }

                if (sfn->adjacency[node->id * sfn->max_nodes + n_neighbour->id])
                {
                    e += 1;
                    sfn_anim("C %lu %s\nE %zu %zu %s\nE %zu %zu %s\n"
                            "E %zu %zu %s\nF\n", 
                            n_neighbour->id, 
                            COLOUR_HIT,
                            neighbour->id,
                            n_neighbour->id,
                            COLOUR_HIT,
                            node->id,
                            n_neighbour->id,
                            COLOUR_HIT,
                            node->id,
                            neighbour->id,
                            COLOUR_HIT);
                }
                else
                {
                    sfn_anim("C %lu %s\nE %zu %zu %s\nF\n", 
                            n_neighbour->id,
                            COLOUR_N_NEIGHBOUR,
                            neighbour->id,
                            n_neighbour->id,
                            COLOUR_N_NEIGHBOUR);
                    sfn_anim("C %lu %s\n",
                            n_neighbour->id, COLOUR_N_NEIGHBOUR_VISITED);
                }
            }

            for (size_t n = 0; n < neighbour->degree; ++n)
            {
                sfn_node_t const *const n_neighbour = neighbour->neighbours[n];
                size_t id = n_neighbour->id;
                if (id == node->id)
                {
                    continue;
                }
                if (neighbours_visited[id])
                {
                    sfn_anim("C %lu %s\n", id, COLOUR_NEIGHBOUR_VISITED);
                }
                else
                {
                    if (n_neighbour->id <= i)
                    {
                        sfn_anim("C %lu %s\n", id, COLOUR_HOME_VISITED);
                    }
                    else
                    {
                        sfn_anim("C %lu %s\n", id, COLOUR_HOME_UNVISITED);
                    }
                }
            }

            neighbours_visited[neighbour->id] = true;
            sfn_anim("C %lu %s\n", neighbour->id, COLOUR_NEIGHBOUR_VISITED);

        }

        for (size_t j = 0; j < node->degree; ++j)
        {
            sfn_node_t const *const neighbour = node->neighbours[j];
            if (neighbour->id <= i)
            {
                sfn_anim("C %lu %s\n", neighbour->id, COLOUR_HOME_VISITED);
            }
            else
            {
                sfn_anim("C %lu %s\n", neighbour->id, COLOUR_HOME_UNVISITED);
            }
        }

        sfn_anim("C %lu %s\n", node->id, COLOUR_HOME_VISITED);

        double const local_cc = (double)e / (double)(k * (k - 1));
        cc += local_cc;
    }
    sfn_anim("F\n");

    free(neighbours_visited);
    return cc / (double)sfn->num_nodes;

error:
    free(neighbours_visited);
    return -1.0;
}


int get_random_index(int max)
{
    return (int) round((double)(rand() * (double)max)
            / (double)RAND_MAX);
}

double approximate_clustering_coefficient(sfn_t const *const sfn,
        int const num_samples)
{
    sfn_anim("F\n");

    double cc = 0.0;

    for (int k = 0; k < num_samples; ++k)
    {
        sfn_node_t const *const i =
                &sfn->nodes[get_random_index(sfn->num_nodes - 1)];

        if (i->degree < 2)
        {
            continue;
        }

        sfn_node_t const *const j =
            i->neighbours[get_random_index(i->degree - 1)];

        sfn_node_t const *n;

        do
        {
            n = i->neighbours[get_random_index(i->degree - 1)];
        } while (j->id == n->id);

        if (sfn->adjacency[j->id * sfn->max_nodes + n->id])
        {
            cc += 1.0;
            sfn_anim("C %zu green\nC %zu green\nC %zu green\n",
                    i->id, j->id, n->id);
            sfn_anim("E %zu %zu green\nE %zu %zu green\nE %zu %zu green\nF\n",
                    i->id, j->id, i->id, n->id, j->id, n->id);
            sfn_anim("C %zu white\nC %zu white\nC %zu white\n",
                    i->id, j->id, n->id);
            sfn_anim("E %zu %zu black\nE %zu %zu black\nE %zu %zu black\n",
                    i->id, j->id, i->id, n->id, j->id, n->id);
        }
        else
        {
            sfn_anim("C %zu red\nC %zu red\nC %zu red\n",
                    i->id, j->id, n->id);
            sfn_anim("E %zu %zu red\nE %zu %zu red\nF\n",
                    i->id, j->id, i->id, n->id);
            sfn_anim("C %zu white\nC %zu white\nC %zu white\n",
                    i->id, j->id, n->id);
            sfn_anim("E %zu %zu black\nE %zu %zu black\n",
                    i->id, j->id, i->id, n->id);
        }
    }

    return cc / (double)num_samples;
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
        if (fprintf(dot_file, "%zu;", sfn->nodes[i].id) < 0)
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
                if(fprintf(dot_file, "%zu--%zu;", i,
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

/*
Read from the edges file and populate the scale free
network with nodes and edges.
*/
static int sfn_edges_init(sfn_t *const sfn, char const *const path)
{
    FILE *edges = fopen(path, "r");

    if (edges == NULL)
    {
        fprintf(stderr, "[ERROR] Cannot open edges file path.\n");
        return 1;
    }

    /*
    String buffer that contains an edge from a number to a number.
    */
    char edge[80];

    /*
    Node number that the edge specifies.
    */
    size_t node;
    size_t node_friend;

    /*
    Ignore the first line of the file that contains the number
    of heroes total.
    */
    char * fgets_result = fgets(edge, 80, edges);
    
    if( fgets_result == NULL )
    {
      fclose(edges);
      return 0;
    }

    while(fgets(edge, 80, edges) != NULL)
    {
        sscanf(edge, "%zd\t%zd", &node, &node_friend);

        if (! sfn->adjacency[node * sfn->max_nodes + node_friend])
        {
            sfn_add_link(sfn, node, node_friend);
        }
    }

    fclose(edges);
    return 0;
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

    struct arg_str *arg_edges_path = arg_strn("e", NULL, "<path>", 0, 1,
            "Path to read an edges file from.");

    struct arg_lit *arg_real = arg_lit0("R", NULL,
            "Run only deterministic algorithm.");

    struct arg_lit *arg_approx = arg_lit0("A", NULL, 
            "Run only heuristic algorithm.");

    struct arg_end *end = arg_end(10);

    void *arg_table[] = { arg_init_num_nodes, arg_init_link_prob,
                    arg_num_links, arg_time_steps, arg_dot_path, arg_seed,
                    arg_num_samples, arg_anim_path, arg_edges_path, arg_real,
                    arg_approx, end };

    int exit_code = EXIT_SUCCESS;

    sfn_t *sfn = NULL;

    if (arg_nullcheck(arg_table) != 0)
    {
        fprintf(stderr, "[ERROR] Insufficient memory for argtable.\n");
        exit_code = EXIT_FAILURE;
        goto exit;
    }

    int const num_errors = arg_parse(argc, argv, arg_table);

    if (num_errors > 0)
    {
        arg_print_errors(stderr, end, SFN_PROG_NAME);
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

    if (init_num_nodes < 1 || time_steps <= 0 || num_links > init_num_nodes
            || init_link_prob > 1 || init_link_prob <= 0 || num_links <= 0)
    {
        fprintf(stderr, "Invalid arguments.\n");
        exit_code = EXIT_FAILURE;
        goto exit;
    }

    if (arg_seed->count > 0)
    {
        fprintf(stderr, "Seeding PRNG with %d.\n", arg_seed->ival[0]);
        srand(arg_seed->ival[0]);
    }

    if (arg_anim_path->count > 0)
    {
        if ((anim = fopen(arg_anim_path->sval[0], "w")) == NULL)
        {
            fprintf(stderr, "[ERROR} Cannot open animation path.");
            exit_code = EXIT_FAILURE;
            goto exit;
        }
    }


    if (arg_edges_path->count > 0)
    {
        /*
        Read graph edges from a text file.
        */

        FILE *edges;
        if ((edges = fopen(arg_edges_path->sval[0], "r")) == NULL)
        {
            fprintf(stderr, "[ERROR} Cannot open edges file path.");
            exit_code = EXIT_FAILURE;
            goto exit;
        }

        /*
        String buffer that contains the result of the bash command execution.
        */

        char number_of_nodes[80];

        /*
        Number of nodes to allocate the data structure
        for the scale free network with.
        */
        int num_nodes = atoi(fgets(number_of_nodes, 80, edges));

        fclose(edges);

        /*
        Allocate space for the scale free network data structure.
        */
        if ((sfn = sfn_alloc(num_nodes, 0, 0)) == NULL)
        {
            fprintf(stderr, "[ERROR] Insufficient memory for sfn.\n");
            exit_code = EXIT_FAILURE;
            goto exit;
        }
        sfn->num_nodes = num_nodes;

        if (sfn_edges_init(sfn, arg_edges_path->sval[0]) != 0)
        {
            fprintf(stderr, "[ERROR] Failed at populating the network with edges.\n");
            exit_code = EXIT_FAILURE;
            goto exit;
        }
    }
    else
    {
      if ((sfn = sfn_alloc(init_num_nodes, num_links, time_steps)) == NULL)
      {
          fprintf(stderr, "[ERROR] Insufficient memory for sfn.\n");
          exit_code = EXIT_FAILURE;
          goto exit;
      }

      sfn_init(sfn, init_num_nodes, init_link_prob);
      sfn_ba(sfn, num_links, time_steps);
    }

    struct timeval before, after;
    if (! arg_real->count > 0)
    {
      gettimeofday(&before, NULL);
      double d1;
      double const approx_cc = approximate_clustering_coefficient(sfn,
                num_samples);
      gettimeofday(&after, NULL);
      d1 = (after.tv_sec - before.tv_sec) * 1000.0
              + (after.tv_usec - before.tv_usec) / 1000.0;
      printf("ACC\t%.100f\n", approx_cc);
      printf("ACCT\t%.100f\n", d1);
    }

    if (! arg_approx->count > 0)
    {
      double real_cc, d2;
      gettimeofday(&before, NULL);
      if ((real_cc = calculate_clustering_coefficient(sfn)) < 0.0)
      {
          fprintf(stderr, "[ERROR] Insufficient memory for calc coeff.\n");
          exit_code = EXIT_FAILURE;
          goto exit;
      }
      gettimeofday(&after, NULL);
      d2 = (after.tv_sec - before.tv_sec) * 1000.0
              + (after.tv_usec - before.tv_usec) / 1000.0;
      printf("CC\t%.100f\n", real_cc);
      printf("CCT\t%.100f\n", d2);
    }

    if (arg_dot_path->count > 0)
    {
        fprintf(stderr, "Writing DOT file...");
        fflush(stderr);
        if (!sfn_write_dot_file(sfn, arg_dot_path->sval[0]))
        {
            fprintf(stderr, "[ERROR] Failed to write dot file.\n");
            exit_code = EXIT_FAILURE;
            goto exit;
        }
        fprintf(stderr, "done.\n");
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

