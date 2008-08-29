// $Id: globals.cpp b1c70e73719f 2008/08/28 15:26:05 Oliver Lau <ola@ct.heise.de> $
// Copyright (c) 2008 Oliver Lau <oliver@ersatzworld.net>
// Alle Rechte vorbehalten.

#include "./globals.h"

using namespace std;
using namespace GPS;

/// If range between start and finish exceeds 100 meters, assume
/// that track is not a circular course but goes from A to B.
const double MAX_RANGE_BETWEEN_START_AND_FINISH = 100.0;

/// Default character encoding
const std::string DEFAULT_CHARACTER_ENCODING = "UTF-8";

bool quiet = false;
int verbose = 0;
string eleFile;
string hrFile;
string eleFileCmdline;
string hrFileCmdline;
string configFile = "./config.xml";
string title;
string url;
string authorName;
string authorMail;
string authorUrl;
string encoding = DEFAULT_CHARACTER_ENCODING;
TiXmlDocument config;
time_t eleTimeOffset = 0;
time_t hrTimeOffset = 0;
vector<SmoothingOptions*> smoothings;
const string MISSING = "?";
const string gnuplotPltFile = "gnuplot.plt";
string gnuplotDatFile;
string gnuplotExe;
string gnuplotExeCmdline;
string gnuplotFormat = "png small size 640,480 enhanced interlace";
string gnuplotSuffix = "png";
string gnuplotXAxis = "distance";
time_t gnuplotSpeedIntervalSeconds = 60;
double gnuplotSpeedIntervalMeters = 100;
bool gnuplotLegend = true;
double gnuplotElevationPct = 0;
IntValue gnuplotElevationLo;
IntValue gnuplotElevationHi;
double gnuplotSpeedPct = 0;
IntValue gnuplotSpeedLo;
IntValue gnuplotSpeedHi;
bool gnuplotSpeedAverage = false;
string gnuplotSpeedSource;
double gnuplotHeartratePct = 0;
IntValue gnuplotHeartrateLo;
IntValue gnuplotHeartrateHi;
bool gnuplotHeartrateAverage = false;
double gnuplotSlopePct = 0;
IntValue gnuplotSlopeLo;
IntValue gnuplotSlopeHi;
string gnuplotSlopeSource;
string dumpFile;
string dumpFileCmdline;
string kmlFile;
bool kmlMarkStart = true;
bool kmlMarkFinish = true;
int kmlKmTicks = 5;
string googleMapsFile;
smoothedTrack_t smoothedTrack;

void errmsg(std::string str, int rc, bool _usage)
{
    std::cerr << endl << "FEHLER (" << ((rc != 0)? rc : errno) << "): " << str << endl;
    if (_usage)
        usage();
    exit(EXIT_FAILURE);
}


void warnmsg(std::string str)
{
    std::cerr << endl << "WARNUNG: " << str << endl;
}


void loadConfiguration(void)
{
    if (!config.LoadFile(configFile.c_str()))
        errmsg("Kann Konfigurationsdatei '" + configFile + "' nicht laden");
    TiXmlHandle configHandle(&config);
    TiXmlHandle cfgRoot = configHandle.FirstChild("config");

    // reading <general> configuration data
    TiXmlHandle cfgGeneral = cfgRoot.FirstChild("general");
    if (cfgGeneral.FirstChild("title").Element() != NULL)
        if (cfgGeneral.FirstChild("title").Element()->GetText() != NULL)
            title = cfgGeneral.FirstChild("title").ToElement()->GetText();
    if (cfgGeneral.FirstChild("url").Element() != NULL)
        if (cfgGeneral.FirstChild("url").Element()->GetText() != NULL)
            url = cfgGeneral.FirstChild("url").Element()->GetText();
    if (cfgGeneral.FirstChild("encoding").Element() != NULL)
        if (cfgGeneral.FirstChild("encoding").Element()->GetText() != NULL)
            encoding = cfgGeneral.FirstChild("encoding").Element()->GetText();
    if (cfgGeneral.FirstChild("author").FirstChild("name").Element() != NULL) {
        if (cfgGeneral.FirstChild("author").FirstChild("name").Element()->GetText() != NULL)
            authorName =  cfgGeneral.FirstChild("author").FirstChild("name").Element()->GetText();
        if (cfgGeneral.FirstChild("author").FirstChild("email").Element()->GetText() != NULL)
            authorMail = cfgGeneral.FirstChild("author").FirstChild("email").Element()->GetText();
        if (cfgGeneral.FirstChild("author").FirstChild("url").Element()->GetText() != NULL)
            authorUrl = cfgGeneral.FirstChild("author").FirstChild("url").Element()->GetText();
    }

    // reading <input> configuration data
    TiXmlHandle cfgInput = cfgRoot.FirstChild("input");
    TiXmlHandle cfgInputElevation = cfgInput.FirstChild("elevation");
    if (cfgInputElevation.FirstChild("file").Element() != NULL && cfgInputElevation.FirstChild("file").Element()->GetText() != NULL)
        eleFile = cfgInputElevation.FirstChild("file").Element()->GetText();
    if (cfgInputElevation.FirstChild("offset").Element() != NULL && cfgInputElevation.FirstChild("offset").Element()->GetText() != NULL)
        eleTimeOffset = Timestamp::offsetToSeconds(cfgInputElevation.FirstChild("offset").Element()->GetText());
    TiXmlHandle cfgInputHeartrate = cfgInput.FirstChild("heartrate");
    if (cfgInputHeartrate.FirstChild("file").Element() != NULL && cfgInputHeartrate.FirstChild("file").Element()->GetText() != NULL)
        hrFile = cfgInputHeartrate.FirstChild("file").Element()->GetText();
    if (cfgInputHeartrate.FirstChild("offset").Element() != NULL && cfgInputHeartrate.FirstChild("offset").Element()->GetText() != NULL)
        hrTimeOffset = Timestamp::offsetToSeconds(cfgInputHeartrate.FirstChild("offset").Element()->GetText());

    // reading <calculations> configuration data
    TiXmlNode* calcSmoothing = cfgRoot.FirstChild("calculations").FirstChild("smoothing").Node();
    while (calcSmoothing != NULL)
    {
        SmoothingOptions* opt = new SmoothingOptions;
        TiXmlElement* calcEle = calcSmoothing->ToElement();
        opt->id = calcEle->Attribute("id");
        if (calcEle->FirstChildElement("algorithm") != NULL && calcEle->FirstChildElement("algorithm")->GetText() != NULL)
            opt->algorithm = calcEle->FirstChildElement("algorithm")->GetText();
        if (calcEle->FirstChildElement("gnuplotOption") != NULL && calcEle->FirstChildElement("gnuplotOption")->GetText() != NULL)
            opt->gnuplotOption = calcEle->FirstChildElement("gnuplotOption")->GetText();
        if (calcEle->FirstChildElement("param") != NULL && calcEle->FirstChildElement("param")->GetText() != NULL)
            opt->param = atof(calcEle->FirstChildElement("param")->GetText());
        if (calcEle->FirstChildElement("draw") != NULL) {
            string draw = (calcEle->FirstChildElement("draw")->GetText() != NULL)? calcEle->FirstChildElement("draw")->GetText() : "";
            opt->draw = (draw == "true");
        }
        smoothings.push_back(opt);
        calcSmoothing = calcSmoothing->NextSibling();
    }

    // reading <gnuplot> configuration data
    TiXmlHandle cfgGnuplot = cfgRoot.FirstChild("gnuplot");
    if (cfgGnuplot.Element() != NULL) {
        if (cfgGnuplot.FirstChild("executable").Element() != NULL && cfgGnuplot.FirstChild("executable").Element()->GetText() != NULL)
            gnuplotExe = cfgGnuplot.FirstChild("executable").Element()->GetText();
        if (cfgGnuplot.FirstChild("format").Element() != NULL && cfgGnuplot.FirstChild("format").Element()->GetText() != NULL)
            gnuplotFormat = cfgGnuplot.FirstChild("format").Element()->GetText();
        if (cfgGnuplot.FirstChild("suffix").Element() != NULL && cfgGnuplot.FirstChild("suffix").Element()->GetText() != NULL)
            gnuplotSuffix = cfgGnuplot.FirstChild("suffix").Element()->GetText();
        if (cfgGnuplot.FirstChild("xaxis").Element() != NULL && cfgGnuplot.FirstChild("xaxis").Element()->GetText() != NULL)
            gnuplotXAxis = cfgGnuplot.FirstChild("xaxis").Element()->GetText();
        if (cfgGnuplot.FirstChild("speedInterval").Element() != NULL && cfgGnuplot.FirstChild("speedInterval").Element()->GetText() != NULL) {
            string str = cfgGnuplot.FirstChild("speedInterval").Element()->GetText();
            if (cfgGnuplot.FirstChild("speedInterval").Element()->Attribute("unit") != NULL) {
                string unit = cfgGnuplot.FirstChild("speedInterval").Element()->Attribute("unit");
                if (unit == "meters") {
                    gnuplotSpeedIntervalMeters = atof(str.c_str());
                    gnuplotSpeedIntervalSeconds = 0;
                }
                else if (unit == "seconds") {
                    gnuplotSpeedIntervalSeconds = atoi(str.c_str());
                    gnuplotSpeedIntervalMeters = 0;
                }
                else errmsg("Unbekannte Einheit in <speedInterval unit=\"" + unit + "\"");
            }
        }
        if (cfgGnuplot.FirstChild("legend").Element() != NULL && cfgGnuplot.FirstChild("legend").Element()->GetText() != NULL) {
            string str = cfgGnuplot.FirstChild("legend").Element()->GetText();
            gnuplotLegend = (str == "true");
        }
        if (cfgGnuplot.FirstChild("elevation").FirstChild("height").Element() != NULL && cfgGnuplot.FirstChild("elevation").FirstChild("height").Element()->GetText() != NULL)
            gnuplotElevationPct = atof(cfgGnuplot.FirstChild("elevation").FirstChild("height").Element()->GetText());
        if (cfgGnuplot.FirstChild("elevation").FirstChild("range").FirstChild("lo").Element() != NULL && cfgGnuplot.FirstChild("elevation").FirstChild("range").FirstChild("lo").Element()->GetText() != NULL)
            gnuplotElevationLo = atoi(cfgGnuplot.FirstChild("elevation").FirstChild("range").FirstChild("lo").Element()->GetText());
        if (cfgGnuplot.FirstChild("elevation").FirstChild("range").FirstChild("hi").Element() != NULL && cfgGnuplot.FirstChild("elevation").FirstChild("range").FirstChild("hi").Element()->GetText() != NULL)
            gnuplotElevationHi = atoi(cfgGnuplot.FirstChild("elevation").FirstChild("range").FirstChild("hi").Element()->GetText());

        if (cfgGnuplot.FirstChild("speed").FirstChild("height").Element() != NULL && cfgGnuplot.FirstChild("speed").FirstChild("height").Element()->GetText() != NULL)
            gnuplotSpeedPct = atof(cfgGnuplot.FirstChild("speed").FirstChild("height").Element()->GetText());
        if (cfgGnuplot.FirstChild("speed").FirstChild("range").FirstChild("lo").Element() != NULL && cfgGnuplot.FirstChild("speed").FirstChild("range").FirstChild("lo").Element()->GetText() != NULL)
            gnuplotSpeedLo = atoi(cfgGnuplot.FirstChild("speed").FirstChild("range").FirstChild("lo").Element()->GetText());
        if (cfgGnuplot.FirstChild("speed").FirstChild("range").FirstChild("hi").Element() != NULL && cfgGnuplot.FirstChild("speed").FirstChild("range").FirstChild("hi").Element()->GetText() != NULL)
            gnuplotSpeedHi = atoi(cfgGnuplot.FirstChild("speed").FirstChild("range").FirstChild("hi").Element()->GetText());
        if (cfgGnuplot.FirstChild("speed").FirstChild("average").Element() != NULL && cfgGnuplot.FirstChild("speed").FirstChild("average").Element()->GetText() != NULL) {
            string str = cfgGnuplot.FirstChild("speed").FirstChild("average").Element()->GetText();
            gnuplotSpeedAverage = (str == "true");
        }
        if (cfgGnuplot.FirstChild("speed").FirstChild("source").Element() != NULL && cfgGnuplot.FirstChild("speed").FirstChild("source").Element()->GetText() != NULL)
            gnuplotSpeedSource = cfgGnuplot.FirstChild("speed").FirstChild("source").Element()->GetText();

        if (cfgGnuplot.FirstChild("hr").FirstChild("height").Element() != NULL && cfgGnuplot.FirstChild("hr").FirstChild("height").Element()->GetText() != NULL)
            gnuplotHeartratePct = atof(cfgGnuplot.FirstChild("hr").FirstChild("height").Element()->GetText());
        if (cfgGnuplot.FirstChild("hr").FirstChild("range").FirstChild("lo").Element() != NULL && cfgGnuplot.FirstChild("hr").FirstChild("range").FirstChild("lo").Element()->GetText() != NULL)
            gnuplotHeartrateLo = atoi(cfgGnuplot.FirstChild("hr").FirstChild("range").FirstChild("lo").Element()->GetText());
        if (cfgGnuplot.FirstChild("hr").FirstChild("range").FirstChild("hi").Element() != NULL && cfgGnuplot.FirstChild("hr").FirstChild("range").FirstChild("hi").Element()->GetText() != NULL)
            gnuplotHeartrateHi = atoi(cfgGnuplot.FirstChild("hr").FirstChild("range").FirstChild("hi").Element()->GetText());
        if (cfgGnuplot.FirstChild("hr").FirstChild("average").Element() != NULL && cfgGnuplot.FirstChild("hr").FirstChild("average").Element()->GetText() != NULL) {
            string str = cfgGnuplot.FirstChild("hr").FirstChild("average").Element()->GetText();
            gnuplotHeartrateAverage = (str == "true");
        }

        if (cfgGnuplot.FirstChild("slope").FirstChild("height").Element() != NULL && cfgGnuplot.FirstChild("slope").FirstChild("height").Element()->GetText() != NULL)
            gnuplotSlopePct = atof(cfgGnuplot.FirstChild("slope").FirstChild("height").Element()->GetText());
        if (cfgGnuplot.FirstChild("slope").FirstChild("range").FirstChild("lo").Element() != NULL && cfgGnuplot.FirstChild("slope").FirstChild("range").FirstChild("lo").Element()->GetText() != NULL)
            gnuplotSlopeLo = atoi(cfgGnuplot.FirstChild("slope").FirstChild("range").FirstChild("lo").Element()->GetText());
        if (cfgGnuplot.FirstChild("slope").FirstChild("range").FirstChild("hi").Element() != NULL && cfgGnuplot.FirstChild("slope").FirstChild("range").FirstChild("hi").Element()->GetText() != NULL)
            gnuplotSlopeHi = atoi(cfgGnuplot.FirstChild("slope").FirstChild("range").FirstChild("hi").Element()->GetText());
        if (cfgGnuplot.FirstChild("slope").FirstChild("source").Element() != NULL && cfgGnuplot.FirstChild("slope").FirstChild("source").Element()->GetText() != NULL)
            gnuplotSlopeSource = cfgGnuplot.FirstChild("slope").FirstChild("source").Element()->GetText();
    }

    // reading <dump> configuration data
    TiXmlElement* cfgDumpFile = cfgRoot.FirstChild("dump").FirstChild("file").Element();
    if (cfgDumpFile != NULL && cfgDumpFile->GetText() != NULL)
            dumpFile = cfgDumpFile->GetText();

    // reading <googlemaps> configuration data
    TiXmlElement* cfgGoogleMaps = cfgRoot.FirstChild("googlemaps").FirstChild("file").Element();
    if (cfgGoogleMaps != NULL && cfgGoogleMaps->GetText() != NULL)
            googleMapsFile = cfgGoogleMaps->GetText();

    // reading <kml> configuration data
    TiXmlHandle cfgKml = cfgRoot.FirstChild("kml");
    if (cfgKml.Element() != NULL) {
        if (cfgKml.FirstChild("file").Element() != NULL && cfgKml.FirstChild("file").Element()->GetText() != NULL)
            kmlFile = cfgKml.FirstChild("file").Element()->GetText();
        if (cfgKml.FirstChild("start").Element() != NULL && cfgKml.FirstChild("start").Element()->GetText() != NULL) {
            string str = cfgKml.FirstChild("start").Element()->GetText();
            kmlMarkStart = (str == "true");
        }
        if (cfgKml.FirstChild("finish").Element() != NULL && cfgKml.FirstChild("finish").Element()->GetText() != NULL) {
            string str = cfgKml.FirstChild("finish").Element()->GetText();
            kmlMarkFinish = (str == "true");
        }
        if (cfgKml.FirstChild("ticks").Element() != NULL && cfgKml.FirstChild("ticks").Element()->GetText() != NULL) {
            const char* str = cfgKml.FirstChild("ticks").Element()->GetText();
            kmlKmTicks = atoi(str);
        }
    }
}


void disclaimer(void)
{
    std::cout << "gpsplot " << VERSION << " - Höhenprofil-Plots aus GPX- oder SDF-Dateien erzeugen." << endl
        << "Copyright (c) 2008 Oliver Lau <oliver@ersatzworld.net>" << endl
        << "Alle Rechte vorbehalten." << endl << endl;
}


void usage(void)
{
    std::cout << "Usage: gpsplot --config <Konfigurationsdatei> [Optionen]" << endl
        << "Optionen:" << endl
        << "  --ele=GPX-Datei_mit_Höheninformationen" << endl
        << "     Eintrag input/elevation/file aus Konfigurationsdatei überschreiben" << endl
        << "  --hr=SDF-Datei_mit_HF-Informationen" << endl
        << "     Eintrag input/hr/file aus Konfigurationsdatei überschreiben" << endl
        << "  --gnuplot=Pfad_zu_Gnuplot-Binary" << endl
        << "     Eintrag gnuplot/executable aus Konfigurationsdatei überschreiben" << endl
        << "  --dump=Dump-Datei" << endl
        << "     Eintrag dump/file aus Konfigurationsdatei überschreiben" << endl
        << "  --quiet" << endl
        << "     zur Laufzeit nichts in die Standardausgabe schreiben" << endl
        << "  --verbose" << endl
        << "     zusätzliche Infos zur Laufzeit ausgeben" << endl
        << "  --help" << endl
        << "     diese Hilfe ausgeben" << endl
        << endl;
}
