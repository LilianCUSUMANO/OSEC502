#include "simulation.h"

/**
 * @brief The main function of the program.
 *
 * Initializes terrain parameters, calls functions to generate a heightmap and
 * simulate terrain erosion.
 *
 * @return int Exit status.
 */
int main()
{
    // Define the size of the terrain
    int width = 512;  // Width of the terrain
    int height = 512; // Height of the terrain

    random_init(); // Initialize random number generator

    // Create the heightmap (2D matrix)
    double heightmap[width][height];

    // Define parameters for terrain generation
    int num_bosses = 500;               // Number of bosses to add to the terrain
    int scale = 10;                     // Scaling factor for boss amplitude
    vec2 width_range = {5.0, 20.0};     // Range of width for bosses (min, max)
    vec2 amplitude_range = {1.0, 15.0}; // Range of amplitude for bosses (min, max)

    // Call the erosion simulation with parameter variations
    erosion_simulation_with_param_variations("./image", width, height, num_bosses, scale, width_range, amplitude_range);

    // // Générer le terrain aléatoire
    // generate_random_heightgaussian( width, height, heightmap,num_bosses, scale, width_range, amplitude_range);

    // save_heightmap_as_image(width, height, heightmap, "generate_random_heightgaussian.png");

    // struct parameters p = {
    //     0.1, // inertia
    //     0.001, // min_slope
    //     32, // capacity
    //     0.03, // deposition
    //     0.1, // erosion
    //     9.81, // gravity
    //     0.002, // evaporation
    //     4 // radius
    //     };

    // simulate_erosion(height,width,heightmap, p, 100000);

    // save_heightmap_as_image(width, height, heightmap, "result.png");

    return 0;
}

/**
 * @brief Creates directories recursively if they do not exist.
 *
 * @param path The path of directories to be created.
 * @return int 0 if successful, -1 if there is an error.
 */
int create_directories(const char *path)
{
    char temp[1024];
    snprintf(temp, sizeof(temp), "%s", path);
    char *p = temp;

    // Traverse the path and create directories one by one
    while ((p = strchr(p, '/')) != NULL)
    {
        *p = '\0'; // Temporarily terminate the string
        if (mkdir(temp, 0755) != 0 && errno != EEXIST)
        {
            return -1; // Error creating directory
        }
        *p = '/'; // Restore the '/' character
        p++;
    }

    // Attempt to create the last directory (or check if it exists)
    if (mkdir(temp, 0755) != 0 && errno != EEXIST)
    {
        return -1; // Error creating directory
    }
    return 0; // Success
}

/**
 * @brief Saves the heightmap as an image file.
 *
 * @param width Width of the heightmap.
 * @param height Height of the heightmap.
 * @param heightmap 2D array representing the heightmap.
 * @param filename Name of the output image file.
 */
void save_heightmap_as_image(int width, int height, double heightmap[width][height], const char *filename)
{
    // Create the necessary directories
    char dir_path[1024];
    snprintf(dir_path, sizeof(dir_path), "%s", filename);

    // Remove the last part after the final '/'
    char *last_slash = strrchr(dir_path, '/');
    if (last_slash != NULL)
    {
        *last_slash = '\0'; // Terminate before the file name
        if (create_directories(dir_path) != 0)
        {
            printf("Error creating directories for: %s\n", dir_path);
            return;
        }
    }

    // Create an integer array for the image (in RGBA format)
    unsigned char *image = (unsigned char *)malloc(width * height * 3 * sizeof(unsigned char));
    if (image == NULL)
    {
        printf("Memory allocation error for the image.\n");
        return;
    }

    // Normalize the heightmap to values between 0 and 255 and store in the image array
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            // Normalize and convert to RGB values
            unsigned char value = (unsigned char)heightmap[y][x];
            int idx = (y * width + x) * 3; // Index for the image in RGB format
            image[idx] = value;            // R
            image[idx + 1] = value;        // G
            image[idx + 2] = value;        // B
        }
    }

    // Save the image as PNG
    int result = stbi_write_png(filename, width, height, 3, image, width * 3);
    if (result)
    {
        printf("Image successfully saved as: %s\n", filename);
    }
    else
    {
        printf("Error saving the image.\n");
    }

    // Free memory
    free(image);
}

/**
 * @brief Initializes the random number generator using the current time.
 */
void random_init()
{
    srand(time(NULL)); // Initialize random seed with current time
}

/**
 * @brief PCG hash function for generating a pseudo-random value.
 *
 * @param input Input value to hash.
 * @return uint32_t Pseudo-random hash.
 */
static uint32_t PCG_Hash(uint32_t input)
{
    uint32_t state = input * 747796405u + 2891336453u;
    uint32_t word = ((state >> ((state >> 28u) + 4u)) ^ state) * 277803737u;
    return (word >> 22u) ^ word;
}

/**
 * @brief Generates a random double between 0 and 1.
 *
 * @param seed Seed for random number generation.
 * @return double Random double in the range [0, 1].
 */
double random_double(unsigned int seed)
{
    // Hash the seed
    uint32_t hash = PCG_Hash((uint32_t)seed);

    // Normalize to [0, 1]
    return (double)hash / (double)UINT32_MAX;
}

/**
 * @brief Generates a random double within a specified range [min, max].
 *
 * @param seed Seed for random number generation.
 * @param min Minimum value of the range.
 * @param max Maximum value of the range.
 * @return double Random double in the range [min, max].
 */
double random_double_min_max(unsigned int seed, double min, double max)
{
    // Normalize to [0.0, 1.0]
    double normalized = random_double(seed);
    // Scale to [min, max]
    return min + normalized * (max - min);
}

/**
 * @brief Generates a random 2D vector with components between 0 and the given dimensions.
 *
 * @param width Width of the area.
 * @param height Height of the area.
 * @return vec2 Randomly generated 2D vector.
 */
vec2 random_vec2(int width, int height)
{
    vec2 rdm;
    rdm.x = random_double_min_max(rand(), 0, width);
    rdm.y = random_double_min_max(rand(), 0, height);
    return rdm;
};

/**
 * @brief Computes the gradient at a given position on the heightmap.
 *
 * @param w Width of the heightmap.
 * @param h Heightmap.
 * @param pos Position (vec2) to compute the gradient at.
 * @return vec2 Gradient vector at the given position.
 */
vec2 compute_gradient_central(int w, double *h, vec2 pos)
{
    int x = (int)pos.x;
    int y = (int)pos.y;

    if (x < 1 || x >= w - 1 || y < 1 || y >= w - 1)
        return (vec2){0.0, 0.0};

    vec2 gradient;
    gradient.x = (H(h, w, x + 1, y) - H(h, w, x - 1, y)) * 0.5;
    gradient.y = (H(h, w, x, y + 1) - H(h, w, x, y - 1)) * 0.5;

    return gradient;
}

/**
 * @brief Computes the gradient at a given position on the heightmap.
 *
 * @param w Width of the heightmap.
 * @param h Heightmap.
 * @param pos Position (vec2) to compute the gradient at.
 * @return vec2 Gradient vector at the given position.
 */
vec2 compute_gradient(int w, double *h, vec2 pos)
{
    int x = (int)pos.x;
    int y = (int)pos.y;
    double u = 0.5;
    double v = 0.5;
    vec2 gradient;
    gradient.x = (H(h, w, x + 1, y) - H(h, w, x, y)) * (1.0 - v) +
                 (H(h, w, x + 1, y + 1) - H(h, w, x, y + 1)) * v;
    gradient.y = (H(h, w, x, y + 1) - H(h, w, x, y)) * (1.0 - u) +
                 (H(h, w, x + 1, y + 1) - H(h, w, x + 1, y)) * u;
    return gradient;
}

/**
 * @brief Verifies if a drop's position is within the valid heightmap boundaries.
 *
 * @param drop The drop object.
 * @param width Width of the heightmap.
 * @param height Height of the heightmap.
 * @return int 1 if the position is valid, 0 otherwise.
 */
int verify_drop_pos(struct drop drop, int width, int height)
{
    int x = (int)drop.position.x;
    int y = (int)drop.position.y;
    return 0 < x && x < width - 1 && 0 < y && y < height - 1;
}

/**
 * @brief Simulates sediment deposition for a drop at a given position.
 *
 * @param width Width of the heightmap.
 * @param height Height of the heightmap.
 * @param heightmap 2D heightmap array.
 * @param pos Position of the drop (vec2).
 * @param to_drop Amount of sediment to deposit.
 * @return double Amount of sediment actually deposited.
 */
double simulate_deposition(int width, int height, double heightmap[width][height], vec2 pos, double to_drop)
{
    int x1 = (int)pos.x;
    int y1 = (int)pos.y;

    int x2 = MIN(x1 + 1, width - 1);
    int y2 = MIN(y1 + 1, height - 1);

    double dx = pos.x - x1;
    double dy = pos.y - y1;

    double w11 = (1.0 - dx) * (1.0 - dy); // Weight for (x1, y1)
    double w12 = (1.0 - dx) * dy;         // Weight for (x1, y2)
    double w21 = dx * (1.0 - dy);         // Weight for (x2, y1)
    double w22 = dx * dy;                 // Weight for (x2, y2)

    double sum_w = w11 + w12 + w21 + w22;
    if (sum_w <= EPSILON)
        return 0.0;

    w11 /= sum_w;
    w12 /= sum_w;
    w21 /= sum_w;
    w22 /= sum_w;

    double sum_dropped = 0.0;
    double dropped;
    if (to_drop * w11 > 0.0)
    {
        dropped = to_drop * w11;
        H(*heightmap, width, x1, y1) += dropped;
        sum_dropped += dropped;
    }
    if (to_drop * w12 > 0.0)
    {
        dropped = to_drop * w12;
        H(*heightmap, width, x1, y2) += dropped;
        sum_dropped += dropped;
    }
    if (to_drop * w21 > 0.0)
    {
        dropped = to_drop * w21;
        H(*heightmap, width, x2, y1) += dropped;
        sum_dropped += dropped;
    }
    if (to_drop * w22 > 0.0)
    {
        dropped = to_drop * w22;
        H(*heightmap, width, x2, y2) += dropped;
        sum_dropped += dropped;
    }

    return sum_dropped;
}

/**
 * @brief Simulates a single erosion event by dropping particles on the terrain.
 *
 * @param width Width of the heightmap.
 * @param height Height of the heightmap.
 * @param heightmap 2D heightmap array.
 * @param drop The drop object (particle).
 * @param capacity Maximum sediment capacity of the terrain.
 * @param inertia Inertia factor of sediment.
 * @param slope Slope factor affecting sediment deposition.
 * @param gravity Gravity force.
 * @param evaporation Evaporation rate of sediment.
 * @return double Amount of sediment deposited.
 */
void simulate_drop(int height, int width, double heightmap[width][height], struct drop drop, struct parameters param)
{
    while (drop.lifetime > 0 && verify_drop_pos(drop, width, height) && drop.water > EPSILON)
    {
        // 0 - update lifetime
        drop.lifetime--;

        // 1 - get gradient
        vec2 gradient = compute_gradient(width, *heightmap, drop.position);

        // 2 - new direction
        vec2 new_dir;
        new_dir.x = drop.direction.x * param.inertia - gradient.x * (1.0 - param.inertia);
        new_dir.y = drop.direction.y * param.inertia - gradient.y * (1.0 - param.inertia);

        double norm = sqrt(new_dir.x * new_dir.x + new_dir.y * new_dir.y);
        while (norm <= EPSILON) // hopefully we don't stay here too long
        {
            new_dir.x = random_double(rand());
            new_dir.y = random_double(rand());
            norm = sqrt(new_dir.x * new_dir.x + new_dir.y * new_dir.y);
        }
        new_dir.x /= norm;
        new_dir.y /= norm;

        // 3 - new position
        vec2 old_pos = drop.position;
        drop.position.x = drop.position.x + new_dir.x;
        drop.position.y = drop.position.y + new_dir.y;

        // gone: went out of bound
        if (!verify_drop_pos(drop, width, height))
            break;

        // 4 - height difference h_diff
        double h_diff = H(*heightmap, width, (int)drop.position.x, (int)drop.position.y) -
                        H(*heightmap, width, (int)old_pos.x, (int)old_pos.y);

        if (h_diff > 0.0)
        {
            double to_drop;
            double keep_going = drop.sediment >= h_diff;
            if (keep_going)
            {
                to_drop = h_diff;
                to_drop -= simulate_deposition(width, height, heightmap, old_pos, to_drop);
                while (to_drop > EPSILON)
                    to_drop -= simulate_deposition(width, height, heightmap, old_pos, drop.sediment);
                break;
            }
            else
            {
                to_drop = drop.sediment;
                drop.sediment -= simulate_deposition(width, height, heightmap, old_pos, to_drop);
                while (drop.sediment > EPSILON)
                    drop.sediment -= simulate_deposition(width, height, heightmap, old_pos, drop.sediment);
                break;
            }
        }
        else
        {
            // 5 - new capacity c
            double c = MAX(-h_diff, param.slope) * drop.velocity * drop.water * param.capacity;
            if (drop.sediment >= c) // deposit
            {
                double to_drop = (drop.sediment - c) * param.deposition;
                double dropped = simulate_deposition(width, height, heightmap, old_pos, to_drop);
                drop.sediment -= dropped;
            }
            else // erode
            {
                double gain = MIN((c - drop.sediment) * param.erosion, -h_diff);
                double total_weight = 0.0;
                int size_weights = (2 * param.radius + 1) * (2 * param.radius + 1);
                struct weight weights[size_weights];
                int index = 0;
                // Iterate over the radius square
                for (int dx = -param.radius; dx <= param.radius; ++dx)
                {
                    for (int dy = -param.radius; dy <= param.radius; ++dy)
                    {

                        int x = (int)(old_pos.x + dx);
                        int y = (int)(old_pos.y + dy);

                        weights[index].used = 0;
                        if (0 <= x && x < width && 0 <= y && y < height)
                        {
                            double dist = sqrt(dx * dx + dy * dy);
                            if (dist <= (double)param.radius)
                            {
                                weights[index].w = (double)param.radius - dist;
                                weights[index].x = x;
                                weights[index].y = y;
                                total_weight += weights[index].w;
                                weights[index].used = 1;
                            }
                        }

                        ++index;
                    }
                }

                // Normalize weights and apply erosion
                if (total_weight > 0.0)
                {
                    for (int i = 0; i < size_weights; ++i)
                    {
                        if (weights[i].used)
                        {
                            float taux_evap = 0.0;
                            if (drop.position.x > width / 2)
                            {
                                taux_evap = 0.8;
                            }
                            else
                            {
                                taux_evap = 0.1;
                            }
                            double quantity = gain * (weights[i].w / total_weight) * 1;
                            H(*heightmap, width, weights[i].x, weights[i].y) -= quantity;
                            drop.sediment += quantity;
                        }
                    }
                }
            }
        }

        // 6 - new velocity
        drop.velocity = sqrt(drop.velocity * drop.velocity + ABS(h_diff) * param.gravity);

        // 7 - new water
        drop.water *= (1.0 - param.evaporation);
    }
}

/**
 * @brief Simulates erosion on the terrain with detailed particle drop events.
 *
 * @param width Width of the heightmap.
 * @param height Height of the heightmap.
 * @param num_particles Number of particles to simulate.
 * @param heightmap 2D heightmap array.
 * @param capacity Maximum sediment capacity of the terrain.
 * @param inertia Inertia factor of sediment.
 * @param slope Slope factor affecting sediment deposition.
 * @param gravity Gravity force.
 * @param evaporation Evaporation rate of sediment.
 */
void simulate_erosion_detailed(int height, int width, double heightmap[width][height], struct parameters param, int nb_drop, char *path_name, int nb_particule_before_save)
{
    random_init();
    struct drop drop;
    char name[50];
    char string_i[50];
    for (int i = 1; i <= nb_drop; ++i)
    {
        if (i % nb_particule_before_save == 0)
        {
            name[0] = '\0';
            string_i[0] = '\0';
            strcat(name, path_name);
            sprintf(string_i, "%d", i);
            strcat(name, string_i);
            strcat(name, ".png");
            // printf("%s\n", name);
            save_heightmap_as_image(width, height, heightmap, name);
        }
        drop.position = random_vec2(width, height);
        drop.direction.x = 0.0;
        drop.direction.y = 0.0;
        drop.velocity = 1.0;
        drop.water = 1.0;
        drop.sediment = 0.0;
        drop.lifetime = 1000;
        simulate_drop(height, width, heightmap, drop, param); // Directly modify heightmap here
    }
}

/**
 * Simulates erosion on a given heightmap using specified parameters and a given number of erosion drops.
 *
 * @param height Height of the terrain grid.
 * @param width Width of the terrain grid.
 * @param heightmap 2D array representing the height values.
 * @param param Structure containing erosion parameters.
 * @param nb_drop Number of erosion drops to apply.
 */
void simulate_erosion(int height, int width, double heightmap[width][height], struct parameters param, int nb_drop)
{
    char *e = "";
    simulate_erosion_detailed(height, width, heightmap, param, nb_drop, e, nb_drop);
}

/**
 * Generates a heightmap with Gaussian boss peaks.
 *
 * @param width Width of the heightmap.
 * @param height Height of the heightmap.
 * @param heightmap 2D array representing the terrain heightmap.
 * @param num_bosses Number of Gaussian boss peaks to generate.
 * @param scale Scale factor to apply to the generated height values.
 * @param width_range Min and max width of the boss peaks.
 * @param amplitude_range Min and max amplitude of the boss peaks.
 */
void generate_random_heightgaussian(int width, int height, double heightmap[width][height], int num_bosses, int scale, vec2 width_range, vec2 amplitude_range)
{
    random_init();
    // set heightmap to 0
    for (int x = 0; x < height; ++x)
    {
        for (int y = 0; y < width; ++y)
        {
            heightmap[x][y] = 0;
        }
    }
    // add num_bosses bosses
    vec2 center;
    for (int i = 0; i < num_bosses; ++i)
    {
        // position random
        center = random_vec2(width, height);

        // random width_range
        // width_range.x is min width and width_range.y is max
        double gaussian_width = random_double_min_max(rand(), width_range.x, width_range.y);

        // random amplitud
        double amplitude = random_double_min_max(rand(), amplitude_range.x, amplitude_range.y);

        // for each position of the heightmap
        // compute it's value
        double distance;
        for (int x = 0; x < height; ++x)
        {
            for (int y = 0; y < width; ++y)
            {
                // gaussian
                distance = sqrt((x - center.x) * (x - center.x) + (y - center.y) * (y - center.y));
                heightmap[x][y] += exp2(-(distance * distance) / (2 * gaussian_width * gaussian_width)) * amplitude;
            }
        }
    }

    // scale and find min & max
    double min = heightmap[0][0] * scale; // min float;
    double max = heightmap[0][0] * scale; // max float;
    for (int x = 0; x < height; ++x)
    {
        for (int y = 0; y < width; ++y)
        {
            heightmap[x][y] *= scale;
            if (heightmap[x][y] > max)
                max = heightmap[x][y];
            if (heightmap[x][y] < min)
                min = heightmap[x][y];
        }
    }

    // normalize to 0 - 255
    for (int x = 0; x < height; ++x)
    {
        for (int y = 0; y < width; ++y)
        {
            heightmap[x][y] -= min;
            heightmap[x][y] /= (max - min);
            heightmap[x][y] *= 255.0;
        }
    }
}

/**
 * Copies the heightmap from the source to the destination.
 *
 * @param w Width of the heightmap.
 * @param h Height of the heightmap.
 * @param dest Destination 2D array for the copied heightmap.
 * @param src Source 2D array to copy from.
 */
void copy_heightmap(int w, int h, double dest[w][h], double src[w][h])
{
    for (int i = 0; i < w; ++i)
    {
        for (int j = 0; j < h; ++j)
        {
            dest[i][j] = src[i][j];
        }
    }
}

/**
 * Generates a directory name by appending an index to the base directory name.
 *
 * @param dest The destination string to store the generated directory name.
 * @param i Index to append to the directory name.
 * @param name Base directory name.
 */
void name_directory(char *dest, int i, char *name)
{
    char dec[4];
    sprintf(dec, "_%d/", i);
    strcat(dest, dec);
    strcat(dest, name);
}

/**
 * Conducts erosion simulations with varying parameters, storing results in separate directories.
 *
 * @param dir_path Base directory path to store results.
 * @param width Width of the heightmap.
 * @param height Height of the heightmap.
 * @param num_bosses Number of Gaussian boss peaks to generate.
 * @param scale Scale factor for heightmap values.
 * @param width_range Range for boss peak widths (min, max).
 * @param amplitude_range Range for boss peak amplitudes (min, max).
 */
void erosion_simulation_with_param_variations(char *dir_path, int width, int height, int num_bosses, int scale, vec2 width_range, vec2 amplitude_range)
{
    random_init();

    double heightmap[width][height];
    double original[width][height];
    generate_random_heightgaussian(width, height, original, num_bosses, scale, width_range, amplitude_range);

    save_heightmap_as_image(width, height, original, "image/original.png");

    struct parameters p = {
        0.1,   // inertia // 0 and 1
        0.001, // min_slope // epsilon and greater than epsilon
        32,    // capacity // 2 8 16 32 ...
        0.001, // deposition // 0 1
        0.1,   // erosion // 0 1
        9.81,  // gravity //
        0.002, // evaporation 0 to 0.5
        4      // radius // 1ugly to 6 10 etc
    };

    // inertia
    char path[1024];
    double param_inertia[] = {0.001, 0.01, 0.1, 0.5};
    int modulo_save_image = 1000;
    int nb_drop = 100000;

    for (int i = 0; i < 4; ++i)
    {
        // edit inertia
        strcpy(path, dir_path);
        strcat(path, "/inertia");
        name_directory(path, i, "inertia");
        copy_heightmap(width, height, heightmap, original);
        p.inertia = param_inertia[i];
        simulate_erosion_detailed(height, width, heightmap, p, nb_drop, path, modulo_save_image);
    }
    p.inertia = 0.1;

    // min_slope
    strcpy(path, dir_path);
    strcat(path, "/slope/slope");
    double param_slope[] = {0.001, 0.01, 0.1};
    for (int i = 0; i < 3; ++i)
    {
        // edit min slope
        strcpy(path, dir_path);
        strcat(path, "/slope");
        name_directory(path, i, "slope");
        copy_heightmap(width, height, heightmap, original);
        p.slope = param_slope[i];
        simulate_erosion_detailed(height, width, heightmap, p, nb_drop, path, modulo_save_image);
    }
    p.slope = 0.001;

    // capacity
    strcpy(path, dir_path);
    strcat(path, "/capacity/capacity");
    double param_capacity[] = {4, 6, 32};
    for (int i = 0; i < 3; ++i)
    {
        // edit capacity
        strcpy(path, dir_path);
        strcat(path, "/capacity");
        name_directory(path, i, "capacity");
        copy_heightmap(width, height, heightmap, original);
        p.capacity = param_capacity[i];
        simulate_erosion_detailed(height, width, heightmap, p, nb_drop, path, modulo_save_image);
    }
    p.capacity = 32;

    // deposition
    strcpy(path, dir_path);
    strcat(path, "/deposition/deposition");
    double param_deposition[] = {0.001, 0.01, 0.1, 0.5};
    for (int i = 0; i < 4; ++i)
    {
        // edit deposition
        strcpy(path, dir_path);
        strcat(path, "/deposition");
        name_directory(path, i, "deposition");
        copy_heightmap(width, height, heightmap, original);
        p.deposition = param_deposition[i];
        simulate_erosion_detailed(height, width, heightmap, p, nb_drop, path, modulo_save_image);
    }
    p.deposition = 0.001;

    // erosion
    strcpy(path, dir_path);
    strcat(path, "/erosion/erosion");
    double param_erosion[] = {0.001, 0.01, 0.1, 0.5};
    for (int i = 0; i < 4; ++i)
    {
        // edit erosion
        strcpy(path, dir_path);
        strcat(path, "/erosion");
        name_directory(path, i, "erosion");
        copy_heightmap(width, height, heightmap, original);
        p.erosion = param_erosion[i];
        simulate_erosion_detailed(height, width, heightmap, p, nb_drop, path, modulo_save_image);
    }
    p.erosion = 0.1;

    // gravity
    strcpy(path, dir_path);
    strcat(path, "/gravity/gravity");
    double param_gravity[] = {9.81, 1.0};
    for (int i = 0; i < 2; ++i)
    {
        // edit gravity
        strcpy(path, dir_path);
        strcat(path, "/gravity");
        name_directory(path, i, "gravity");
        copy_heightmap(width, height, heightmap, original);
        p.gravity = param_gravity[i];
        simulate_erosion_detailed(height, width, heightmap, p, nb_drop, path, modulo_save_image);
    }
    p.gravity = 9.81;

    // evaporation
    strcpy(path, dir_path);
    strcat(path, "/evaporation/evaporation");
    double param_evaporation[] = {0.001, 0.01, 0.1, 0.2, 0.5};
    for (int i = 0; i < 5; ++i)
    {
        // edit inertia
        strcpy(path, dir_path);
        strcat(path, "/evaporation");
        name_directory(path, i, "evaporation");
        copy_heightmap(width, height, heightmap, original);
        p.evaporation = param_evaporation[i];
        simulate_erosion_detailed(height, width, heightmap, p, nb_drop, path, modulo_save_image);
    }
    p.evaporation = 0.002;

    // radius
    strcpy(path, dir_path);
    strcat(path, "/radius/radius");
    double param_radius[] = {1, 2, 4, 8};
    for (int i = 0; i < 4; ++i)
    {
        // edit radius
        strcpy(path, dir_path);
        strcat(path, "/radius");
        name_directory(path, i, "radius");
        copy_heightmap(width, height, heightmap, original);
        p.radius = param_radius[i];
        simulate_erosion_detailed(height, width, heightmap, p, nb_drop, path, modulo_save_image);
    }
    p.radius = 4;
}