/**
 * @file   mudbox_deformers_util.h
 * @brief  Useful utilities that are used throughout the plugin.
 */
#ifndef MUDBOX_DEFORMERS_UTIL_H
#define MUDBOX_DEFORMERS_UTIL_H

#include <Mudbox/mudbox.h>


/**
 * Marks the components of the given subdivision level dirty and notifies all
 * renderers that they need to be re-drawn.
 *
 * @param subdivLevel 	A pointer to the subdivision level that is to be marked
 * 					as dirty.
 */
void markSubdivisionLevelDirty(mudbox::SubdivisionLevel *subdivLevel);


/**
 * Re-draws the given subdivision level's components in the viewport. Also updates
 * normals and vertices' adjacency.
 *
 * @param subdivLevel	The subdivision level to re-draw.
 */
void updateSubdivisionLevel(mudbox::SubdivisionLevel *subdivLevel);


/**
 * Re-draws the given subdivision level's components in the viewport. Similar to
 * ``updateSubdivisionLevel`` except that it performs faster since vertex
 * normals/adjacencies are not re-calculated.
 *
 * @param subdivLevel	The subdivision level to re-draw.
 */
void quickUpdateSubdivisionLevel(mudbox::SubdivisionLevel *subdivLevel);


/**
 * Checks if there is active geometry being selected. If not, displays a GUI dialog.
 *
 * @return	``true`` if geometry is selected, ``false`` otherwise.
 */
bool checkIfNoGeometrySelectedAndDisplayWarning();


#endif /* MUDBOX_DEFORMERS_UTIL_H */
