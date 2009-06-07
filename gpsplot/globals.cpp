// $Id$
// Copyright (c) 2008-2009 Oliver Lau <oliver@von-und-fuer-lau.de>
// Alle Rechte vorbehalten.

#include "./globals.h"

/// If range between start and finish exceeds 100 meters, assume
/// that track is not a circular course but goes from A to B.
const double MAX_RANGE_BETWEEN_START_AND_FINISH = 100.0;

/// Default character encoding
const std::string DEFAULT_CHARACTER_ENCODING = "UTF-8";

bool quiet = false;
int verbose = 0;
bool metricSystem = true;
bool doList = false;
std::string defaultFile;
std::string mergeFile;
std::string defaultFileCmdline;
std::string mergeFileCmdline;
std::string configFile;
std::string title;
std::string url;
std::string authorName;
std::string authorMail;
std::string authorUrl;
std::string encoding = DEFAULT_CHARACTER_ENCODING;
std::string trackSelector;
std::string trackSelectorCmdline;
std::string trackSelectBy;
int mergeWhat = 0;
TiXmlDocument config;
time_t eleTimeOffset = 0;
time_t hrTimeOffset = 0;
GPS::DoubleValue eleOffset;
std::vector<SmoothingOptions*> smoothings;
const std::string MISSING = "?";
const std::string gnuplotPltFile = "gnuplot.plt";
std::string gnuplotDatFile;
#ifdef _DEBUG
std::string gnuplotExe = "I:\\Programme\\gnuplot\\bin\\wgnuplot.exe";
#else
std::string gnuplotExe = "C:\\Programme\\gnuplot\\bin\\wgnuplot.exe";
#endif
std::string gnuplotExeCmdline;
std::string gnuplotFormat = "png small size 800,600 enhanced interlace";
std::string gnuplotSuffix = "png";
std::string gnuplotXAxis = "distance";
time_t gnuplotSpeedIntervalSeconds = 0;
double gnuplotSpeedIntervalMeters = 0;
bool gnuplotLegend = true;
bool gnuplotPadding = false;
double gnuplotElevationPct = 60;
GPS::IntValue gnuplotElevationLo;
GPS::IntValue gnuplotElevationHi;
double gnuplotSpeedPct = 20;
GPS::IntValue gnuplotSpeedLo;
GPS::IntValue gnuplotSpeedHi;
bool gnuplotSpeedAverage = true;
std::string gnuplotSpeedSource;
double gnuplotHeartratePct = 0;
GPS::IntValue gnuplotHeartrateLo;
GPS::IntValue gnuplotHeartrateHi;
bool gnuplotHeartrateAverage = false;
double gnuplotSlopePct = 20;
GPS::IntValue gnuplotSlopeLo;
GPS::IntValue gnuplotSlopeHi;
std::string gnuplotSlopeSource;
std::string dumpFile;
std::string dumpFileCmdline;
std::string kmlFile;
std::string staticImagesUrl = "http://von-und-fuer-lau.de/images";
bool kmlMarkStart = true;
bool kmlMarkFinish = true;
int kmlKmTicks = 5;
std::string googleMapsFile;
smoothedTrack_t smoothedTrack;

GPS::Track* trk = NULL;


void errmsg(std::string str, int rc, bool _usage)
{
    std::cerr << std::endl << _("FEHLER") << " (" << ((rc != 0)? rc : errno) << "): " << str << std::endl;
    if (_usage)
        usage();
    exit(EXIT_FAILURE);
}


void warnmsg(std::string str)
{
    std::cerr << std::endl << _("WARNUNG") << ": " << str << std::endl;
}


void initDefaultConfiguration(void)
{
    smoothings.push_back(new SmoothingOptions("Original", true, std::string(), 0.0, "with lines lw 2 lc rgb \"#E77171\""));
    smoothings.push_back(new SmoothingOptions("Threshold (3m)", false, "threshold", 3.0));
    smoothings.push_back(new SmoothingOptions("Douglas-Peucker (3m)", false, "douglas_peucker", 3.0));
    gnuplotSlopeSource = smoothings.back()->id;
    smoothings.push_back(new SmoothingOptions("Douglas-Peucker (5m)", true, "douglas_peucker", 5.0, "with points pt 5 ps 0.3"));
    smoothings.push_back(new SmoothingOptions("Douglas-Peucker (8m)", false, "douglas_peucker", 8.0));
}


void loadConfiguration(void)
{
    if (!config.LoadFile(configFile.c_str()))
        errmsg(_("Laden der Konfigurationsdatei fehlgeschlagen"));
    TiXmlHandle configHandle(&config);
    TiXmlHandle cfgRoot = configHandle.FirstChild("config");

    // reading <general> configuration data
    TiXmlHandle cfgGeneral = cfgRoot.FirstChild("general");
    if (cfgGeneral.FirstChild("title").Element() != NULL)
        if (cfgGeneral.FirstChild("title").Element()->GetText() != NULL)
            title = cfgGeneral.FirstChild("title").ToElement()->GetText();
    if (cfgGeneral.FirstChild("url").Element() != NULL)
    {
        if (cfgGeneral.FirstChild("url").Element()->GetText() != NULL) 
            url = cfgGeneral.FirstChild("url").Element()->GetText();
    }
    if (cfgGeneral.FirstChild("encoding").Element() != NULL)
    {
        if (cfgGeneral.FirstChild("encoding").Element()->GetText() != NULL)
            encoding = cfgGeneral.FirstChild("encoding").Element()->GetText();
    }
    if (cfgGeneral.FirstChild("author").FirstChild("name").Element() != NULL)
    {
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
        defaultFile = cfgInputElevation.FirstChild("file").Element()->GetText();
    if (cfgInputElevation.FirstChild("offset").Element() != NULL && cfgInputElevation.FirstChild("offset").Element()->GetText() != NULL)
        eleTimeOffset = GPS::Timestamp::offsetToSeconds(cfgInputElevation.FirstChild("offset").Element()->GetText());
    if (cfgInputElevation.FirstChild("select").Element() != NULL && cfgInputElevation.FirstChild("select").Element()->GetText() != NULL)
    {
        trackSelector = cfgInputElevation.FirstChild("select").Element()->GetText();
        trackSelectBy = cfgInputElevation.FirstChild("select").Element()->Attribute("by");
    }
    TiXmlHandle cfgInputHeartrate = cfgInput.FirstChild("merge");
    if (cfgInputHeartrate.FirstChild("file").Element() != NULL && cfgInputHeartrate.FirstChild("file").Element()->GetText() != NULL)
        mergeFile = cfgInputHeartrate.FirstChild("file").Element()->GetText();
    if (cfgInputHeartrate.FirstChild("offset").Element() != NULL && cfgInputHeartrate.FirstChild("offset").Element()->GetText() != NULL)
        hrTimeOffset = GPS::Timestamp::offsetToSeconds(cfgInputHeartrate.FirstChild("offset").Element()->GetText());

	for (TiXmlElement* cfgInputHeartrateUseEle = cfgInputHeartrate.FirstChild("use").ToElement();
        cfgInputHeartrateUseEle != NULL;
        cfgInputHeartrateUseEle = cfgInputHeartrateUseEle->NextSiblingElement())
	{
        if (cfgInputHeartrateUseEle->GetText() != NULL)
        {
            std::string useStr = cfgInputHeartrateUseEle->GetText();
            if (useStr == "heartrate")      { mergeWhat |= GPS::Track::HEARTRATE; }
            else if (useStr == "elevation") { mergeWhat |= GPS::Track::ELEVATION; }
            else if (useStr == "geocoords") { mergeWhat |= GPS::Track::GEOCOORDS; }
            else errmsg(_("Unbekannte Merge-Option: <merge><use>") + useStr + "</use></merge>");
        }
	}

    // reading <calculations> configuration data
    TiXmlNode* calcSmoothing = cfgRoot.FirstChild("calculations").FirstChild("smoothing").Node();
    if (calcSmoothing != NULL)
        smoothings.clear();
    while (calcSmoothing != NULL)
    {
        TiXmlElement* calcEle = calcSmoothing->ToElement();
        if (calcEle != NULL)
        {
            SmoothingOptions* opt = new SmoothingOptions;
            opt->id = calcEle->Attribute("id");
            if (calcEle->FirstChildElement("algorithm") != NULL && calcEle->FirstChildElement("algorithm")->GetText() != NULL)
                opt->algorithm = calcEle->FirstChildElement("algorithm")->GetText();
            if (calcEle->FirstChildElement("gnuplotOption") != NULL && calcEle->FirstChildElement("gnuplotOption")->GetText() != NULL)
                opt->gnuplotOption = calcEle->FirstChildElement("gnuplotOption")->GetText();
            if (calcEle->FirstChildElement("param") != NULL && calcEle->FirstChildElement("param")->GetText() != NULL)
                opt->param = atof(calcEle->FirstChildElement("param")->GetText());
            if (calcEle->FirstChildElement("draw") != NULL)
            {
                std::string draw = (calcEle->FirstChildElement("draw")->GetText() != NULL)? calcEle->FirstChildElement("draw")->GetText() : "";
                opt->draw = (draw == "true");
            }
            smoothings.push_back(opt);
        }
        calcSmoothing = calcSmoothing->NextSibling();
    }

    // reading <gnuplot> configuration data
    TiXmlHandle cfgGnuplot = cfgRoot.FirstChild("gnuplot");
    if (cfgGnuplot.Element() != NULL)
    {
        if (cfgGnuplot.FirstChild("executable").Element() != NULL && cfgGnuplot.FirstChild("executable").Element()->GetText() != NULL)
            gnuplotExe = cfgGnuplot.FirstChild("executable").Element()->GetText();
        if (cfgGnuplot.FirstChild("format").Element() != NULL && cfgGnuplot.FirstChild("format").Element()->GetText() != NULL)
            gnuplotFormat = cfgGnuplot.FirstChild("format").Element()->GetText();
        if (cfgGnuplot.FirstChild("suffix").Element() != NULL && cfgGnuplot.FirstChild("suffix").Element()->GetText() != NULL)
            gnuplotSuffix = cfgGnuplot.FirstChild("suffix").Element()->GetText();
        if (cfgGnuplot.FirstChild("xaxis").Element() != NULL && cfgGnuplot.FirstChild("xaxis").Element()->GetText() != NULL)
            gnuplotXAxis = cfgGnuplot.FirstChild("xaxis").Element()->GetText();
        if (cfgGnuplot.FirstChild("speedInterval").Element() != NULL && cfgGnuplot.FirstChild("speedInterval").Element()->GetText() != NULL) {
            std::string str = cfgGnuplot.FirstChild("speedInterval").Element()->GetText();
            if (cfgGnuplot.FirstChild("speedInterval").Element()->Attribute("unit") != NULL) { // deprecated
                warnmsg(_("Verwenden Sie <speed>/<interval> statt <speedInterval>"));
                std::string unit = cfgGnuplot.FirstChild("speedInterval").Element()->Attribute("unit");
                if (unit == "meters")
                {
                    gnuplotSpeedIntervalMeters = atof(str.c_str());
                    gnuplotSpeedIntervalSeconds = 0;
                }
                else if (unit == "seconds")
                {
                    gnuplotSpeedIntervalSeconds = atoi(str.c_str());
                    gnuplotSpeedIntervalMeters = 0;
                }
                else errmsg(_("Unbekannte Einheit in <speedInterval unit=\")") + unit + "\"");
            }
        }
        if (cfgGnuplot.FirstChild("padding").Element() != NULL && cfgGnuplot.FirstChild("padding").Element()->GetText() != NULL)
        {
            std::string str = cfgGnuplot.FirstChild("padding").Element()->GetText();
            gnuplotPadding = (str == "true");
        }
        if (cfgGnuplot.FirstChild("legend").Element() != NULL && cfgGnuplot.FirstChild("legend").Element()->GetText() != NULL)
        {
            std::string str = cfgGnuplot.FirstChild("legend").Element()->GetText();
            gnuplotLegend = (str == "true");
        }
        if (cfgGnuplot.FirstChild("elevation").FirstChild("height").Element() != NULL && cfgGnuplot.FirstChild("elevation").FirstChild("height").Element()->GetText() != NULL)
            gnuplotElevationPct = atof(cfgGnuplot.FirstChild("elevation").FirstChild("height").Element()->GetText());
        if (cfgGnuplot.FirstChild("elevation").FirstChild("range").FirstChild("lo").Element() != NULL && cfgGnuplot.FirstChild("elevation").FirstChild("range").FirstChild("lo").Element()->GetText() != NULL)
            gnuplotElevationLo = atoi(cfgGnuplot.FirstChild("elevation").FirstChild("range").FirstChild("lo").Element()->GetText());
        if (cfgGnuplot.FirstChild("elevation").FirstChild("range").FirstChild("hi").Element() != NULL && cfgGnuplot.FirstChild("elevation").FirstChild("range").FirstChild("hi").Element()->GetText() != NULL)
            gnuplotElevationHi = atoi(cfgGnuplot.FirstChild("elevation").FirstChild("range").FirstChild("hi").Element()->GetText());
        if (cfgGnuplot.FirstChild("elevation").FirstChild("offset").Element() != NULL && cfgGnuplot.FirstChild("elevation").FirstChild("offset").Element()->GetText() != NULL)
            eleOffset = atof(cfgGnuplot.FirstChild("elevation").FirstChild("offset").Element()->GetText());

        if (cfgGnuplot.FirstChild("speed").FirstChild("height").Element() != NULL && cfgGnuplot.FirstChild("speed").FirstChild("height").Element()->GetText() != NULL)
            gnuplotSpeedPct = atof(cfgGnuplot.FirstChild("speed").FirstChild("height").Element()->GetText());
        if (cfgGnuplot.FirstChild("speed").FirstChild("range").FirstChild("lo").Element() != NULL && cfgGnuplot.FirstChild("speed").FirstChild("range").FirstChild("lo").Element()->GetText() != NULL)
            gnuplotSpeedLo = atoi(cfgGnuplot.FirstChild("speed").FirstChild("range").FirstChild("lo").Element()->GetText());
        if (cfgGnuplot.FirstChild("speed").FirstChild("range").FirstChild("hi").Element() != NULL && cfgGnuplot.FirstChild("speed").FirstChild("range").FirstChild("hi").Element()->GetText() != NULL)
            gnuplotSpeedHi = atoi(cfgGnuplot.FirstChild("speed").FirstChild("range").FirstChild("hi").Element()->GetText());
        if (cfgGnuplot.FirstChild("speed").FirstChild("average").Element() != NULL && cfgGnuplot.FirstChild("speed").FirstChild("average").Element()->GetText() != NULL)
        {
            std::string str = cfgGnuplot.FirstChild("speed").FirstChild("average").Element()->GetText();
            gnuplotSpeedAverage = (str == "true");
        }
        if (cfgGnuplot.FirstChild("speed").FirstChild("source").Element() != NULL && cfgGnuplot.FirstChild("speed").FirstChild("source").Element()->GetText() != NULL)
            gnuplotSpeedSource = cfgGnuplot.FirstChild("speed").FirstChild("source").Element()->GetText();

        if (cfgGnuplot.FirstChild("speed").FirstChild("interval").Element() != NULL && cfgGnuplot.FirstChild("speed").FirstChild("interval").Element()->GetText() != NULL)
        {
            std::string str = cfgGnuplot.FirstChild("speed").FirstChild("interval").Element()->GetText();
            if (cfgGnuplot.FirstChild("speed").FirstChild("interval").Element()->Attribute("unit") != NULL)
            {
                std::string unit = cfgGnuplot.FirstChild("speed").FirstChild("interval").Element()->Attribute("unit");
                if (unit == "meters")
                {
                    gnuplotSpeedIntervalMeters = atof(str.c_str());
                    gnuplotSpeedIntervalSeconds = 0;
                }
                else if (unit == "seconds")
                {
                    gnuplotSpeedIntervalSeconds = atoi(str.c_str());
                    gnuplotSpeedIntervalMeters = 0;
                }
                else errmsg(_("Unbekannte Einheit in <speed>/<interval unit=\")") + unit + "\">");
            }
        }

        if (cfgGnuplot.FirstChild("hr").FirstChild("height").Element() != NULL && cfgGnuplot.FirstChild("hr").FirstChild("height").Element()->GetText() != NULL)
            gnuplotHeartratePct = atof(cfgGnuplot.FirstChild("hr").FirstChild("height").Element()->GetText());
        if (cfgGnuplot.FirstChild("hr").FirstChild("range").FirstChild("lo").Element() != NULL && cfgGnuplot.FirstChild("hr").FirstChild("range").FirstChild("lo").Element()->GetText() != NULL)
            gnuplotHeartrateLo = atoi(cfgGnuplot.FirstChild("hr").FirstChild("range").FirstChild("lo").Element()->GetText());
        if (cfgGnuplot.FirstChild("hr").FirstChild("range").FirstChild("hi").Element() != NULL && cfgGnuplot.FirstChild("hr").FirstChild("range").FirstChild("hi").Element()->GetText() != NULL)
            gnuplotHeartrateHi = atoi(cfgGnuplot.FirstChild("hr").FirstChild("range").FirstChild("hi").Element()->GetText());
        if (cfgGnuplot.FirstChild("hr").FirstChild("average").Element() != NULL && cfgGnuplot.FirstChild("hr").FirstChild("average").Element()->GetText() != NULL) {
            std::string str = cfgGnuplot.FirstChild("hr").FirstChild("average").Element()->GetText();
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
        if (cfgKml.FirstChild("start").Element() != NULL && cfgKml.FirstChild("start").Element()->GetText() != NULL)
        {
            std::string str = cfgKml.FirstChild("start").Element()->GetText();
            kmlMarkStart = (str == "true");
        }
        if (cfgKml.FirstChild("finish").Element() != NULL && cfgKml.FirstChild("finish").Element()->GetText() != NULL)
        {
            std::string str = cfgKml.FirstChild("finish").Element()->GetText();
            kmlMarkFinish = (str == "true");
        }
        if (cfgKml.FirstChild("ticks").Element() != NULL && cfgKml.FirstChild("ticks").Element()->GetText() != NULL)
        {
            const char* str = cfgKml.FirstChild("ticks").Element()->GetText();
            kmlKmTicks = atoi(str);
        }
    }
}


void disclaimer(void)
{
    std::cout << "gpsplot " << VERSION << " - " << _("Höhenprofil-Plots aus GPX- oder SDF-Dateien erzeugen.") << std::endl
        << "Copyright (c) 2008-2009 Oliver Lau <oliver@von-und-fuer-lau.de>" << std::endl
        << _("Alle Rechte vorbehalten.") << std::endl << std::endl;
}


void usage(void)
{
    std::cout << _("Aufruf: gpsplot --config <Konfigurationsdatei> [Optionen]") << std::endl
        << _("Optionen:") << std::endl
        << "  " << _("--ele=GPX-Datei_mit_Höheninformationen") << std::endl
        << "     " << _("Eintrag input/elevation/file aus Konfigurationsdatei überschreiben") << std::endl
        << "  " << _("--hr=SDF-Datei_mit_HF-Informationen") << std::endl
        << "     " << _("Eintrag input/hr/file aus Konfigurationsdatei überschreiben") << std::endl
        << "  " << _("--gnuplot=Pfad_zu_Gnuplot-Binary") << std::endl
        << "     " << _("Eintrag gnuplot/executable aus Konfigurationsdatei überschreiben") << std::endl
        << "  " << _("--eleoffset=Korrektur der Höhendaten in Metern (Dezimalzahl)") << std::endl
        << "     " << _("Eintrag input/elevation/offset aus Konfigurationsdatei überschreiben") << std::endl
        << "  " << _("--dump=Dump-Datei") << std::endl
        << "     " << _("Eintrag dump/file aus Konfigurationsdatei überschreiben") << std::endl
        << "  " << _("--quiet") << std::endl
        << "     " << _("zur Laufzeit nichts in die Standardausgabe schreiben") << std::endl
        << "  " << _("--verbose") << std::endl
        << "     " << _("zusätzliche Infos zur Laufzeit ausgeben") << std::endl
        << "  " << _("--help") << std::endl
        << "     " << _("diese Hilfe ausgeben") << std::endl
        << std::endl;
}
