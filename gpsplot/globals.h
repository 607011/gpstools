// $Id$
// Copyright (c) 2008 Oliver Lau <oliver@ersatzworld.net>
// Alle Rechte vorbehalten.

#ifndef __GLOBALS_H_
#define __GLOBALS_H_

#define VERSION "1.0.3"

// gettext-Dummy
#define _(x) x

#define TRACK_SELECT_BY_NAME        "name"
#define TRACK_SELECT_BY_NUMBER      "number"

#include <string>
#include <vector>
#include <ctime>

#include "tinyxml/tinyxml.h"

#include "gpslib/Value.h"
#include "gpslib/Timestamp.h"
#include "gpslib/Point.h"
#include "gpslib/Vector.h"
#include "gpslib/Trackpoint.h"
#include "gpslib/Track.h"
#include "gpslib/GPXFile.h"
#include "gpslib/SuuntoDatafile.h"
#include "gpslib/helper.h"

#include "SmoothingOptions.h"

// functions
extern void disclaimer(void);
extern void usage(void);
extern void errmsg(std::string str, int rc = 0, bool _usage = false);
extern void warnmsg(std::string str);
extern void initDefaultConfiguration(void);
extern void loadConfiguration(void);

extern void dumpData(void);
extern void writeGnuplotData(void);
extern void writeKml(void);
extern void writeGoogleMaps(void);
extern void executeGnuplot(void);

// constants & variables
extern const double MAX_RANGE_BETWEEN_START_AND_FINISH;
extern const std::string DEFAULT_CHARACTER_ENCODING;
extern int verbose;
extern bool quiet;
extern bool metricSystem;
extern bool doList;
extern std::string defaultFile;
extern std::string mergeFile;
extern std::string defaultFileCmdline;
extern std::string mergeFileCmdline;
extern std::string configFile;
extern std::string title;
extern std::string url;
extern std::string authorName;
extern std::string authorMail;
extern std::string authorUrl;
extern std::string encoding;
extern std::string trackSelector;
extern std::string trackSelectorCmdline;
extern std::string trackSelectBy;
extern int mergeWhat;
extern TiXmlDocument config;
extern time_t eleTimeOffset;
extern time_t hrTimeOffset;
extern double eleOffset;
extern std::vector<SmoothingOptions*> smoothings;
extern const std::string MISSING;
extern const std::string gnuplotPltFile;
extern std::string gnuplotDatFile;
extern std::string gnuplotExe;
extern std::string gnuplotExeCmdline;
extern std::string gnuplotFormat;
extern std::string gnuplotSuffix;
extern std::string gnuplotXAxis;
extern time_t gnuplotSpeedIntervalSeconds;
extern double gnuplotSpeedIntervalMeters;
extern bool gnuplotLegend;
extern bool gnuplotPadding;
extern double gnuplotElevationPct;
extern GPS::IntValue gnuplotElevationLo;
extern GPS::IntValue gnuplotElevationHi;
extern double gnuplotSpeedPct;
extern GPS::IntValue gnuplotSpeedLo;
extern GPS::IntValue gnuplotSpeedHi;
extern bool gnuplotSpeedAverage;
extern std::string gnuplotSpeedSource;
extern double gnuplotHeartratePct;
extern GPS::IntValue gnuplotHeartrateLo;
extern GPS::IntValue gnuplotHeartrateHi;
extern bool gnuplotHeartrateAverage;
extern double gnuplotSlopePct;
extern GPS::IntValue gnuplotSlopeLo;
extern GPS::IntValue gnuplotSlopeHi;
extern std::string gnuplotSlopeSource;
extern std::string dumpFile;
extern std::string dumpFileCmdline;
extern std::string kmlFile;
extern std::string staticImagesUrl;
extern bool kmlMarkStart;
extern bool kmlMarkFinish;
extern int kmlKmTicks;
extern std::string googleMapsFile;
extern smoothedTrack_t smoothedTrack;

extern GPS::Track* trk;

#endif // __GLOBALS_H_
