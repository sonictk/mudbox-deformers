/**
 * @file		mudbox_deformers_main.h
 * @brief		Contains the entry point for this plugin.
 */
#ifndef MUDBOX_DEFORMERS_MAIN_H
#define MUDBOX_DEFORMERS_MAIN_H

#include <Mudbox/mudbox.h>
#include <QtCore/QString>

// NOTE: (sonictk) Unity build
#include "mudbox_deformers_common_math.cpp"
#include "mudbox_deformers_util.cpp"
#include "mudbox_deformers_eventhandler_node.cpp"
#include "mudbox_deformers_spherify_deformer.cpp"
#include "mudbox_deformers_bend_deformer.cpp"


static const char MUDBOX_DEFORMERS_PLUGIN_NAME[] = "Mudbox deformers";
static const char MUDBOX_DEFORMERS_PLUGIN_DESCRIPTION[] = "Various mesh deformers.";
static const char MUDBOX_DEFORMERS_PLUGIN_AUTHOR[] = "Siew Yi Liang";
static const char MUDBOX_DEFORMERS_PLUGIN_URL[] = "http://www.sonictk.com/";


/**
 * The entry point for the plugin shared library.
 */
void initializer();


#endif /* MUDBOX_DEFORMERS_MAIN_H */
