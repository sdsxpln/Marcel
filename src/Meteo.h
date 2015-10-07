/* Meteo.h
 *	Publish meteo information.
 *
 *	Based on OpenWeatherMap.org information
 */

#ifndef METEO_H
#define METEO_H

#ifdef METEO

#include "Marcel.h"

	/* This key has been provided by courtesy of OpenWeatherMap
	 * and is it reserved to this project.
	 * Be smart and request another one if you need your own key
	 */
#define APIKEY "eeec13daf6e332c80ff3b648fbf628aa"

extern void *process_MeteoST(void *);

#endif
#endif