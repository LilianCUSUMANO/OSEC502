#ifndef SIMULATION_H
#define SIMULATION_H

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <time.h>
#include <math.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"





#define H(heightmap, w, x, y) (*((heightmap) + (y) * (w) + (x)))
#define MIN(a, b) (a < b ? a : b)
#define MAX(a, b) (a > b ? a : b)
#define ABS(a) (a < 0.0 ? -a : a)
#define EPSILON 0.00001

typedef struct _vec2 
{
    double x;   /**< X-coordinate of the vector. */
    double y;   /**< Y-coordinate of the vector. */
} vec2;


struct drop {
    vec2 position;   /**< Position of the drop. */
    vec2 direction;  /**< Direction of the drop's movement. */
    int lifetime;    /**< Lifetime of the drop before it expires. */
    double velocity; /**< Velocity of the drop. */
    double water;    /**< Volume of water in the drop. */
    double sediment; /**< Amount of sediment in the drop. */
};

struct parameters {
    double inertia;    /**< Inertia factor for sediment movement (0 to 1). */
    double slope;      /**< Minimum slope for erosion (greater than `EPSILON`). */
    double capacity;   /**< Sediment capacity of a cell. */
    double deposition; /**< Sediment deposition rate (0 to 1). */
    double erosion;    /**< Sediment erosion rate (0 to 1). */
    double gravity;    /**< Gravitational constant (commonly 9.81 m/s^2). */
    double evaporation;/**< Water evaporation rate (0 to 0.5). */
    int radius;        /**< Radius of influence for erosion/deposition. */
};



struct weight {
    int used;   /**< Flag indicating whether the weight is used. */
    int x;      /**< X-coordinate of the neighboring cell. */
    int y;      /**< Y-coordinate of the neighboring cell. */
    double w;   /**< Weight of the neighbor for interpolation or calculation. */
};



/** 
 * Initializes the random number generator.
 */
void random_init();


/** 
 * Generates a random float between 0 and 1.
 * 
 * @param seed The random seed to use.
 * @return A random float between 0 and 1.
 */
double random_double(unsigned int seed);

/** 
 * Generates a random double in the range [min, max].
 * 
 * @param seed The random seed to use.
 * @param min The minimum bound of the range.
 * @param max The maximum bound of the range.
 * @return A random double in the specified range.
 */
double random_double_min_max(unsigned int seed, double min, double max);


/** 
 * Generates a random 2D vector within the specified width and height bounds.
 * 
 * @param width The width bound.
 * @param height The height bound.
 * @return A randomly generated 2D vector.
 */
vec2 random_vec2(int width, int height);


/** 
 * Computes the gradient at a given position using neighboring height values.
 * 
 * @param w Width of the heightmap.
 * @param h Pointer to the heightmap array.
 * @param pos Position at which to compute the gradient.
 * @return The computed gradient as a 2D vector.
 */
vec2 compute_gradient(int w, double *h, vec2 pos);


/** 
 * Verifies if a drop's position is within valid boundaries.
 * 
 * @param drop The drop structure containing position information.
 * @param width The width of the terrain.
 * @param height The height of the terrain.
 * @return 1 if the position is valid, 0 otherwise.
 */
int verify_drop_pos(struct drop drop, int width, int height);


/** 
 * Simulates sediment deposition at a given position.
 * 
 * @param width The width of the heightmap.
 * @param height The height of the heightmap.
 * @param heightmap 2D array representing the terrain heightmap.
 * @param pos The position to deposit sediment.
 * @param to_drop Amount of sediment to deposit.
 * @return The remaining sediment after deposition.
 */
double simulate_deposition(int width, int height, double heightmap[width][height], vec2 pos, double to_drop);

/** 
 * Simulates a single drop, considering water and sediment transport.
 * 
 * @param height The height of the terrain grid.
 * @param width The width of the terrain grid.
 * @param heightmap 2D array representing the terrain heightmap.
 * @param drop The drop structure containing drop information.
 * @param param Simulation parameters.
 */
void simulate_drop(int height, int width, double heightmap[width][height], struct drop drop, struct parameters param);


/** 
 * Simulates erosion on the terrain using a specified number of drops.
 * 
 * @param height The height of the terrain grid.
 * @param width The width of the terrain grid.
 * @param heightmap 2D array representing the terrain heightmap.
 * @param param Simulation parameters.
 * @param nb_drop The number of drops to simulate erosion with.
 */
void simulate_erosion(int height, int width, double heightmap[width][height], struct parameters param, int nb_drop);

/** 
 * Generates a heightmap with Gaussian boss peaks.
 * 
 * @param width Width of the heightmap.
 * @param height Height of the heightmap.
 * @param heightmap 2D array representing the terrain heightmap.
 * @param num_bosses Number of Gaussian boss peaks to generate.
 * @param scale Scale factor for the height values.
 * @param width_range Min and max width of the boss peaks.
 * @param amplitude_range Min and max amplitude of the boss peaks.
 */
void generate_random_heightgaussian(int width, int height, double heightmap[width][height], int num_bosses, int scale, vec2 width_range, vec2 amplitude_range);


/** 
 * Conducts erosion simulations with varying parameters and stores the results.
 * 
 * @param dir_path Base directory path for saving results.
 * @param width Width of the heightmap.
 * @param height Height of the heightmap.
 * @param num_bosses Number of Gaussian boss peaks.
 * @param scale Scale factor for height values.
 * @param width_range Range of boss peak widths.
 * @param amplitude_range Range of boss peak amplitudes.
 */
void erosion_simulation_with_param_variations(char *dir_path, int width, int height, int num_bosses, int scale, vec2 width_range, vec2 amplitude_range);


#endif // SIMULATION_H
