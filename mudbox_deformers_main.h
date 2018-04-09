/**
 * @file		mudbox_deformers_main.h
 * @brief		Contains the entry point for this plugin.
 */
#ifndef MUDBOX_DEFORMERS_MAIN_H
#define MUDBOX_DEFORMERS_MAIN_H

#include <Mudbox/mudbox.h>
#include <QtCore/QString>

// NOTE: (sonictk) This is after the mudbox headers since some defines will clash otherwise
#define SS_PLATFORM_LEAN
#include <ssmath/ss_platform.h>

#ifdef MessageBox
#undef MessageBox
#endif // MessageBox

// NOTE: (sonictk) Unity build
#include "mudbox_deformers_spherify_deformer.cpp"
#include "mudbox_deformers_bend_deformer.cpp"


globalVar const char MUDBOX_DEFORMERS_PLUGIN_NAME[] = "Mudbox deformers";
globalVar const char MUDBOX_DEFORMERS_PLUGIN_DESCRIPTION[] = "Various mesh deformers.";
globalVar const char MUDBOX_DEFORMERS_PLUGIN_AUTHOR[] = "Siew Yi Liang";
globalVar const char MUDBOX_DEFORMERS_PLUGIN_URL[] = "http://www.sonictk.com/";


/**
 * The entry point for the plugin shared library.
 */
void initializer();


#endif /* MUDBOX_DEFORMERS_MAIN_H */
