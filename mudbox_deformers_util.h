/**
 * @file   mudbox_deformers_util.h
 * @brief  Useful utilities that are used throughout the plugin.
 */
#ifndef MUDBOX_DEFORMERS_UTIL_H
#define MUDBOX_DEFORMERS_UTIL_H

#include <Mudbox/mudbox.h>


using mudbox::SubdivisionLevel;
using mudbox::MeshRenderer;


/**
 * Marks the components of the given subdivision level dirty and notifies all
 * renderers that they need to be re-drawn.
 *
 * @param subdivLevel 	A pointer to the subdivision level that is to be marked
 * 					as dirty.
 */
void markSubdivisionLevelDirty(SubdivisionLevel *subdivLevel);


/**
 * Re-draws the given subdivision level's components in the viewport.
 *
 * @param subdivLevel	The subdivision level to re-draw.
 */
void updateSubdivisionLevel(SubdivisionLevel *subdivLevel);


#endif /* MUDBOX_DEFORMERS_UTIL_H */
