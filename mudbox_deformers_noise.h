/**
 * @file   mudbox_deformers_noise.h
 * @brief  Noise functions.
 */
#ifndef MUDBOX_DEFORMERS_NOISE_H
#define MUDBOX_DEFORMERS_NOISE_H


/**
 * Function to create an "ease" curve for use with perlin noise.
 * Reference implementation from https://mrl.nyu.edu/%7Eperlin/noise/
 *
 * @param t 	Input value to ease.
 *
 * @return		Eased value.
 */
float perlinFade(float t);


/**
 * Calculates the dot product of a randomly-selected gradient vector and the 8
 * location vectors used in perlin noise.
 *
 * @param hash	Used to select a random vector.
 * @param x 	X-coordinate of location vector.
 * @param y	Y-coordinate of location vector.
 * @param z	Z-coordinate of location vector.
 *
 * @return		The gradient value.
 */
float perlinGradient(int hash, float x, float y, float z);


/**
 * Function to generate perlin noise for given coordinate.
 *
 * @param x 	The X coordinate.
 * @param y	The Y coordinate.
 * @param z	The Z coordinate.
 *
 * @return		The normalized noise value.
 */
float perlin(float x, float y, float z);


#endif /* MUDBOX_DEFORMERS_NOISE_H */
